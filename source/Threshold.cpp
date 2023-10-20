#include "opencv_functions/Threshold.h"
#include "Widget/widget.h"

Threshold::Threshold() :Object() {}

Threshold::~Threshold() {}

void Threshold::initialize()
{
	//阈值 最大值
	int threshold_value = 128, maxVal = 255;
}

int Threshold::convertChoice()
{
	switch (current_choice)
	{
	case THRESHOLD::Binary:
		return cv::ThresholdTypes::THRESH_BINARY;
	case THRESHOLD::Binary_inv:
		return cv::ThresholdTypes::THRESH_BINARY_INV;
	case THRESHOLD::Trunc:
		return cv::ThresholdTypes::THRESH_TRUNC;
	case THRESHOLD::Tozero:
		return cv::ThresholdTypes::THRESH_TOZERO;
	case THRESHOLD::Tozero_inv:
		return cv::ThresholdTypes::THRESH_TOZERO_INV;
	default:
		return -1;
	}
	return -1;
}

void Threshold::threshold() {
	int types = -1;
	if ((types = convertChoice()) == -1) {
		return;
	}

	cv::Mat _mt;
	getMat(_mt);
	cv::Mat tMt;

	cv::threshold(_mt, tMt, threshold_value, maxVal, types);

	Object::update(tMt);
}

void Threshold::onTriggered_slider1_valueChanged_thresholdValue(int value) {
	
	threshold_value = value;
	threshold();
}

void Threshold::onTriggered_slider2_valueChanged_maxValue(int value) {
	
	maxVal = value;
	threshold();
}

void Threshold::onReturnPressed_Edit(QList<QString> strs)
{
	threshold_value = strs[0].toInt();
	maxVal = strs[1].toInt();
	threshold();
}
