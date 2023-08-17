#include "../include/Blur.h"
#include "../include/Mat2QImage.h"
#include <QDebug>
#include <QLabel>

Blur::Blur() {}

Blur::Blur(const std::string& fileName)
	:Object(fileName) {}

Blur::Blur(const cv::Mat& mt)
	:Object(mt) {}

Blur::~Blur() {}

void Blur::initialize()
{
	//均值滤波
	avg_Ksize = 1, anchorX = -1, anchorY = -1;
	//高斯滤波
	gas_Ksize = 7, sigmaX = -1, sigmaY = -1;
	//中值滤波
	median_Ksize = 5;
	//双边滤波
	bin_d = 5, sigmaColor = 10, sigmaSpace = 10;
}

void Blur::restore() {
	Object::restore();
	initialize();
}

QImage Blur::avg_blur()
{
	if (avg_Ksize <= 1) {
		return _img;
	}

	cv::Mat tMt;
	//对预览图操作
	cv::blur(_mt, tMt, cv::Size(avg_Ksize, avg_Ksize), cv::Point(anchorX, anchorY));
	

	if (mode) {
		_mt = tMt;
	}
	_img = Mat2QImage(tMt); //200*200
	return _img;
}

QImage Blur::Gaussian_blur()
{
	if (gas_Ksize <= 1) {
		return _img;
	}
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