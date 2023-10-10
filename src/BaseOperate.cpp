#include "../include/widget.h"
#include "../include/assist/Res.h"
#include "../include/opencv_functions/BaseOperate.h"
#include "../include/assist/Mat2QImage.h"
#include "../include/Widget/LabelImg.h"
#include "../include/Widget/ShowImgWidget.h"
#include <QLabel>

BaseOperate::BaseOperate()
	:Object()
{
	if (!show_wid) {
		show_wid = new ShowImgWidget;
	}
}

BaseOperate::~BaseOperate()
{
	if (show_wid) {
		delete show_wid;
		show_wid = nullptr;
	}
}

void BaseOperate::initialize()
{
}

void BaseOperate::cvtColor(QAction* action) {

	cv::Mat _mt;
	//每次都对原始根图片进行操作，然后获取一份副本，因为重复的cvtColor操作会导致图片重叠转换！！！
	_mt = get()->res->root_mt; 
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
	get()->res->inter_mt = ori_mt_res;
	get()->updateFromIntermediate();
}

void BaseOperate::onTriggered_picture_rotate90() {
	cv::Mat _mt;
	//对当前lab_img显示的Mat进行操作
	_mt = get()->res->curr_mt;
	cv::Mat tMt;
	cv::rotate(_mt, tMt, cv::RotateFlags::ROTATE_90_CLOCKWISE);
	get()->res->inter_mt = tMt;
	get()->updateFromIntermediate();
}

void BaseOperate::onTriggered_picture_rotate180() {
	cv::Mat _mt;
	_mt = get()->res->curr_mt;
	cv::Mat tMt;
	cv::rotate(_mt, tMt, cv::RotateFlags::ROTATE_180);
	get()->res->inter_mt = tMt;
	get()->updateFromIntermediate();
}

void BaseOperate::onTriggered_picture_rotate270() {
	//逆时针旋转90度
	cv::Mat _mt;
	_mt = get()->res->curr_mt;
	cv::Mat tMt;
	cv::rotate(_mt, tMt, cv::RotateFlags::ROTATE_90_COUNTERCLOCKWISE);
	get()->res->inter_mt = tMt;
	get()->updateFromIntermediate();
}

void BaseOperate::onTriggered_picture_flip(int flag)
{
	cv::Mat _mt;
	_mt = get()->res->curr_mt;
	cv::Mat tMt;
	cv::flip(_mt, tMt, flag);
	get()->res->inter_mt = tMt;
	get()->updateFromIntermediate();
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
	_mt = get()->res->curr_mt;
	cv::Mat tMt;
	cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
		0, -1, 0,
		-1, 5, -1,
		0, -1, 0
		);
	cv::filter2D(_mt, tMt, _mt.depth(), kernel);
	get()->res->inter_mt = tMt;
	get()->updateFromIntermediate();
}

void BaseOperate::drawGrayHist(const std::string& title, cv::Mat mt) {
	cv::Mat hist;
	cv::Mat grayMt;
	cv::cvtColor(get()->res->curr_mt, grayMt, cv::ColorConversionCodes::COLOR_BGR2GRAY);
	if (mt.empty()) {
		hist = getHist(grayMt);
	}
	else {
		hist = getHist(mt);
	}

	auto histHeight = 400;
	auto histWidth = 512;
	//创建直方图
	cv::Mat histogram(histHeight, histWidth, CV_8UC3, cv::Scalar(255, 255, 255));

	auto x_oneWidth = cvRound((double)histWidth / 256);

	//归一化直方图
	cv::normalize(hist, hist, 0, histHeight, cv::NORM_MINMAX);
	//[0,255]
	for (int i = 0; i < 256; i++) {
		auto x_oneHeight = cvRound(hist.at<float>(i));
		cv::rectangle(histogram, cv::Point(i * x_oneWidth, histHeight), cv::Point(i * x_oneWidth + x_oneWidth - 1, histHeight - x_oneHeight), cv::Scalar(0, 0, 0));
	}

	//cv::Mat rgbMat;
	//cv::cvtColor(histogram, rgbMat, cv::COLOR_BGR2RGB);
	//QImage image(rgbMat.data, rgbMat.cols, rgbMat.rows, static_cast<int>(rgbMat.step), QImage::Format_RGB888);
	show_wid->lab_img->setPixmap(QPixmap::fromImage(Mat2QImage(histogram)));

	show_wid->show();
}


void BaseOperate::showEqualizedBGRImage()
{
	cv::Mat tMt;
	cv::Mat grayMt;
	std::vector<cv::Mat> channels;
	cv::split(get()->res->curr_mt, channels);
	cv::Mat m1,m2,m3;
	cv::equalizeHist(channels[0], m1);
	cv::equalizeHist(channels[1], m2);
	cv::equalizeHist(channels[2], m3);
	cv::merge(std::vector<cv::Mat>{m1, m2, m3}, tMt);

	show_wid->lab_img->setPixmap(QPixmap::fromImage(Mat2QImage(tMt)));
	show_wid->show();
}

void BaseOperate::showEqualizedGrayImage()
{
	cv::Mat grayMat;
	cv::cvtColor(get()->res->curr_mt, grayMat, cv::ColorConversionCodes::COLOR_BGR2GRAY);
	cv::Mat tMt;
	cv::equalizeHist(grayMat, tMt);

	show_wid->lab_img->setPixmap(QPixmap::fromImage(Mat2QImage(tMt)));
	show_wid->show();
}
cv::Mat BaseOperate::showContrastLinearBroaden(cv::Mat mat)
{
	//对比度线性展宽 (f1,f2) --> (g1,g2)
	cv::Mat grayMat;
	//转换为灰度图
	if (mat.empty()) {
		cv::cvtColor(get()->res->curr_mt, grayMat, cv::COLOR_BGR2GRAY);
	}
	else {
		grayMat = mat;
	}

	double f1, f2;
	cv::minMaxLoc(grayMat, &f1, &f2);
	double g1 = 0.0, g2 = 255.0;

	double k1 = g1 / f1;
	double k2 = (g2 - g1) / (f2 - f1);
	double k3 = (255 - g2) / (255 - f2);

	for (int i = 0; i < grayMat.rows; i++) {
		for (int j = 0; j < grayMat.cols; j++) {
			uchar pix = grayMat.at<uchar>(i, j);
			if (pix >= 0 && pix < f1) {
				grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(k1 * pix);
			}
			else if (pix >= f1 && pix <= f2) {
				grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(k2 * (pix - f1) + g1);
			}
			else if (pix > f2 && pix <= 255) {
				grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(k3 * (pix - f2) + g2);
			}
		}
	}
	show_wid->lab_img->setPixmap(QPixmap::fromImage(Mat2QImage(grayMat)));
	show_wid->show();
	return grayMat;
}

void BaseOperate::showBGRContrastLinearBroaden()
{
	std::vector<cv::Mat> channels;
	cv::split(get()->res->curr_mt, channels);
	cv::Mat ch1 = showContrastLinearBroaden(channels[0]);
	cv::Mat ch2 = showContrastLinearBroaden(channels[1]);
	cv::Mat ch3 = showContrastLinearBroaden(channels[2]);
	cv::Mat res;
	cv::merge(channels, res);

	show_wid->lab_img->setPixmap(QPixmap::fromImage(Mat2QImage(res)));
	show_wid->show();
}

/*
------- help function -----------
*/
cv::Mat BaseOperate::getHist(cv::Mat mt) {
	int channel = mt.channels();
	int histSize = 256;
	float r[2] = { 0,256 };
	const float* range = { r };
	cv::Mat hist;

	cv::calcHist(&mt, 1, 0, cv::Mat(), hist, 1, &histSize, &range);
	return hist;
}