#ifndef BLUR_H_
#define BLUR_H_

#include <QWidget>
#include <opencv2/opencv.hpp>
class QImage;

class Blur :public QWidget {
	Q_OBJECT
public:
	explicit Blur(QWidget* parent = nullptr);
	explicit Blur(const std::string& fileName, QWidget* parent = nullptr);
	explicit Blur(const cv::Mat& mt, QWidget* parent = nullptr);
	~Blur();
	QImage avg_blur();//均值滤波
	QImage Gaussian_blur();//高斯滤波
	QImage median_blur();//中值滤波
	QImage bilateral_blur();//双边滤波
public slots:
	void onTriggered_slider1_valueChange_avgBlur(int);
	void onTriggered_slider2_valueChange_avgBlur(int );
	void onTriggered_slider3_valueChange_avgBlur(int );

	void onTriggered_slider1_valueChange_gaussianBlur(int);
	void onTriggered_slider2_valueChange_gaussianBlur(int);
	void onTriggered_slider3_valueChange_gaussianBlur(int);

	void onTriggered_slider_valueChange_medianBlur(int);

	void onTriggered_slider1_valueChange_bilateralBlur(int);
	void onTriggered_slider2_valueChange_bilateralBlur(int);
	void onTriggered_slider3_valueChange_bilateralBlur(int);
public:
	cv::Mat	_mt;
	QImage _img;
	bool type; //创作 or 测试

	//均值滤波
	int avg_Ksize = 5, anchorX = -1, anchorY = -1;
	//高斯滤波
	int gas_Ksize = 7, sigmaX = -1, sigmaY = -1;
	//中值滤波
	int median_Ksize = 5;
	//双边滤波
	int bin_d = 5, sigmaColor = 10, sigmaSpace = 10;
};


#endif