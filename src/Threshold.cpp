#include "../include/opencv_functions/Threshold.h"
#include "../include/widget.h"

Threshold::Threshold() :Object() {}

Threshold::~Threshold() {}

void Threshold::initialize()
{
	//阈值 最大值
	int threshold_value = 128, maxVal = 255;
}

void Threshold::threshold() {
	if (current_choice == -1) {
		return;
	}

	cv::Mat _mt;
	getMat(_mt);
	cv::Mat tMt;

	cv::threshold(_mt, tMt, threshold_value, maxVal, current_choice);

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