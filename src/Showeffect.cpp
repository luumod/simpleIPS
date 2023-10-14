#include "../include/opencv_functions/Showeffect.h"
#include "../include/widget.h"
#include "../include/assist/Res.h"
#include <QDebug>
#include "../include/assist/Mat2QImage.h"

Showeffect::Showeffect()
	:Object()
{
}
Showeffect::~Showeffect() {}

void Showeffect::initialize()
{
	 bright_value = 0;
	 gamma_value = 1.0;

	//对比度线性展宽
	 linear_g1 = 0.0, linear_g2 = 0.0;//假定目标区域
	 linear_mode = 0; //灰度还是彩色

	//灰级窗
	 gray_f1 = 0.0, gray_f2 = 0.0;

	//线性动态范围调整
	 dp_a = 0.0, dp_b = 0.0;
	 DpLinear_mode = 0; //灰度还是彩色

	//非线性动态范围调整
	 n_dp_c = 1.0;
	 NoneDpLinear_mode = 0;  //灰度还是彩色
}

void Showeffect::Bright()
{
	cv::Mat _mt;
	getMat(_mt);
	cv::Mat tMt = cv::Mat::zeros(_mt.size(), _mt.type());

	//调整亮度
	for (int i = 0; i < _mt.rows; i++) {
		for (int j = 0; j < _mt.cols; j++) {
			for (int z = 0; z < _mt.channels(); z++) {
				tMt.at<cv::Vec3b>(i, j)[z] = cv::saturate_cast<uchar>(_mt.at<cv::Vec3b>(i, j)[z] + this->bright_value);
			}
		}
	}

	Object::update(tMt);
}

void Showeffect::Gamma()
{
	cv::Mat _mt;
	getMat(_mt);
	//标准化为 0-255之间
	cv::Mat normalizedImage;
	cv::normalize(_mt, normalizedImage, 0, 255, cv::NormTypes::NORM_MINMAX);

	//转换为浮点型
	cv::Mat tMt;
	normalizedImage.convertTo(tMt, CV_32FC3);

	//γ矫正
	cv::Mat GammaMat;
	cv::pow(tMt / 255.0, gamma_value, GammaMat);
	//CV_32FC3类型需要进一步转换为CV_8UC3
	Object::update(GammaMat);
}

cv::Mat Showeffect::showContrastLinearBroaden(cv::Mat mat)
{
	//对比度线性展宽 (f1,f2) --> (g1,g2)
	cv::Mat grayMat;

	cv::Mat _mt;
	getMat(_mt);
	if (mat.empty()) {
		//转换为灰度图
		cv::cvtColor(_mt, grayMat, cv::COLOR_BGR2GRAY);
	}
	else {
		grayMat = mat;
	}

	double f1, f2;
	cv::minMaxLoc(grayMat, &f1, &f2);
	double g1 = this->linear_g1;
	double g2 = this->linear_g2;


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
	if (!linear_mode){ // !=0 彩色无须更新
		Object::update(grayMat);
	}
	return grayMat;
}

void Showeffect::showBGRContrastLinearBroaden()
{
	std::vector<cv::Mat> channels;
	cv::Mat _mt;
	getMat(_mt);
	cv::split(_mt, channels);
	cv::Mat ch1 = showContrastLinearBroaden(channels[0]);
	cv::Mat ch2 = showContrastLinearBroaden(channels[1]);
	cv::Mat ch3 = showContrastLinearBroaden(channels[2]);
	cv::Mat res;
	cv::merge(channels, res);

	Object::update(res);//彩色图
}

void Showeffect::showGrayWindow()
{
	//灰级窗处理
	cv::Mat _mt;
	getMat(_mt);
	cv::Mat grayMat;
	cv::cvtColor(_mt, grayMat, cv::COLOR_BGR2GRAY);

	double k = 255 / (gray_f2 - gray_f1);

	for (int i = 0; i < grayMat.rows; i++) {
		for (int j = 0; j < grayMat.cols; j++) {
			uchar pix = grayMat.at<uchar>(i, j);
			if (pix >= 0 && pix < gray_f1) {
				grayMat.at<uchar>(i, j) = 0;
			}
			else if (pix >= gray_f1 && pix < gray_f2) {
				grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(k * (pix - gray_f1));
			}
			else if (pix >= gray_f2 && pix < 255) {
				grayMat.at<uchar>(i, j) = 0;
			}
		}
	}
	Object::update(grayMat);
}

cv::Mat Showeffect::showDynamicLinearAdj(cv::Mat mat)
{
	//对比度线性展宽 (f1,f2) --> (g1,g2)
	cv::Mat grayMat;

	cv::Mat _mt;
	getMat(_mt);
	if (mat.empty()) {
		//转换为灰度图
		cv::cvtColor(_mt, grayMat, cv::COLOR_BGR2GRAY);
	}
	else {
		grayMat = mat;
	}

	double f1, f2;
	cv::minMaxLoc(grayMat, &f1, &f2);

	double k = 255 / (dp_b - dp_a);

	for (int i = 0; i < grayMat.rows; i++) {
		for (int j = 0; j < grayMat.cols; j++) {
			uchar pix = grayMat.at<uchar>(i, j);
			if (pix >= 0 && pix < f1) {
				grayMat.at<uchar>(i, j) = 0;
			}
			else if (pix >= f1 && pix < f2) {
				grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(k * (pix - dp_a));
			}
			else if (pix >= f2 && pix <= 255) {
				grayMat.at<uchar>(i, j) = 255;
			}
		}
	}
	if (!linear_mode) { // !=0 彩色无须更新
		Object::update(grayMat);
	}
	return grayMat;
}

void Showeffect::showBGRDynamicLinearAdj()
{
	std::vector<cv::Mat> channels;
	cv::Mat _mt;
	getMat(_mt);
	cv::split(_mt, channels);
	cv::Mat ch1 = showDynamicLinearAdj(channels[0]);
	cv::Mat ch2 = showDynamicLinearAdj(channels[1]);
	cv::Mat ch3 = showDynamicLinearAdj(channels[2]);
	cv::Mat res;
	cv::merge(channels, res);

	Object::update(res);//彩色图
}

cv::Mat Showeffect::showNoneDynamicLinearAdj(cv::Mat mat)
{
	//对比度线性展宽 (f1,f2) --> (g1,g2)
	cv::Mat grayMat;

	cv::Mat _mt;
	getMat(_mt);
	if (mat.empty()) {
		//转换为灰度图
		cv::cvtColor(_mt, grayMat, cv::COLOR_BGR2GRAY);
	}
	else {
		grayMat = mat;
	}

	for (int i = 0; i < grayMat.rows; i++) {
		for (int j = 0; j < grayMat.cols; j++) {
			uchar pix = grayMat.at<uchar>(i, j);
			grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(n_dp_c * cv::log(1+pix));
		}
	}
	if (!linear_mode) { // !=0 彩色无须更新
		Object::update(grayMat);
	}
	return grayMat;
}

void Showeffect::showBGRNoneDynamicLinearAdj()
{
	std::vector<cv::Mat> channels;
	cv::Mat _mt;
	getMat(_mt);
	cv::split(_mt, channels);
	cv::Mat ch1 = showNoneDynamicLinearAdj(channels[0]);
	cv::Mat ch2 = showNoneDynamicLinearAdj(channels[1]);
	cv::Mat ch3 = showNoneDynamicLinearAdj(channels[2]);
	cv::Mat res;
	cv::merge(channels, res);

	Object::update(res);//彩色图
}

void Showeffect::choice_NoneDpLinearAlgorithm()
{
	if (!NoneDpLinear_mode) {
		//灰度图
		showNoneDynamicLinearAdj();
	}
	else {
		showBGRNoneDynamicLinearAdj();
	}
}


void Showeffect::choice_DpLinearAlgorithm()
{
	if (!DpLinear_mode) {
		//灰度图
		showDynamicLinearAdj();
	}
	else {
		showBGRDynamicLinearAdj();
	}
}

void Showeffect::choice_linearAlgorithm()
{
	if (!linear_mode) {
		//灰度图
		showContrastLinearBroaden();
	}
	else {
		showBGRContrastLinearBroaden();
	}
}


void Showeffect::onTriggered_slider_valueChange_brighten(int bright_value)
{
	this->bright_value = bright_value;
	Bright();
}

void Showeffect::onTriggered_slider_valueChange_gamma(double gamma_value)
{
	this->gamma_value = gamma_value;
	Gamma();
}

void Showeffect::onTriggered_slider_valueChange_linearg1(double g1)
{
	this->linear_g1 = g1;
	choice_linearAlgorithm();
}

void Showeffect::onTriggered_slider_valueChange_linearg2(double g2)
{
	this->linear_g2 = g2;
	choice_linearAlgorithm();
}


void Showeffect::onTriggered_slider_valueChange_GaryWindowF1(double value)
{
	this->gray_f1 = value;
	showGrayWindow();
}

void Showeffect::onTriggered_slider_valueChange_GaryWindowF2(double value)
{
	this->gray_f2 = value;
	showGrayWindow();
}

void Showeffect::onTriggered_slider_valueChange_DynamicA(double value)
{
	this->dp_a = value;
	choice_DpLinearAlgorithm();
}

void Showeffect::onTriggered_slider_valueChange_DynamicB(double value)
{
	this->dp_b = value;
	choice_DpLinearAlgorithm();
}

void Showeffect::onTriggered_slider_valueChange_NoneDynamicC(double value)
{
	this->n_dp_c = value;
	choice_NoneDpLinearAlgorithm();
}