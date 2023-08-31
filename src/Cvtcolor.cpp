#include "../include/Cvtcolor.h"
#include "../include/Mat2QImage.h"
#include "../include/widget.h"

CvtColor::CvtColor() :Object() {}

CvtColor::~CvtColor() {}

void CvtColor::initialize()
{
}

void CvtColor::changeImg(int current_choice,TYPE dstType)
{
	cv::Mat _mt;
	if (get()->mode) {
		_mt = get()->savePoint_mt;
	}
	else {
		_mt = get()->mt;
	}
	cv::Mat tMt;
	cv::cvtColor(_mt, tMt, current_choice);
	
	Object::update(tMt);
}

void CvtColor::onTriggered_Comb1_currentTextChanged_cvtColorMode(const QString& name)
{
	int current_choice = 0;
	TYPE dstType{};
	//BGR转换为其他的形式
	if (name == "COLOR_BGR2GRAY") {
		current_choice = cv::ColorConversionCodes::COLOR_BGR2GRAY;
		dstType = TYPE::GRAY;
	}
	else if (name == "COLOR_BGR2HSV") {
		current_choice = cv::ColorConversionCodes::COLOR_BGR2HSV;
		dstType = TYPE::HSV;
	}
	else if (name == "COLOR_BGR2HLS") {
		current_choice = cv::ColorConversionCodes::COLOR_BGR2HLS;
		dstType = TYPE::HLS;
	}
	else if (name == "COLOR_BGR2Lab") {
		current_choice = cv::ColorConversionCodes::COLOR_BGR2Lab;
		dstType = TYPE::LAB;
	}
	changeImg(current_choice,dstType);
}
