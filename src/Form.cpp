#include "../include/Form.h"
#include "../include/Mat2QImage.h"

Form::Form(QWidget* parent)
	:ori_mt(),_mt(),QWidget(parent)
{
}

Form::Form(const std::string& fileName, QWidget* parent)
	:ori_mt(cv::imread(fileName)), _img(Mat2QImage(ori_mt)), QWidget(parent)
{
	ori_mt.copyTo(_mt);
}

Form::Form(const cv::Mat& mt,QWidget* parent)
	:QWidget(parent)
{
	mt.copyTo(ori_mt);
	ori_mt.copyTo(_mt);
	_img = Mat2QImage(_mt);
}

Form::~Form()
{
}

void Form::savePoint()
{
	//QUndoStack
	_mt.copyTo(savePoint_mt);
}

void Form::returnPoint()
{
	savePoint_mt.copyTo(_mt);
}

void Form::update(const cv::Mat& newMt)
{
	newMt.copyTo(ori_mt);
	restore();
}


void Form::restore()
{
	ori_mt.copyTo(_mt);
	_img = Mat2QImage(_mt);

	//核 锚点 
	int Kernel = 1, anchorX = -1, anchorY = -1, iters = 1;
}

QImage Form::morphology()
{
	if (current_choice == -1) {
		return Mat2QImage(_mt);
	}
	cv::Mat tMt;

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(Kernel, Kernel));
	cv::morphologyEx(_mt, tMt, current_choice, kernel,
			cv::Point(anchorX, anchorY),iters);
	if (mode) {
		_mt = tMt;
	}
	_img = Mat2QImage(tMt);
	return _img;
}

void Form::onTriggered_slider1_valueChanged_kernel(int value) {
	if (-1 <= anchorX && anchorX < value && -1 <= anchorY && anchorY < value ){
		Kernel = value;
		qInfo() << value;
		morphology();
	}
}

void Form::onTriggered_slider2_valueChanged_anchorX(int value) {
	if (-1 <= value && value < Kernel && -1 <= anchorY && anchorY < Kernel ) {
		anchorX = value;
		morphology();
	}
}

void Form::onTriggered_slider3_valueChanged_anchorY(int value) {
	if (-1 <= anchorX && anchorX < Kernel && -1 <= value && value < Kernel ) {
		anchorY = value;
		morphology();
	}
}