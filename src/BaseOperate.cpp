#include "../include/widget.h"
#include "../include/opencv_functions/BaseOperate.h"
#include "../include/assist/Mat2QImage.h"
#include "../include/other_functions/LabelImg.h"
#include <QLabel>

BaseOperate::BaseOperate()
	:Object()
{
}

BaseOperate::~BaseOperate()
{
}

void BaseOperate::initialize()
{
}

void BaseOperate::cvtColor(QAction* action) {

	cv::Mat _mt;
	_mt = get()->root_mt; //每次都对原始根图片进行操作，因为重复的cvtColor操作会导致图片显示出现问题！！！
	cv::Mat ori_mt_res;
	if (action == get()->action_hsv) {
		cv::cvtColor(_mt, ori_mt_res, cv::COLOR_BGR2HSV);
	}
	else if (action == get()->action_hls) {
		cv::cvtColor(_mt, ori_mt_res, cv::COLOR_BGR2HLS);
	}
	else if (action == get()->action_lab) {
		cv::cvtColor(_mt, ori_mt_res, cv::COLOR_BGR2Lab);
	}
	else if (action == get()->action_rgb) {
		cv::cvtColor(_mt, ori_mt_res, cv::COLOR_BGR2RGB);
	}
	else {
		//原图
		ori_mt_res = _mt;
	}
	//对原图片进行修改
	get()->ori_mt = ori_mt_res;
	get()->ClearImageToOrigin();
	//对子图片进行及时修改，防止出现更新不及时
	get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->img.scaled(200, 200)));
}

void BaseOperate::onTriggered_picture_rotate90() {
	cv::Mat _mt;
	_mt = get()->mt;
	cv::Mat tMt;
	cv::rotate(_mt, tMt, cv::RotateFlags::ROTATE_90_CLOCKWISE);

	tMt.copyTo(get()->ori_mt);
	get()->ClearImageToOrigin();
	get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->img.scaled(200, 200)));
}

void BaseOperate::onTriggered_picture_rotate180() {
	cv::Mat _mt;
	_mt = get()->mt;
	cv::Mat tMt;
	cv::rotate(_mt, tMt, cv::RotateFlags::ROTATE_180);

	tMt.copyTo(get()->ori_mt);
	get()->ClearImageToOrigin();
	get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->img.scaled(200, 200)));
}