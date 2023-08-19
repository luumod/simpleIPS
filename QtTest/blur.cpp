#include "blur.h"
#include <QDebug>
#include "testWidget.h"
#include <QLabel>

blur::blur():Object() {
	qInfo() << "blur";


	get()->lab->setPixmap(QPixmap::fromImage(avg_blur()));
}

blur::~blur()
{
}

QImage blur::avg_blur()
{
	if (avg_Ksize <= 1) {
		return Mat2QImage(get()->mt);
	}

	cv::Mat _mt = get()->mt;
	cv::Mat tMt;
	//对预览图操作
	cv::blur(_mt, tMt, cv::Size(avg_Ksize, avg_Ksize), cv::Point(anchorX, anchorY));

	return Mat2QImage(tMt);
}

