#include "Include/Widget/widget.h"
#include "Include/assist/Res.h"
#include "Include/assist/Mat2QImage.h"
#include "Include/opencv_functions/BaseOperate.h"
#include "Include/Widget/ShowImgWidget.h"
#include "ui_mainwindow.h"
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
    if (action == get()->ui->action_hsv) {
		cv::cvtColor(_mt, ori_mt_res, cv::COLOR_BGR2HSV);
	}
    else if (action == get()->ui->action_hls) {
		cv::cvtColor(_mt, ori_mt_res, cv::COLOR_BGR2HLS);
	}
    else if (action == get()->ui->action_lab) {
		cv::cvtColor(_mt, ori_mt_res, cv::COLOR_BGR2Lab);
	}
    else if (action == get()->ui->action_rgb) {
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

void BaseOperate::drawGrayHist(const QString& title, cv::Mat mt) {
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

	show_wid->lab_img->setPixmap(QPixmap::fromImage(Mat2QImage(histogram)));
    show_wid->setWindowTitle(title);
    show_wid->show();
}

QImage BaseOperate::showGrayHist_AdjArea(const QString &title,int height,int width)
{
    Q_UNUSED(title);
    //直方图的宽高，由外部传入
    auto histHeight = height;
    auto histWidth = width;
    cv::Mat histogram(histHeight, histWidth, CV_8UC3, cv::Scalar(255, 255, 255)); //白色背景

    cv::Mat hist1,hist2,hist3;
    cv::Mat hist;
    std::vector<cv::Mat> channels;
    cv::Mat gray;
    const cv::Mat pp = get()->res->curr_mt;
    if (pp.channels() == 1 || pp.type() == CV_8UC1){
        //灰度图
        gray = pp;
        hist = getHist(gray);
        //灰度图只能获取灰度直方图

        cv::normalize(hist, hist, 0, histHeight, cv::NORM_MINMAX);

        auto x_oneWidth = cvRound((double)histWidth / 256);
        for (int i = 0; i < 256; i++) {
            auto x_oneHeight = cvRound(hist.at<float>(i));
            cv::rectangle(histogram,
                          cv::Point(i * x_oneWidth, histHeight),
                          cv::Point(i * x_oneWidth + x_oneWidth - 1, histHeight - x_oneHeight),
                          cv::Scalar(0,0,0));
        }
    }
    else{
        //彩色图片转换为灰度图
        cv::split(pp,channels);
        hist1 = getHist(channels[0]);
        hist2 = getHist(channels[1]);
        hist3 = getHist(channels[2]);
        //分别获取RGB分量的直方图

        cv::normalize(hist1, hist1, 0, histHeight, cv::NORM_MINMAX);
        cv::normalize(hist2, hist2, 0, histHeight, cv::NORM_MINMAX);
        cv::normalize(hist3, hist3, 0, histHeight, cv::NORM_MINMAX);

        auto x_oneWidth = cvRound((double)histWidth / 256);

        for (int i = 0; i < 256; i++) {
            auto x_oneHeight_B = cvRound(hist1.at<float>(i));
            auto x_oneHeight_G = cvRound(hist2.at<float>(i));
            auto x_oneHeight_R = cvRound(hist3.at<float>(i));
            cv::rectangle(histogram,
                          cv::Point(i * x_oneWidth, histHeight),
                          cv::Point(i * x_oneWidth + x_oneWidth - 1, histHeight - x_oneHeight_B),
                          cv::Scalar(255,0,0));
            cv::rectangle(histogram,
                          cv::Point(i * x_oneWidth, histHeight),
                          cv::Point(i * x_oneWidth + x_oneWidth - 1, histHeight - x_oneHeight_G),
                          cv::Scalar(0,255,0));
            cv::rectangle(histogram,
                          cv::Point(i * x_oneWidth, histHeight),
                          cv::Point(i * x_oneWidth + x_oneWidth - 1, histHeight - x_oneHeight_R),
                          cv::Scalar(0,0,255));
        }
    }

    return Mat2QImage(histogram);
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


/*
------- help function -----------
*/
cv::Mat BaseOperate::getHist(cv::Mat mt) {
	int histSize = 256;
	float r[2] = { 0,256 };
	const float* range = { r };
	cv::Mat hist;

	cv::calcHist(&mt, 1, 0, cv::Mat(), hist, 1, &histSize, &range);
	return hist;
}
