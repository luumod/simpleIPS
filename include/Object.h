#ifndef OBJECT_H_
#define OBJECT_H_


#include <opencv2/opencv.hpp>
#include "Enums.h"
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