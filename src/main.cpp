#include "../include/widget.h"
#include <QApplication>
//#include <QCoreApplication>
#include <vld.h>

class Widget;

//添加控制台windows/console
#pragma comment(linker,"/subSystem:console /entry:mainCRTStartup")

int main(int argc,char* argv[])
{
	QApplication a(argc, argv);

	Widget* w = Widget::getInstance();
	w->show();
	w->initFunction();

	//释放单例对象
	QObject::connect(&a, &QApplication::aboutToQuit, [=]() {
		delete w;
	});
	return a.exec();
}