﻿#ifndef THRESHOLD_H_
#define THRESHOLD_H_

#include <QWidget>
#include <opencv2/opencv.hpp>
class QImage;

class Threshold :public QWidget {
	Q_OBJECT
public:
	Threshold(QWidget* parent = nullptr);
	Threshold(const std::string& fileName, QWidget* parent = nullptr);
	Threshold(const cv::Mat& mt,QWidget* parent = nullptr);
	~Threshold();
public:
	void savePoint();
	void returnPoint();
	void update(const cv::Mat& newMt);
	void restore();

	QImage threshold();
public slots:
	void onTriggered_slider1_valueChanged_thresholdValue(int value);
	void onTriggered_slider2_valueChanged_maxValue(int value);
public:
	cv::Mat ori_mt;
	cv::Mat savePoint_mt;
	cv::Mat _mt;
	QImage _img;
	bool mode = false;

	//阈值 最大值
	int threshold_value = 128, maxVal = 255;

	int current_choice = -1;
};


#endif