#include <QApplication>
#include <QWidget>
#include <QSlider>
#include <iostream>
#include <vld.h>
class Widget :public QWidget {
public:
	Widget(QWidget* parent = nullptr)
		:QWidget(parent){
		s = new QSlider(this);
		connect(s, &QSlider::valueChanged, this, [=](int value) {
			qInfo() << "valueChanged: " << value;
			});
		connect(s, &QSlider::sliderMoved, this, [=](int value) {
			qInfo() << "sliderMoved: " << value;
			});


		s->setValue(10);
	}
private:
	QSlider* s = nullptr;
};


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	auto aa = new int[10];

	Widget w;
	w.show();

	return a.exec();
}