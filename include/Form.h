#ifndef FORM_H_
#define FORM_H_

#include <QWidget>
#include <opencv2/opencv.hpp>
class QImage;

class Form :public QWidget {
	Q_OBJECT
public:
	Form(QWidget* parent = nullptr);
	Form(const std::string& fileName, QWidget* parent = nullptr);
	Form(const cv::Mat& mt, QWidget* parent = nullptr);
	~Form();
public:
	void savePoint();
	void returnPoint();
	void update(const cv::Mat& newMt);
	void restore();
	
	QImage morphology();
public slots:
	void onTriggered_slider1_valueChanged_kernel(int value);
	void onTriggered_slider2_valueChanged_anchorX(int value);
	void onTriggered_slider3_valueChanged_anchorY(int value); 
public:
	cv::Mat ori_mt;
	cv::Mat savePoint_mt;
	cv::Mat _mt;
	QImage _img;
	bool mode = false;
	//核
	int Kernel = 5, anchorX = -1, anchorY = -1, iters = 1;

	int current_choice = -1;

};


#endif //FORM_H_