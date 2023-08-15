#include "../include/Threshold.h"
#include "../include/Mat2QImage.h"

Threshold::Threshold(QWidget* parent)
	:QWidget(parent)
{
}

Threshold::Threshold(const std::string& fileName, QWidget* parent)
	:_mt(cv::imread(fileName)),_img(Mat2QImage(_mt)),QWidget(parent)
{
}

Threshold::Threshold(const cv::Mat& mt)
{
	mt.copyTo(_mt);
	_img = Mat2QImage(_mt);
}

Threshold::~Threshold()
{
}

QImage Threshold::threshold() {
	cv::Mat tMt{};
	_mt.copyTo(tMt);

	if (current_choice != -1) {
		cv::threshold(_mt, tMt, threshold_value, maxVal, current_choice);
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