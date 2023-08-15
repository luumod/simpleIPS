#include "../include/Blur.h"
#include "../include/Mat2QImage.h"
#include <QDebug>
#include <QLabel>

Blur::Blur(QWidget* parent)
	:_mt(), _img(), QWidget(parent)
{
}

Blur::Blur(const std::string& fileName, QWidget* parent)
	:_mt(cv::imread(fileName)), _img(Mat2QImage(_mt)),QWidget(parent)
{
}

Blur::Blur(const cv::Mat& mt, QWidget* parent)
	:QWidget(parent)
{
	mt.copyTo(_mt);
	_img = Mat2QImage(_mt);
}

Blur::~Blur()
{
}



QImage Blur::avg_blur()
{
	if (avg_Ksize <= 1) {
		return _img;//return Mat2QImage(tMt);
	}
	qInfo() << "均值滤波";

	if (type) {
		//制作模式
		cv::blur(_mt, _mt, cv::Size(avg_Ksize, avg_Ksize), cv::Point(anchorX, anchorY));
		_img = Mat2QImage(_mt);
	}
	else {
		//预览模式
		cv::Mat tMt{};// = cv::Mat::zeros(_mt.size(), _mt.type());
		_mt.copyTo(tMt);
		cv::blur(_mt, tMt, cv::Size(avg_Ksize, avg_Ksize), cv::Point(anchorX, anchorY));
		_img = Mat2QImage(tMt);
	}
	return _img;
}

QImage Blur::Gaussian_blur()
{
	if (gas_Ksize <= 1) {
		return _img;//return Mat2QImage(tMt);
	}
	qInfo() << "高斯滤波";

	if (type) {
		cv::GaussianBlur(_mt, _mt, cv::Size(gas_Ksize, gas_Ksize), sigmaX, sigmaY);
		_img = Mat2QImage(_mt);
	}
	else {
		cv::Mat tMt{};
		_mt.copyTo(tMt);

		cv::GaussianBlur(_mt, tMt, cv::Size(gas_Ksize, gas_Ksize), sigmaX, sigmaY);

		_img = Mat2QImage(tMt);
	}
	return _img;
}

QImage Blur::median_blur()
{
	qInfo() << "中值滤波";


	if (type) {
		cv::medianBlur(_mt, _mt, median_Ksize);
		_img = Mat2QImage(_mt);
	}
	else {
		cv::Mat tMt{};
		_mt.copyTo(tMt);
		cv::medianBlur(_mt, tMt, median_Ksize);

		_img = Mat2QImage(tMt);
	}
	

	return _img;
}

QImage Blur::bilateral_blur()
{
	qInfo() << "双边滤波";

	cv::Mat tMt{};
	_mt.copyTo(tMt);

	cv::bilateralFilter(_mt, tMt, bin_d,sigmaColor, sigmaSpace);

	_img = Mat2QImage(tMt);

	return _img;
}

//-----------均值滤波--------------
void Blur::onTriggered_slider1_valueChange_avgBlur(int value) {
	if (-1 <= anchorX && anchorX < value - 3 && -1 <= anchorY && anchorY < value - 3) {
		avg_Ksize = value;
		avg_blur(); //type = 1：制作模式  type = 0 单图操作模式
	}
}

void Blur::onTriggered_slider2_valueChange_avgBlur(int value) {
	if (-1 <= value && value < avg_Ksize - 3 && -1 <= anchorY && anchorY < avg_Ksize - 3) {
		anchorX = value;
		avg_blur();
	}
}

void Blur::onTriggered_slider3_valueChange_avgBlur(int value) {
	if (-1 <= anchorX && anchorX < avg_Ksize-3 && -1 <= value && value < avg_Ksize - 3) {
		anchorY = value;
		avg_blur();
	}
}


//-----------高斯滤波--------------
void Blur::onTriggered_slider1_valueChange_gaussianBlur(int value) {
	if (-1 <= sigmaX && sigmaX < value - 3 && -1 <= sigmaY && sigmaY < value - 3) {
		gas_Ksize = value;
		Gaussian_blur();
	}
}

void Blur::onTriggered_slider2_valueChange_gaussianBlur(int value) {
	if (-1 <= value && value < gas_Ksize - 3 && -1 <= sigmaY && sigmaY < gas_Ksize - 3) {
		sigmaX = value;
		Gaussian_blur();
	}
}

void Blur::onTriggered_slider3_valueChange_gaussianBlur(int value) {
	if (-1 <= sigmaX && sigmaX < gas_Ksize - 3 && -1 <= value && value < gas_Ksize - 3) {
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