#ifndef FRAMEOPERATOR_H
#define FRAMEOPERATOR_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include "Include/opencv_functions/Showeffect.h"

//任务操作器
class frameOperator
{
public:
    static frameOperator* opera;
    static frameOperator* getInstance();
    void initMat(const cv::Mat& mat);

    //操作
    void blur(int value);
    void bright(int value);

    cv::Mat src;
    cv::Mat dst;

    Showeffect* effect;

private:
    frameOperator();
};

#endif // FRAMEOPERATOR_H
