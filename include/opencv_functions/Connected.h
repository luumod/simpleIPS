#ifndef CONNECTED_H_
#define CONNECTED_H_

/// <summary>
/// 图像连通性分析操作
/// author: Yuleo
/// </summary>
/// 

#include "Object.h"
#include <QWidget>
#include <opencv2/opencv.hpp>

/*
图像连通操作
*/
class Connected :public Object {
public:
	Connected();
	~Connected();

	int convertChoice()override;
	void handle(cv::Mat& tMt, cv::Mat& tMt2);
	void convert(const cv::Mat& src, cv::Mat& out, int nConAreas);
	void connectedComponents();
	void connectedComponentsWithStats();
protected:
	void initialize()override;
public slots:
	void onTriggered_Comb1_currentTextChanged_connectivtiy(int index);
	void onTriggered_Comb2_currentTextChanged_ccltype(int index);

	

	int connectivity = 8, ccltype = cv::CCL_DEFAULT;
};

#endif