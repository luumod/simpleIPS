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
	Form(const cv::Mat& mt);
	~Form();
	
	QImage morphology();
public slots:
	void onTriggered_slider1_valueChanged_kernel(int value);
	void onTriggered_slider2_valueChanged_anchorX(int value);
	void onTriggered_slider3_valueChanged_anchorY(int value); 
	//void onTriggered_slider3_valueChanged_anthorY(int value);
public:
	cv::Mat _mt;
	QImage _img;

	//核
	int Kernel = 3, anchorX = -1, anchorY = -1, iters = 1;

	int current_choice = -1;

};


#endif //FORM_H_