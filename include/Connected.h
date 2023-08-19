#ifndef CONNECTED_H_
#define CONNECTED_H_

#include "Object.h"
#include <QWidget>
#include <opencv2/opencv.hpp>

class Connected :public Object {
public:
	Connected();
	~Connected();
protected:
	void initialize()override;
public slots:
	void onTriggered_Comb1_currentTextChanged_connectivtiy(int index);
	void onTriggered_Comb2_currentTextChanged_ccltype(int index);

	void handle(cv::Mat& tMt, cv::Mat& tMt2);
	void convert(const cv::Mat& src, cv::Mat& out, int nConAreas);
	void connectedComponents();
	void connectedComponentsWithStats();

	int current_choice = -1;
	int connectivity = 8, ccltype = cv::CCL_DEFAULT;
};

#endif