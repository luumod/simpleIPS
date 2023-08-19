#ifndef TESTOBJECT_H_
#define TESTOBJECT_H_

#include <opencv2/opencv.hpp>
class QImage;
class Widget;

class Object {
public:
	Object();
	Widget* get();
	virtual ~Object();

private:
};


#endif