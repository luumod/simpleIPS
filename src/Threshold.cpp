#include "../include/Threshold.h"
#include "../include/Mat2QImage.h"

Threshold::Threshold() {}

Threshold::Threshold(const std::string& fileName)
	:Object(fileName) {}

Threshold::Threshold(const cv::Mat& mt)
	:Object(mt) {}

Threshold::~Threshold() {}

void Threshold::initialize()
{
	//阈值 最大值
	int threshold_value = 128, maxVal = 255;
}

void Threshold::restore()
{
	Object::restore();
	initialize();
}

QImage Threshold::threshold() {
	if (current_choice == -1) {
		return Mat2QImage(_mt);
	}

	cv::Mat tMt;

	cv::threshold(_mt, tMt, threshold_value, maxVal, current_choice);

	if (mode) {
		_mt = tMt;
	}
	_img = Mat2QImage(tMt);
	return _img;
}

void Threshold::onTriggered_slider1_valueChanged_thresholdValue(int value) {
	
	threshold_value = value;
	threshold();
}

void Threshold::onTriggered_slider2_valueChanged_maxValue(int value) {
	
	maxVal = value;
	threshold();
}