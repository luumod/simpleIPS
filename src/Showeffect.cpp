#include "../include/opencv_functions/Showeffect.h"
#include "../include/widget.h"
#include <QDebug>

Showeffect::Showeffect()
	:Object()
{
}
Showeffect::~Showeffect() {}

void Showeffect::initialize()
{
	gamma_value = 0.0;
	bright_value = 0;
}

void Showeffect::Bright()
{
	cv::Mat _mt;
	getMat(_mt);
	cv::Mat tMt = cv::Mat::zeros(_mt.size(), _mt.type());

	//调整亮度
	for (int i = 0; i < _mt.rows; i++) {
		for (int j = 0; j < _mt.cols; j++) {
			for (int z = 0; z < _mt.channels(); z++) {
				tMt.at<cv::Vec3b>(i, j)[z] = cv::saturate_cast<uchar>(_mt.at<cv::Vec3b>(i, j)[z] + this->bright_value);
			}
		}
	}

	Object::update(tMt);
}

void Showeffect::Gamma()
{
	cv::Mat _mt;
	getMat(_mt);
	//标准化为 0-255之间
	cv::Mat normalizedImage;
	cv::normalize(_mt, normalizedImage, 0, 255, cv::NormTypes::NORM_MINMAX);

	//转换为浮点型
	cv::Mat tMt;
	normalizedImage.convertTo(tMt, CV_32FC3);

	//γ矫正
	cv::Mat GammaMat;
	cv::pow(tMt / 255.0, gamma_value, GammaMat);
	//CV_32FC3类型需要进一步转换为CV_8UC3
	Object::update(GammaMat);
}

void Showeffect::onTriggered_slider_valueChange_brighten(int bright_value)
{
	this->bright_value = bright_value;
	Bright();
}

void Showeffect::onTriggered_slider_valueChange_gamma(double gamma_value)
{
	this->gamma_value = gamma_value;
	Gamma();
}