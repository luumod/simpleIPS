#ifndef TESTWIDGET_H_
#define TESTWIDGET_H_
#include <QMainWindow>
#include <opencv2/opencv.hpp>
class QLabel;
class blur;

class Widget :public QMainWindow {
public:
	//friend class Object;
	static Widget* getInstance();
	~Widget();
	void create();
	cv::Mat mt;
	QLabel* lab = nullptr;
private:
	Widget(QMainWindow* parent = nullptr);
	static Widget* wid;
	blur* obj;

};

#endif