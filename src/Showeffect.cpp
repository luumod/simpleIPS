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
	bright_value = 0;
}

void Showeffect::Bright()
{
	cv::Mat _mt;
	if (get()->mode) {
		_mt = get()->preview_mt;
	}
	else {
		_mt = get()->inter_mt;
	}
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


//void Showeffect::mask()
//{
//	//掩膜操作
//	cv::Mat _mt;
//	if (get()->mode) {
//		_mt = get()->preview_mt;
//	}
//	else {
//		_mt = get()->inter_mt;
//	}
//	cv::Mat tMt;
//	cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
//		0, -1, 0,
//		-1, 5, -1,
//		0, -1, 0
//		);
//	cv::filter2D(_mt, tMt, _mt.depth(), kernel);
//
//	Object::update(tMt);
//}

void Showeffect::onTriggered_slider_valueChange_brighten(int bright_value)
{
	this->bright_value = bright_value;
	Bright();
}

//void Showeffect::onTriggered_slider_valueChange_mask()
//{
//	mask();
//}