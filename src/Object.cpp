#include "../include/opencv_functions/Object.h"
#include "../include/widget.h"
#include "../include/assist/Mat2QImage.h"
#include "../include/other_functions/LabelImg.h"
#include <QDebug>

Object::Object()
{
}

Object::~Object()
{
}

Widget* Object::get() {
	return Widget::getInstance();
}

void Object::update(const cv::Mat& tMt)
{
	get()->mt = tMt;
	get()->img = Mat2QImage(tMt);

	if (get()->mode) {
		//混合加工模式，修改当前预览图片 sub_lab_img
		get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->img.scaled(200, 200)));
	}
	else {
		//不是混合加工，直接修改当前lab_img图片
		get()->lab_img->setPixmap(QPixmap::fromImage(get()->img)); //直接修改当前图片
	}
}