#ifndef MAT2QIMAGE_H
#define MAT2QIMAGE_H

#include <QImage>
#include <opencv2/opencv.hpp>
#include "Enums.h"

inline QImage Mat2QImage(const cv::Mat& mat)
{
    QImage image;
    if (mat.type() == CV_8UC1) {
        image = QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8);
        return image;
    }
    else if (mat.type() == CV_32FC3) {
        cv::Mat cvImage8Bit;
        mat.convertTo(cvImage8Bit, CV_8UC3, 255.0);
        // 创建QImage
        return Mat2QImage(cvImage8Bit);
    }

    if (mat.empty()) {
        return QImage();
    }

    // Convert the color space of the OpenCV matrix to RGB
    cv::Mat rgbMat;
    cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);

    //返回结果
    image = QImage(rgbMat.cols, rgbMat.rows, QImage::Format_RGB888);

    uchar* imagePtr = image.bits();
    size_t imageBytesPerLine = image.bytesPerLine();
    const uchar* matPtr = rgbMat.data;
    size_t matBytesPerLine = rgbMat.step;

    for (int i = 0; i < image.height(); i++) {
        memcpy(imagePtr, matPtr, imageBytesPerLine);
        imagePtr += imageBytesPerLine;
        matPtr += matBytesPerLine;
    }

    return image;
}

#endif // MAT2QIMAGE_H


