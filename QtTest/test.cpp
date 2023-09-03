#include <QApplication>
#include <opencv2/opencv.hpp>
#include "DrawWidget.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	
	DrawWidget w;
	w.show();

	return a.exec();
}