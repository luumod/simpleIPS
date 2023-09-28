#include "../include/opencv_functions/Morphology.h"
#include "../include/widget.h"

Morphology::Morphology() :Object() {}

Morphology::~Morphology() {}

void Morphology::initialize()
{
	//核
	int Kernel = 5, anchorX = -1, anchorY = -1, iters = 1;
}


void Morphology::morphology()
{
	if (current_choice == -1) {
		return;
	}

	cv::Mat _mt;
	if (get()->mode) {
		_mt = get()->preview_mt;
	}
	else {
		_mt = get()->inter_mt; //当前图片
	}
	cv::Mat tMt;

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(Kernel, Kernel));
	cv::morphologyEx(_mt, tMt, current_choice, kernel,
			cv::Point(anchorX, anchorY),iters);
	
	Object::update(tMt);
}

void Morphology::onTriggered_slider1_valueChanged_kernel(int value) {
	if (-1 <= anchorX && anchorX < value && -1 <= anchorY && anchorY < value ){
		Kernel = value;
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

void Morphology::onReturnPressed_Edit(QList<QString> strs)
{
	this->Kernel = strs[0].toInt();
	this->anchorX = strs[1].toInt();
	this->anchorY = strs[2].toInt();
	this->iters = strs[3].toInt();
	morphology();
}