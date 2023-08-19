#ifndef OBJECT_H_
#define OBJECT_H_


#include <opencv2/opencv.hpp>
class QImage;
class Widget;

class Object{
public:
	explicit Object();
	virtual ~Object();

	Widget* get();
	
public:
	//virtual void restore() = 0;
	virtual void initialize() = 0;
	void update(const cv::Mat& tMt);

};


#endif