#include "../include/Blur.h"
#include "../include/Mat2QImage.h"
#include <QDebug>
#include <QLabel>

Blur::Blur(QWidget* parent)
	:ori_mt(),_mt(), _img(), QWidget(parent)
{
}

Blur::Blur(const std::string& fileName, QWidget* parent)
	:ori_mt(cv::imread(fileName)), _img(Mat2QImage(ori_mt)),QWidget(parent)
{
	ori_mt.copyTo(_mt);
}

Blur::Blur(const cv::Mat& mt, QWidget* parent)
	:QWidget(parent)
{
	mt.copyTo(ori_mt);
	ori_mt.copyTo(_mt);
	_img = Mat2QImage(_mt);
}

Blur::~Blur()
{
}

void Blur::savePoint()
{
	//QUndoStack
	_mt.copyTo(savePoint_mt);
	
}

void Blur::returnPoint()
{
	savePoint_mt.copyTo(_mt);
}

void Blur::update(const cv::Mat& newMt)
{
	newMt.copyTo(ori_mt);
	restore(); 
}

void Blur::restore()
{
	//重做 / 初始化
	ori_mt.copyTo(_mt);  //恢复原始图片
	_img = Mat2QImage(_mt);

	//均值滤波
	avg_Ksize = 1, anchorX = -1, anchorY = -1;
	//高斯滤波
	gas_Ksize = 7, sigmaX = -1, sigmaY = -1;
	//中值滤波
	median_Ksize = 5;
	//双边滤波
	bin_d = 5, sigmaColor = 10, sigmaSpace = 10;
}


QImage Blur::avg_blur()
{
	if (avg_Ksize <= 1) {
		return _img;
	}
	qInfo() << "均值滤波";

	cv::Mat tMt;

	cv::blur(_mt, tMt, cv::Size(avg_Ksize, avg_Ksize), cv::Point(anchorX, anchorY));
	

	if (mode) {
		//创造模式
		_mt = tMt;
	}
	_img = Mat2QImage(tMt);
	return _img;
}

QImage Blur::Gaussian_blur()
{
	if (gas_Ksize <= 1) {
		return _img;//return Mat2QImage(tMt);
	}
	qInfo() << "高斯滤波";
	cv::Mat tMt;

	cv::GaussianBlur(_mt, tMt, cv::Size(gas_Ksize, gas_Ksize), sigmaX, sigmaY);
	
	if (mode) {
		_mt = tMt;
	}
	_img = Mat2QImage(tMt);
	return _img;
}

QImage Blur::median_blur()
{
	qInfo() << "中值滤波";

	cv::Mat tMt{};
	cv::medianBlur(_mt, tMt, median_Ksize);
	if (mode) {
		_mt = tMt;
	}
	_img = Mat2QImage(tMt);
	return _img;
}

QImage Blur::bilateral_blur()
{
	if (_mt.type() != CV_8UC1 && _mt.type() != CV_8UC3) {
		return QImage(_img = Mat2QImage(_mt));
	}
	qInfo() << "双边滤波";

	cv::Mat tMt;

	cv::bilateralFilter(_mt, tMt, bin_d, sigmaColor, sigmaSpace);
	if (mode) {
		_mt = tMt; //保存操作后的图像
	}
	_img = Mat2QImage(tMt);
	return _img;
}

//-----------均值滤波--------------
void Blur::onTriggered_slider1_valueChange_avgBlur(int value) {
	if (-1 <= anchorX && anchorX < value && -1 <= anchorY && anchorY < value) {
		avg_Ksize = value;
		avg_blur(); //type = 1：制作模式  type = 0 单图操作模式
	}
}

void Blur::onTriggered_slider2_valueChange_avgBlur(int value) {
	if (-1 <= value && value < avg_Ksize && -1 <= anchorY && anchorY < avg_Ksize) {
		anchorX = value;
		avg_blur();
	}
}

void Blur::onTriggered_slider3_valueChange_avgBlur(int value) {
	if (-1 <= anchorX && anchorX < avg_Ksize && -1 <= value && value < avg_Ksize ) {
		anchorY = value;
		avg_blur();
	}
}


//-----------高斯滤波--------------
void Blur::onTriggered_slider1_valueChange_gaussianBlur(int value) {
	if (-1 <= sigmaX && sigmaX < value  && -1 <= sigmaY && sigmaY < value ) {
		gas_Ksize = value;
		Gaussian_blur();
	}
}

void Blur::onTriggered_slider2_valueChange_gaussianBlur(int value) {
	if (-1 <= value && value < gas_Ksize  && -1 <= sigmaY && sigmaY < gas_Ksize ) {
		sigmaX = value;
		Gaussian_blur();
	}
}

void Blur::onTriggered_slider3_valueChange_gaussianBlur(int value) {
	if (-1 <= sigmaX && sigmaX < gas_Ksize  && -1 <= value && value < gas_Ksize ) {
		sigmaY = value;
		Gaussian_blur();
	}
}

void Blur::onTriggered_slider_valueChange_medianBlur(int value)
{
	median_Ksize = value;
	median_blur();
}

void Blur::onTriggered_slider1_valueChange_bilateralBlur(int value)
{
	bin_d = value;
	bilateral_blur();
}

void Blur::onTriggered_slider2_valueChange_bilateralBlur(int value)
{
	sigmaColor = value;
	bilateral_blur();
}

void Blur::onTriggered_slider3_valueChange_bilateralBlur(int value)
{
	sigmaSpace = value;
	bilateral_blur();
}