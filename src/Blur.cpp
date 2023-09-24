#include "../include/Blur.h"
#include "../include/widget.h"
#include <QDebug>
#include <QLabel>

Blur::Blur() :Object() {}

Blur::~Blur() {}

void Blur::initialize()
{
	//均值滤波
	avg_Ksize = 1, anchorX = -1, anchorY = -1;
	//高斯滤波
	gas_Ksize = 7, sigmaX = -1, sigmaY = -1;
	//中值滤波
	median_Ksize = 5;
	//双边滤波
	bin_d = 5, sigmaColor = 10, sigmaSpace = 10;
}

void Blur::avg_blur()
{
	if (avg_Ksize <= 1) {
		return; //当前图片不变
	}

	cv::Mat _mt;
	if (get()->mode) {
		_mt = get()->savePoint_mt;
	}
	else {
		_mt = get()->ori_mt; //当前图片
	}
	cv::Mat tMt;

	cv::blur(_mt, tMt, cv::Size(avg_Ksize, avg_Ksize), cv::Point(anchorX, anchorY));
	
	Object::update(tMt);
}

void Blur::Gaussian_blur()
{
	if (gas_Ksize <= 1) {
		return ;
	}

	cv::Mat _mt;
	if (get()->mode) {
		_mt = get()->savePoint_mt;
	}
	else {
		_mt = get()->ori_mt; //当前图片
	}
	cv::Mat tMt;

	cv::GaussianBlur(_mt, tMt, cv::Size(gas_Ksize, gas_Ksize), sigmaX, sigmaY);
	
	Object::update(tMt);
}

void Blur::median_blur()
{
	cv::Mat _mt;
	if (get()->mode) {
		_mt = get()->savePoint_mt;
	}
	else {
		_mt = get()->ori_mt; //当前图片
	}
	cv::Mat tMt{};

	cv::medianBlur(_mt, tMt, median_Ksize);

	Object::update(tMt);
}

void Blur::bilateral_blur()
{
	cv::Mat _mt;
	if (get()->mode) {
		_mt = get()->savePoint_mt;
	}
	else {
		_mt = get()->ori_mt; //当前图片
	}

	if (_mt.type() != CV_8UC1 && _mt.type() != CV_8UC3) {
		return;
	}
	cv::Mat tMt;

	cv::bilateralFilter(_mt, tMt, bin_d, sigmaColor, sigmaSpace);

	Object::update(tMt);
}

//-----------均值滤波--------------
void Blur::onTriggered_slider1_valueChange_avgBlur(int value) {
	if (-1 <= anchorX && anchorX < value && -1 <= anchorY && anchorY < value) {
		avg_Ksize = value;
		avg_blur(); //type = 1：制作模式  type = 0 单图操作模式
	}
}

void Blur::onTriggered_slider2_valueChange_avgBlur(int value) {
	if (-1 <= value && value < avg_Ksize && -1 <= anchorY && anchorY < avg_Ksize) {
		anchorX = value;
		avg_blur();
	}
}

void Blur::onTriggered_slider3_valueChange_avgBlur(int value) {
	if (-1 <= anchorX && anchorX < avg_Ksize && -1 <= value && value < avg_Ksize ) {
		anchorY = value;
		avg_blur();
	}
}


//-----------高斯滤波--------------
void Blur::onTriggered_slider1_valueChange_gaussianBlur(int value) {
	if (-1 <= sigmaX && sigmaX < value  && -1 <= sigmaY && sigmaY < value ) {
		gas_Ksize = value;
		Gaussian_blur();
	}
}

void Blur::onTriggered_slider2_valueChange_gaussianBlur(int value) {
	if (-1 <= value && value < gas_Ksize  && -1 <= sigmaY && sigmaY < gas_Ksize ) {
		sigmaX = value;
		Gaussian_blur();
	}
}

void Blur::onTriggered_slider3_valueChange_gaussianBlur(int value) {
	if (-1 <= sigmaX && sigmaX < gas_Ksize  && -1 <= value && value < gas_Ksize ) {
		sigmaY = value;
		Gaussian_blur();
	}
}

void Blur::onTriggered_slider_valueChange_medianBlur(int value)
{
	median_Ksize = value;
	median_blur();
}

void Blur::onTriggered_slider1_valueChange_bilateralBlur(int value)
{
	bin_d = value;
	bilateral_blur();
}

void Blur::onTriggered_slider2_valueChange_bilateralBlur(int value)
{
	sigmaColor = value;
	bilateral_blur();
}

void Blur::onTriggered_slider3_valueChange_bilateralBlur(int value)
{
	sigmaSpace = value;
	bilateral_blur();
}


void Blur::onReturnPressed_Edit(QList<QString> strs) {
	if (get()->now_operation == BLUR::Average) {
		onReturnPressed_AvgBlur_Edit(strs);
	}
	else if (get()->now_operation == BLUR::Gaussian) {
		onReturnPressed_Gaussian_Edit(strs);
	}
	else if (get()->now_operation == BLUR::Median) {
		onReturnPressed_Median_Edit(strs);
	}
	else if (get()->now_operation == BLUR::Bilateral) {
		onReturnPressed_Bilateral_Edit(strs);
	}
}

//自己输入
void Blur::onReturnPressed_AvgBlur_Edit(QList<QString> strs)
{
	this->avg_Ksize = strs[0].toInt();
	this->anchorX = strs[1].toInt();
	this->anchorY = strs[2].toInt();
	avg_blur();
}


void Blur::onReturnPressed_Gaussian_Edit(QList<QString> strs) {
	int n1 = strs[0].toInt();
	n1 = (n1 % 2 == 0) ? n1 - 1 : n1;
	this->gas_Ksize = n1;
	this->sigmaX = strs[1].toInt();
	this->sigmaY = strs[2].toInt();
	Gaussian_blur();
}

void Blur::onReturnPressed_Median_Edit(QList<QString> strs)
{
	int n1 = strs[0].toInt();
	n1 = (n1 % 2 == 0) ? n1 - 1 : n1;
	this->median_Ksize = n1;
	median_blur();
}

void Blur::onReturnPressed_Bilateral_Edit(QList<QString> strs)
{
	this->bin_d = strs[0].toInt();
	this->sigmaColor = strs[1].toInt();
	this->sigmaSpace = strs[2].toInt();
	bilateral_blur();
}