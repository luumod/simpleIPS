#ifndef BLUR_H_
#define BLUR_H_

/// <summary>
/// 图像轮廓模糊操作
/// author: Yuleo
/// </summary>
/// 

#include "Object.h"
#include <QWidget>
#include <opencv2/opencv.hpp>
class QImage;
class Blur :public Object {
public:
	Blur();
	~Blur();

	int convertChoice()override;
	void initialize()override;
	void avg_blur();//均值滤波
	void Gaussian_blur();//高斯滤波
	void median_blur();//中值滤波
	void bilateral_blur();//双边滤波
public slots:
	void onTriggered_slider1_valueChange_avgBlur(int);
	void onTriggered_slider2_valueChange_avgBlur(int);
	void onTriggered_slider3_valueChange_avgBlur(int);

	void onTriggered_slider1_valueChange_gaussianBlur(int);
	void onTriggered_slider2_valueChange_gaussianBlur(int);
	void onTriggered_slider3_valueChange_gaussianBlur(int);

	void onTriggered_slider_valueChange_medianBlur(int);

	void onTriggered_slider1_valueChange_bilateralBlur(int);
	void onTriggered_slider2_valueChange_bilateralBlur(int);
	void onTriggered_slider3_valueChange_bilateralBlur(int);

	void onReturnPressed_AvgBlur_Edit(QList<QString> strs);
	void onReturnPressed_Gaussian_Edit(QList<QString> strs);
	void onReturnPressed_Median_Edit(QList<QString> strs);
	void onReturnPressed_Bilateral_Edit(QList<QString> strs);

	void onReturnPressed_Edit(QList<QString> strs);
public:

	//均值滤波
	int avg_Ksize = 1, anchorX = -1, anchorY = -1;
	//高斯滤波
	int gas_Ksize = 1, sigmaX = -1, sigmaY = -1;
	//中值滤波
	int median_Ksize = 5;
	//双边滤波
	int bin_d = 5, sigmaColor = 10, sigmaSpace = 10;
};


#endif