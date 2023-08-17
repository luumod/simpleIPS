#include "../include/Morphology.h"
#include "../include/Mat2QImage.h"

Morphology::Morphology() {}

Morphology::Morphology(const std::string& fileName)
	:Object(fileName) {}

Morphology::Morphology(const cv::Mat& mt)
	:Object(mt) {}

Morphology::~Morphology() {}

void Morphology::initialize()
{
	//核
	int Kernel = 5, anchorX = -1, anchorY = -1, iters = 1;
}

void Morphology::restore()
{
	Object::restore();
	initialize();
}

QImage Morphology::morphology()
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

void Morphology::onTriggered_slider1_valueChanged_kernel(int value) {
	if (-1 <= anchorX && anchorX < value && -1 <= anchorY && anchorY < value ){
		Kernel = value;
	//	qInfo() << value;
		morphology();
	}
}

void Morphology::onTriggered_slider2_valueChanged_anchorX(int value) {
	if (-1 <= value && value < Kernel && -1 <= anchorY && anchorY < Kernel ) {
		anchorX = value;
		morphology();
	}
}

void Morphology::onTriggered_slider3_valueChanged_anchorY(int value) {
	if (-1 <= anchorX && anchorX < Kernel && -1 <= value && value < Kernel ) {
		anchorY = value;
		morphology();
	}
}