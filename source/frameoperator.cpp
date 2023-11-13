#include "Include/video/frameoperator.h"

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
    this->src1 = mat;
    this->dst = src1.clone(); //dst默认为src
}

void frameOperator::blur(int value)
{
    cv::blur(src1,dst,cv::Size(value,value));
}

void frameOperator::bright(int value)
{

}

frameOperator::frameOperator()
{

}
