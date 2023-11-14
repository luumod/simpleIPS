#include "Include/opencv_functions/Showeffect.h"
#include "Include/Widget/widget.h"
#include "Include/assist/Res.h"
#include "Include/assist/Mat2QImage.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>

void warning() {
    // 创建警告对话框
    QMessageBox warningBox;
    warningBox.setIcon(QMessageBox::Warning);
    warningBox.setWindowTitle("警告");
    warningBox.setText("您正在对灰度图进行操作，因此无法转化为彩色图。");

    // 添加“确定”按钮
    warningBox.addButton(QMessageBox::Ok);

    // 显示对话框并等待用户响应
    warningBox.exec();

}

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


void Showeffect::Bright(const cv::Mat &_mt, cv::Mat &tMt,int value) {
     // 调整亮度
     for (int i = 0; i < _mt.rows; i++) {
         for (int j = 0; j < _mt.cols; j++) {
             if (_mt.channels() == 1) {
                 // 单通道图像
                 tMt.at<uchar>(i, j) = cv::saturate_cast<uchar>(_mt.at<uchar>(i, j) + value);
             } else {
                 // 多通道图像
                 for (int z = 0; z < _mt.channels(); z++) {
                     tMt.at<cv::Vec3b>(i, j)[z] = cv::saturate_cast<uchar>(_mt.at<cv::Vec3b>(i, j)[z] + value);
                 }
             }
         }
     }
}



void Showeffect::Gamma(const cv::Mat& _mt, cv::Mat& tMt) {
     for (int i = 0; i < _mt.rows; i++) {
         for (int j = 0; j < _mt.cols; j++) {
             if (_mt.channels() == 1) {
                 // 单通道图像
                 float pix = static_cast<float>(_mt.at<uchar>(i, j)) / 255.0;
                 float res = gamma_c * std::pow(pix, gamma_value) * 255.0;
                 tMt.at<uchar>(i, j) = cv::saturate_cast<uchar>(res);
             } else {
                 // 多通道图像
                 for (int z = 0; z < _mt.channels(); z++) {
                     float pix = static_cast<float>(_mt.at<cv::Vec3b>(i, j)[z]) / 255.0;
                     float res = gamma_c * std::pow(pix, gamma_value) * 255.0;
                     tMt.at<cv::Vec3b>(i, j)[z] = cv::saturate_cast<uchar>(res);
                 }
             }
         }
     }
}


cv::Mat Showeffect::showContrastLinearBroaden(cv::Mat mat)
{
	//对比度线性展宽 (f1,f2) --> (g1,g2)
	cv::Mat grayMat;

	cv::Mat _mt;
	getMat(_mt);
    if (mat.empty()){
        //直接对灰度图处理
        if (_mt.channels() == 1){
            grayMat = _mt.clone();
        }
        else{
            cv::cvtColor(_mt, grayMat, cv::COLOR_BGR2GRAY);
        }
    }
    else{
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
            float pix =cv::saturate_cast<float>( grayMat.at<uchar>(i, j));
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
    if (channels.size() == 1){
        warning();
        return;
    }
    channels[0] = showContrastLinearBroaden(channels[0]);
    channels[1] = showContrastLinearBroaden(channels[1]);
    channels[2] = showContrastLinearBroaden(channels[2]);
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
    if (_mt.channels()!=1){
        cv::cvtColor(_mt, grayMat, cv::COLOR_BGR2GRAY);
    }
    else{
        grayMat = _mt.clone();
    }

	double k = 255 / (gray_f2 - gray_f1);

	for (int i = 0; i < grayMat.rows; i++) {
        for (int j = 0; j < grayMat.cols; j++) {
            float pix = cv::saturate_cast<float>(grayMat.at<uchar>(i, j));
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
    if (mat.empty()){
        //直接对灰度图处理
        if (_mt.channels() == 1){
            grayMat = _mt.clone();
        }
        else{
            cv::cvtColor(_mt, grayMat, cv::COLOR_BGR2GRAY);
        }
    }
    else{
        grayMat = mat;
    }

	double f1, f2;
	cv::minMaxLoc(grayMat, &f1, &f2);

    float k = 255 / (dp_b - dp_a);

	for (int i = 0; i < grayMat.rows; i++) {
        for (int j = 0; j < grayMat.cols; j++) {
            float pix =cv::saturate_cast<float>(grayMat.at<uchar>(i, j));
            if (pix >= 0 && pix < f1) {
                grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(0);
			}
			else if (pix >= f1 && pix < f2) {
				grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(k * (pix - dp_a));
			}
            else if (pix >= f2 && pix <= 255) {
                grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(255);
			}
		}
	}
    if (!DpLinear_mode) { // !=0 彩色无须更新
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
    if (channels.size() == 1){
        warning();
        return;
    }
    channels[0] = showDynamicLinearAdj(channels[0]);
    channels[1] = showDynamicLinearAdj(channels[1]);
    channels[2] = showDynamicLinearAdj(channels[2]);
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
    if (mat.empty()){
        //直接对灰度图处理
        if (_mt.channels() == 1){
            grayMat = _mt.clone();
        }
        else{
            cv::cvtColor(_mt, grayMat, cv::COLOR_BGR2GRAY);
        }
    }
    else{
        grayMat = mat;
    }

	for (int i = 0; i < grayMat.rows; i++) {
		for (int j = 0; j < grayMat.cols; j++) {
            float pix = cv::saturate_cast<float>(grayMat.at<uchar>(i, j));
            float trans = cv::saturate_cast<float>(n_dp_c * std::log10(1+pix));
            grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(trans);
		}
    }

    if (!NoneDpLinear_mode) { // !=0 彩色无须更新
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
    if (channels.size() == 1){
        warning();
        return;
    }
    channels[0] = showNoneDynamicLinearAdj(channels[0]);
    channels[1] = showNoneDynamicLinearAdj(channels[1]);
    channels[2] = showNoneDynamicLinearAdj(channels[2]);
    cv::Mat res;
    cv::merge(channels, res);
	Object::update(res);//彩色图
}

void Showeffect::choice_NoneDpLinearAlgorithm()
{
	if (!NoneDpLinear_mode) {
		showNoneDynamicLinearAdj();
	}
	else {
        showBGRNoneDynamicLinearAdj();
    }
}

cv::Mat Showeffect::showNormalNoneDynamicLinearAdj(cv::Mat mat)
{
    cv::Mat grayMat;

    cv::Mat _mt;
    getMat(_mt);
    if (mat.empty()){
        //直接对灰度图处理
        if (_mt.channels() == 1){
            grayMat = _mt.clone();
        }
        else{
            cv::cvtColor(_mt, grayMat, cv::COLOR_BGR2GRAY);
        }
    }
    else{
        grayMat = mat;
    }

    for (int i = 0; i < grayMat.rows; i++) {
        for (int j = 0; j < grayMat.cols; j++) {
            float pix = cv::saturate_cast<float>(grayMat.at<uchar>(i, j));
            float trans = cv::saturate_cast<float>(pix * pix / 255);
            grayMat.at<uchar>(i, j) = cv::saturate_cast<uchar>(trans);
        }
    }

    if (!NormalNoneDpLinear_mode) { // !=0 彩色无须更新
        Object::update(grayMat);
    }
    return grayMat;
}

void Showeffect::show_NormalNoneDpLinearAlgorithm()
{
    std::vector<cv::Mat> channels;
    cv::Mat _mt;
    getMat(_mt);
    cv::split(_mt, channels);
    if (channels.size() == 1){
        warning();
        return;
    }
    channels[0]= showNormalNoneDynamicLinearAdj(channels[0]);
    channels[1]= showNormalNoneDynamicLinearAdj(channels[1]);
    channels[2] = showNormalNoneDynamicLinearAdj(channels[2]);
    cv::Mat res;
    cv::merge(channels, res);

    Object::update(res);//彩色图
}

void Showeffect::choice_NormalNoneDpLinearAlgorithm()
{
    if (!NormalNoneDpLinear_mode) {
        showNormalNoneDynamicLinearAdj();
    }
    else {
        show_NormalNoneDpLinearAlgorithm();
    }
}


void Showeffect::choice_DpLinearAlgorithm()
{
	if (!DpLinear_mode) {
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

    cv::Mat src = Object::getMat2(),tMt = src.clone();
    Bright(src,tMt,bright_value);
    Object::update(tMt);
}

void Showeffect::onTriggered_slider_valueChange_gamma_C(double gamma_C){
    this->gamma_c = gamma_C;

    cv::Mat src = Object::getMat2(),tMt = src.clone();
    Gamma(src,tMt);
    Object::update(tMt);
}

void Showeffect::onTriggered_slider_valueChange_gamma(double gamma_value)
{
    this->gamma_value = gamma_value;

    cv::Mat src = Object::getMat2(),tMt = src.clone();
    Gamma(src,tMt);
    Object::update(tMt);
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

void Showeffect::onReturnPressed_Bright_Edit(QList<QString> strs)
{
	int val = strs[0].toInt();
    this->bright_value = val;

    cv::Mat src = Object::getMat2(),tMt = src.clone();
    Bright(src,tMt,bright_value);
    Object::update(tMt);
}

void Showeffect::onReturnPressed_Gamma_Edit(QList<QString> strs)
{
	this->gamma_value = strs[0].toInt();

    /*三部曲：
     * 1. 获取
     * 2. 操作
     * 3. 显示
    */
    cv::Mat src = Object::getMat2(),tMt = src.clone();
    Gamma(src,tMt);
    Object::update(tMt);
}

void Showeffect::onReturnPressed_Linear_Edit(QList<QString> strs)
{
	this->linear_g1 = strs[0].toInt();
	this->linear_g2 = strs[1].toInt();
	choice_linearAlgorithm();
}

void Showeffect::onReturnPressed_Gray_Edit(QList<QString> strs)
{
	this->gray_f1 = strs[0].toInt();
	this->gray_f2 = strs[1].toInt();
	showGrayWindow();
}

void Showeffect::onReturnPressed_DpLinear_Edit(QList<QString> strs)
{
	this->dp_a = strs[0].toInt();
	this->dp_b = strs[1].toInt();
	choice_DpLinearAlgorithm();
}

void Showeffect::onReturnPressed_NDpLinear_Edit(QList<QString> strs)
{
	this->n_dp_c = strs[0].toInt();
	choice_NoneDpLinearAlgorithm();
}

void Showeffect::onReturnPressed_Edit(QList<QString> strs)
{
	switch (current_choice)
	{
	case SHOW::LIGHT:
		onReturnPressed_Bright_Edit(strs);
		return;
	case SHOW::GAMMA:
		onReturnPressed_Gamma_Edit(strs);
		return;
	case SHOW::LINEAR:
		onReturnPressed_Linear_Edit(strs);
		return;
	case SHOW::GRAYWINDOW:
		onReturnPressed_Gray_Edit(strs);
		return;
	case SHOW::DPLINEAR:
		onReturnPressed_DpLinear_Edit(strs);
		return;
	case SHOW::NON_DPLINEAR:
		onReturnPressed_NDpLinear_Edit(strs);
		return;
	default:
		return;
	}
	return;
}
