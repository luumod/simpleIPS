#include "Include/opencv_functions/Object.h"
#include "Include/Widget/widget.h"
#include "Include/assist/Mat2QImage.h"
#include "Include/Widget/LookWidget.h"
#include "Include/assist/Common.h"
#include "Include/assist/Res.h"
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

void Object::initialize()
{
	current_choice = 0;
	return;
}

void Object::update(const cv::Mat& tMt)
{
	//修改当前Mat与其对应的Image
	get()->res->curr_mt = tMt;
	get()->res->curr_img = Mat2QImage(tMt);

	//根据curr_img显示为完美缩放比例
	get()->update_Image(get()->ori_scaledDelta);
}

void Object::getMat(cv::Mat& res_mt)
{
	if (get()->mode) {
		res_mt = get()->res->flash_mt;
	}
	else {
        res_mt = get()->res->inter_mt;
    }
}

cv::Mat Object::getMat2()
{
    if (get()->mode) {
        return get()->res->flash_mt;
    }
    else {
        return get()->res->inter_mt;
    }
    return cv::Mat();
}
