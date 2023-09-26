#include "../include/opencv_functions/Contours.h"
#include "../include/assist/Mat2QImage.h"
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

/*
params:
	tMt是灰度图
	tMt2是阈值化图
*/
void Contours::handle(cv::Mat& tMt, cv::Mat& tMt2, bool convexHull) {
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
	if (!convexHull) {
		//不是凸包，则直接阈值化
		cv::threshold(tMt, tMt2, 128, 255, cv::ThresholdTypes::THRESH_BINARY_INV);
	}
	else {
		//如果是绘制凸包，则首先模糊降噪一下
		cv::Mat tMMt;
		cv::blur(tMt, tMMt, cv::Size(7, 7));
		cv::threshold(tMMt, tMt2, 128, 255, cv::ThresholdTypes::THRESH_BINARY_INV);
		//cv::adaptiveThreshold(tMt,tMt2,255,cv::AdaptiveThresholdTypes::ADAPTIVE_THRESH_GAUSSIAN_C,cv::ThresholdTypes::THRESH_BINARY_INV,21,2);
	}
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

void Contours::convexHull()
{
	//1. 图像灰度处理
	//2. 图像阈值化处理
	cv::Mat _mt, tMt2;
	handle(_mt,tMt2,true);
	cv::imshow("test1", _mt);
	cv::imshow("tes2", tMt2);
	//3. 寻找轮廓
	cv::findContours(tMt2, points, hierarchy,
		cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//4. 寻找凸包轮廓
	if (get()->mode) {
		_mt = get()->savePoint_mt;
	}
	else {
		_mt = get()->ori_mt; //当前图片
	}
	cv::Mat tMt;
	_mt.copyTo(tMt);
	for (int i = 0; i < points.size(); i++) {
		std::vector<cv::Point> convex;
		cv::convexHull(points[i], convex);
		for (int j = 0; j < convex.size(); j++) {
			//cv::circle(tMt, convex[j], 1, cv::Scalar(0, 0, 255));
			cv::line(tMt, convex[j % (convex.size())], convex[(j + 1) % (convex.size())], cv::Scalar(255, 255, 0),3); //两点之间相连接
		}
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

void Contours::onClicked_btn_convexHull()
{
	convexHull();
}