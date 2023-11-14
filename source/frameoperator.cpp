#include "Include/video/frameoperator.h"
#include "Include/Widget/widget.h"

frameOperator* frameOperator::opera = nullptr;
frameOperator *frameOperator::getInstance()
{
    if (!opera){
        opera = new frameOperator;
    }
    return opera;
}

void frameOperator::initMat(const cv::Mat &mat)
{
    if (mat.empty()){
        return;
    }
    this->src = mat;
    this->dst = src.clone(); //dst默认为src
}

void frameOperator::blur(int value)
{
    cv::blur(src,dst,cv::Size(value,value));
}

void frameOperator::bright(int value)
{
    effect->Bright(src,dst,value);//获取dst
}

frameOperator::frameOperator()
    :effect(Widget::getInstance()->showeffect)
{

}
