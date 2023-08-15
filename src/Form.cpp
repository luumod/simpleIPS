#include "../include/Form.h"
#include "../include/Mat2QImage.h"

Form::Form(QWidget* parent)
	:QWidget(parent)
{
}

Form::Form(const std::string& fileName, QWidget* parent)
	:_mt(cv::imread(fileName)), _img(Mat2QImage(_mt)), QWidget(parent)
{
}

Form::Form(const cv::Mat& mt)
{
	mt.copyTo(_mt);
	_img = Mat2QImage(_mt);
}

Form::~Form()
{
}

QImage Form::morphology()
{
	cv::Mat tMt{};
	_mt.copyTo(tMt);

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(Kernel, Kernel));
	if (current_choice != -1) {
		cv::morphologyEx(_mt, tMt, current_choice, kernel,cv::Point(anchorX, anchorY),iters);
	}

	_img = Mat2QImage(tMt);

	return _img;
}

void Form::onTriggered_slider1_valueChanged_kernel(int value) {
	if (-1 <= anchorX && anchorX < value - 3 && -1 <= anchorY && anchorY < value - 3){
		Kernel = value;
		morphology();
	}
}

void Form::onTriggered_slider2_valueChanged_anchorX(int value) {
	if (-1 <= value && value < Kernel - 3 && -1 <= anchorY && anchorY < Kernel - 3) {
		anchorX = value;
		morphology();
	}
}

void Form::onTriggered_slider3_valueChanged_anchorY(int value) {
	if (-1 <= anchorX && anchorX < Kernel - 3 && -1 <= value && value < Kernel - 3) {
		anchorY = value;
		morphology();
	}
}