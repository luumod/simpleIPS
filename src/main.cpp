#include "../include/widget.h"
#include <QApplication>
#include <QCoreApplication>
//#include <vld.h>

//添加控制台windows/console
#pragma comment(linker,"/subSystem:console /entry:mainCRTStartup")

int main(int argc,char* argv[])
{
	QApplication a(argc, argv);

	Widget w;
	w.show();

	return a.exec();
}