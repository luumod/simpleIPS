#include <QApplication>
#include "testWidget.h"
#include <vld.h>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Widget* w = Widget::getInstance();
	w->show();
	w->create();

	QObject::connect(&a, &QApplication::aboutToQuit, [=]() {
		delete w;
		});

	return a.exec();
}