#include "../include/Threshold.h"
#include "../include/Mat2QImage.h"

Threshold::Threshold(QWidget* parent)
	:QWidget(parent)
{
}

Threshold::Threshold(const std::string& fileName, QWidget* parent)
	:ori_mt(cv::imread(fileName)),_img(Mat2QImage(ori_mt)),QWidget(parent)
{
	ori_mt.copyTo(_mt);
}

Threshold::Threshold(const cv::Mat& mt,QWidget* parent)
	:QWidget(parent)
{
	mt.copyTo(ori_mt);
	ori_mt.copyTo(_mt);
	_img = Mat2QImage(_mt);
}

Threshold::~Threshold()
{
}

void Threshold::savePoint()
{
	//QUndoStack
	_mt.copyTo(savePoint_mt);
}

void Threshold::returnPoint()
{
	savePoint_mt.copyTo(_mt);
}

void Threshold::update(const cv::Mat& newMt)
{
	newMt.copyTo(ori_mt);
	restore();
}


void Threshold::restore()
{
	ori_mt.copyTo(_mt);
	_img = Mat2QImage(_mt);

	//阈值 最大值
	threshold_value = 128, maxVal = 255;
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