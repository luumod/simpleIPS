#include "../include/opencv_functions/Object.h"
#include "../include/widget.h"
#include "../include/assist/Mat2QImage.h"
#include "../include/other_functions/LabelImg.h"
#include <QDebug>
#include "../include/Common.h"

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
	//修改当前Mat与其对应的Image
	get()->curr_mt = tMt;
	get()->curr_img = Mat2QImage(tMt);

	if (get()->mode) {
		//混合加工模式，首先修改当前预览图片，然后由用户决定是否将其预览作为主图片
		get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->curr_img.scaled(SUB_LAB_IMG_WIDTH, SUB_LAB_IMG_HEIGHT)));
	}
	else {
		//不是混合加工，直接修改当前lab_img图片
		get()->lab_img->setPixmap(QPixmap::fromImage(get()->curr_img));
	}
}