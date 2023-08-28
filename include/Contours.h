#ifndef CONTOURS_H_
#define CONTOURS_H_

#include "Object.h"
#include <QWidget>
#include <opencv2/opencv.hpp>
class QColor;
//图像轮廓操作
class Contours :public Object {
public:
	Contours();
	~Contours();

	void initialize()override;
	void handle(cv::Mat& tMt, cv::Mat& tMt2);
	void findContours();
	void drawContours();
public slots:
	void onTriggered_Comb1_currentTextChanged_contoursMode(int index);
	void onTriggered_Comb2_currentTextChanged_contoursMethod(int index);
	void onTriggered_Comb3_currentTextChanged_contoursThick(int index);
	void onTriggered_Color_currentTextChanged_contoursColor(QColor color);
private:
	std::vector<std::vector<cv::Point>> points;
	std::vector<cv::Vec4i> hierarchy;
	int mode = cv::RETR_CCOMP;
	int method = cv::CHAIN_APPROX_NONE;
	int thick = 1;
	QColor color = Qt::red;
};

#endif // !CONTOURS_H_
