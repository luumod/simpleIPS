#ifndef CONNECTED_H_
#define CONNECTED_H_

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QWidget>

class Connected :public QWidget {
public:
	Connected(QWidget* parent= nullptr);
	Connected(const cv::Mat& mt,QWidget* parent= nullptr);
	~Connected();
public:
	void update(const cv::Mat& mt);
	void restore();
	void handle(cv::Mat& tMt, cv::Mat& tMt2);
	void convert(const cv::Mat& src, cv::Mat& out, int nConAreas);
	QImage connectedComponents();
	QImage connectedComponentsWithStats();

	void onTriggered_Comb1_currentTextChanged_connectivtiy(const QString& value);
	void onTriggered_Comb2_currentTextChanged_ccltype(const QString& value);

	cv::Mat ori_mt{};
	cv::Mat _mt{};
	QImage _img{};

	int current_choice = -1;

	int connectivity = 8, ccltype = cv::CCL_DEFAULT;
};



#endif