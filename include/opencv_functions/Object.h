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
	
public:;
	virtual void initialize() = 0;
	void update(const cv::Mat& tMt);

};


#endif