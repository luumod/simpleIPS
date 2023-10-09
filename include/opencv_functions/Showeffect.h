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
public slots:
	void onTriggered_slider_valueChange_brighten(int); //亮度
	void onTriggered_slider_valueChange_gamma(double); //Gamma
private:
	int bright_value = 0;
	double gamma_value = 1.0;
};


#endif //!SHOWEFFECT_H_