#ifndef BLUR_H_
#define BLUR_H_

#include "testObject.h"
#include <opencv2/opencv.hpp>
#include "../include/Mat2QImage.h"
class QImage;
class Widget;

class blur :public Object {
public:
	blur();
	~blur();

	QImage avg_blur();

public:
	//均值滤波
	int avg_Ksize = 5, anchorX = -1, anchorY = -1;
	//高斯滤波
	int gas_Ksize = 1, sigmaX = -1, sigmaY = -1;
	//中值滤波
	int median_Ksize = 5;
	//双边滤波
	int bin_d = 5, sigmaColor = 10, sigmaSpace = 10;
};


#endif