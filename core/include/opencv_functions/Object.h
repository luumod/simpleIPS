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
	Object();
	virtual ~Object();
protected:
	virtual void initialize();
	static Widget* get();
	static void update(const cv::Mat& tMt);
	static void getMat(cv::Mat& res_mt);
public:
	int current_choice = 0;
};

#endif