#include "../include/Contours.h"
#include "../include/Mat2QImage.h"
#include "../include/widget.h"
#include <QColor>
#include <QDebug>
Contours::Contours() :Object() {}

Contours::~Contours() {}

void Contours::initialize()
{
	points.clear();
	hierarchy.clear();
	mode = cv::RETR_CCOMP;
	method = cv::CHAIN_APPROX_NONE;
	thick = 1;
	color = Qt::red;
}

void Contours::handle(cv::Mat& tMt, cv::Mat& tMt2) {
	/*
	tMt2为转换后的阈值图
	*/
	cv::Mat _mt;
	if (get()->mode) {
		_mt = get()->savePoint_mt;
	}
	else {
		_mt = get()->ori_mt;
	}
	//转换为灰度图
	cv::cvtColor(_mt, tMt, cv::COLOR_BGR2GRAY);
	//阈值化
	cv::threshold(tMt, tMt2, 128, 255, cv::ThresholdTypes::THRESH_BINARY_INV);
}

void Contours::findContours()
{
	cv::Mat _mt,tMt;
	handle(_mt,tMt);
	//tMt是阈值化后的图
	cv::findContours(tMt, points, hierarchy, mode, method);
}

void Contours::drawContours()
{
	findContours(); //获取points轮廓信息
	cv::Mat tMt = cv::Mat::zeros(get()->ori_mt.size(), CV_8UC3);
	
	for (int i = 0; i < points.size();i++) {
		cv::Scalar scolor = cv::Scalar(color.blue(), color.green(), color.red());
		cv::drawContours(tMt, points, i, scolor, thick);
	}
	Object::update(tMt);
}

void Contours::onTriggered_Comb1_currentTextChanged_contoursMode(int index)
{
	mode = cv::RetrievalModes(index);
	drawContours();
}

void Contours::onTriggered_Comb2_currentTextChanged_contoursMethod(int index)
{
	method = cv::ContourApproximationModes(index + 1);
	drawContours();
}

void Contours::onTriggered_Comb3_currentTextChanged_contoursThick(int index) {
	thick = index + 1;
	drawContours();
}

void Contours::onTriggered_Color_currentTextChanged_contoursColor(QColor color)
{
	this->color = color;
	drawContours();
}