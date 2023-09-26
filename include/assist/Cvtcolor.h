#ifndef CVTCOLOR_H_
#define CVTCOLOR_H_

#include "../opencv_functions/Object.h"
#include <QWidget>
#include <opencv2/opencv.hpp>

//图像转换操作
class CvtColor :public Object {
public:
	CvtColor();
	~CvtColor();

	void initialize()override;
	void changeImg(int, TYPE);
public slots:
	void onTriggered_Comb1_currentTextChanged_cvtColorMode(const QString&);
private:
};

#endif // !CVTCOLOR_H_
