﻿#include "../include/opencv_functions/Object.h"
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

	//根据curr_img显示为完美缩放比例
	get()->update_Image(get()->ori_scaledDelta);
}

void Object::getMat(cv::Mat& res_mt)
{
	if (get()->mode) {
		res_mt = get()->preview_mt;
	}
	else {
		res_mt = get()->inter_mt;
	}
}