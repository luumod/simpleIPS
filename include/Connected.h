#ifndef CONNECTED_H_
#define CONNECTED_H_

#include "Object.h"
#include <opencv2/opencv.hpp>
#include <QImage>

class Connected :public Object {
public:
	Connected();
	Connected(const std::string& fileName);
	Connected(const cv::Mat& mt);
	~Connected();
public:
	void initialize()override;
	void restore()override;
public slots:
	void onTriggered_Comb1_currentTextChanged_connectivtiy(const QString& value);
	void onTriggered_Comb2_currentTextChanged_ccltype(const QString& value);

	void handle(cv::Mat& tMt, cv::Mat& tMt2);
	void convert(const cv::Mat& src, cv::Mat& out, int nConAreas);
	QImage connectedComponents();
	QImage connectedComponentsWithStats();

	int current_choice = -1;
	int connectivity = 8, ccltype = cv::CCL_DEFAULT;
};



#endif