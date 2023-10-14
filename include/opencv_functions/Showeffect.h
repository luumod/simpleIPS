#ifndef SHOWEFFECT_H_
#define SHOWEFFECT_H_

/// <summary>
/// 图像效果增强操作
/// author: Yuleo
/// </summary>
/// 

#include "Object.h"
#include <QWidget>
#include <opencv2/opencv.hpp>

class Showeffect :public Object {
public:
	Showeffect();
	~Showeffect();

	void initialize()override;

	void Bright();
	void Gamma();

	//对比度线性展宽
	cv::Mat showContrastLinearBroaden(cv::Mat mat = cv::Mat());
	void showBGRContrastLinearBroaden();
	void choice_linearAlgorithm();

	//灰级窗处理
	void showGrayWindow();

	cv::Mat showDynamicLinearAdj(cv::Mat mat = cv::Mat());
	void showBGRDynamicLinearAdj();
	void choice_DpLinearAlgorithm();

	cv::Mat showNoneDynamicLinearAdj(cv::Mat mat = cv::Mat());
	void showBGRNoneDynamicLinearAdj();
	void choice_NoneDpLinearAlgorithm();
public slots:
	void onTriggered_slider_valueChange_brighten(int); //亮度
	void onTriggered_slider_valueChange_gamma(double); //Gamma

	void onTriggered_slider_valueChange_linearg1(double); //g1
	void onTriggered_slider_valueChange_linearg2(double); //g2

	void onTriggered_slider_valueChange_GaryWindowF1(double); //f1
	void onTriggered_slider_valueChange_GaryWindowF2(double); //f2

	void onTriggered_slider_valueChange_DynamicA(double); //a
	void onTriggered_slider_valueChange_DynamicB(double); //b

	void onTriggered_slider_valueChange_NoneDynamicC(double);


	int bright_value = 0;
	double gamma_value = 1.0;

	//对比度线性展宽
	double linear_g1 = 0.0, linear_g2 = 0.0;//假定目标区域
	int linear_mode = 0; //灰度还是彩色

	//灰级窗
	double gray_f1 = 0.0, gray_f2 = 0.0;

	//线性动态范围调整
	double dp_a = 0.0, dp_b = 0.0;
	int DpLinear_mode = 0; //灰度还是彩色

	//非线性动态范围调整
	double n_dp_c = 1.0;
	int NoneDpLinear_mode = 0;  //灰度还是彩色
};


#endif //!SHOWEFFECT_H_