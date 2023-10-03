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
	//每次都对原始根图片进行操作，然后获取一份副本，因为重复的cvtColor操作会导致图片重叠转换！！！
	_mt = get()->root_mt; 
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
	get()->inter_mt = ori_mt_res;
	get()->updateFromIntermediate();
	//对子图片进行及时修改，防止出现更新不及时
	get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->curr_img.scaled(200, 200)));
}

void BaseOperate::onTriggered_picture_rotate90() {
	cv::Mat _mt;
	//对当前lab_img显示的Mat进行操作
	_mt = get()->curr_mt;
	cv::Mat tMt;
	cv::rotate(_mt, tMt, cv::RotateFlags::ROTATE_90_CLOCKWISE);
	get()->inter_mt = tMt;
	get()->updateFromIntermediate();
	get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->curr_img.scaled(200, 200)));
}

void BaseOperate::onTriggered_picture_rotate180() {
	cv::Mat _mt;
	_mt = get()->curr_mt;
	cv::Mat tMt;
	cv::rotate(_mt, tMt, cv::RotateFlags::ROTATE_180);
	get()->inter_mt = tMt;
	get()->updateFromIntermediate();
	get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->curr_img.scaled(200, 200)));
}

void BaseOperate::onTriggered_picture_rotate270() {
	//逆时针旋转90度
	cv::Mat _mt;
	_mt = get()->curr_mt;
	cv::Mat tMt;
	cv::rotate(_mt, tMt, cv::RotateFlags::ROTATE_90_COUNTERCLOCKWISE);
	get()->inter_mt = tMt;
	get()->updateFromIntermediate();
	get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->curr_img.scaled(200, 200)));
}

void BaseOperate::onTriggered_picture_flip(int flag)
{
	cv::Mat _mt;
	_mt = get()->curr_mt;
	cv::Mat tMt;
	cv::flip(_mt, tMt, flag);
	get()->inter_mt = tMt;
	get()->updateFromIntermediate();
	get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->curr_img.scaled(200, 200)));
}

void BaseOperate::onTriggered_picture_flip0()
{
	onTriggered_picture_flip(0);
}

void BaseOperate::onTriggered_picture_flip1()
{
	onTriggered_picture_flip(1);
}

void BaseOperate::onTriggered_picture_flip_1()
{
	onTriggered_picture_flip(-1);
}

void BaseOperate::onTriggered_picture_mask()
{
	cv::Mat _mt;
	_mt = get()->curr_mt;
	cv::Mat tMt;
	cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
		0, -1, 0,
		-1, 5, -1,
		0, -1, 0
		);
	cv::filter2D(_mt, tMt, _mt.depth(), kernel);
	get()->inter_mt = tMt;
	get()->updateFromIntermediate();
	get()->sub_lab_img->setPixmap(QPixmap::fromImage(get()->curr_img.scaled(200, 200)));
}
