#ifndef OBJECT_H_
#define OBJECT_H_

/// <summary>
/// 图像操作基类
/// author: Yuleo
/// </summary>
/// 

#include <opencv2/opencv.hpp>
#include "../assist/Enums.h"
class QImage;
class Widget;

class Object{
public:
	explicit Object();
	virtual ~Object();

	Widget* get();
	
public:
	//《可选择重写》根据当前current_choice来确定当前操作
	virtual int convertChoice();
	//《可选择重写》初始化参数值
	virtual void initialize();
	void update(const cv::Mat& tMt);
	void getMat(cv::Mat& res_mt);
public:
	int current_choice = 0;
};


#endif