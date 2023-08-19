#include "testWidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include "blur.h"


Widget* Widget::wid = nullptr;

Widget::Widget(QMainWindow* parent)
	:QMainWindow(parent){
	this->resize(640, 480);

	lab = new QLabel;
	lab->show();

	mt = cv::imread("../resource/dog.png");
}

Widget::~Widget() {
	qInfo() << "~Widget";
	delete obj;
	delete lab;
}

void Widget::create()
{
	obj = new blur();
}

Widget* Widget::getInstance()
{
	if (wid == nullptr) {
		wid = new Widget;
	}
	return wid;
}
