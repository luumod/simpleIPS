#ifndef MAT2QIMAGE_H
#define MAT2QIMAGE_H

#include <QImage>
#include <opencv2/opencv.hpp>
#include <QLabel>
#include <QDebug>

inline QImage Mat2QImage(const cv::Mat& mat)
{
    // Check if the input OpenCV matrix is empty
    if (mat.empty()) {
        return QImage();
    }

    // Convert the color space of the OpenCV matrix to RGB
    cv::Mat rgbMat;
    cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);

    //返回结果
    QImage image(rgbMat.cols, rgbMat.rows, QImage::Format_RGB888);

    // Copy the data from the OpenCV matrix to the QImage
    uchar* imagePtr = image.bits();
    int imageBytesPerLine = image.bytesPerLine();
    const uchar* matPtr = rgbMat.data;
    int matBytesPerLine = rgbMat.step;

    for (int i = 0; i < image.height(); i++) {
        memcpy(imagePtr, matPtr, imageBytesPerLine);
        imagePtr += imageBytesPerLine;
        matPtr += matBytesPerLine;
    }

    return image;
}

#endif // MAT2QIMAGE_H


