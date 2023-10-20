#ifndef FORM_H_
#define FORM_H_

/// <summary>
/// 图像形态学操作
/// author: Yuleo
/// </summary>
/// 

#include "Object.h"
#include <QWidget>
#include <opencv2/opencv.hpp>
class QImage;

class Morphology :public Object {
public:
	Morphology();
	~Morphology();

	int convertChoice();
	void morphology();
protected:
	void initialize()override;
public slots:
	void onTriggered_slider1_valueChanged_kernel(int value);
	void onTriggered_slider2_valueChanged_anchorX(int value);
	void onTriggered_slider3_valueChanged_anchorY(int value); 
	void onReturnPressed_Edit(QList<QString> strs);
public:
	//核
	int Kernel = 5, anchorX = -1, anchorY = -1, iters = 1;

};


#endif //FORM_H_