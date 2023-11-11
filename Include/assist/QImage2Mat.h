#ifndef QIMAGE2MAT_H
#define QIMAGE2MAT_H

#include <QImage>
#include <opencv2/opencv.hpp>

//转换为CV_8UC4类型
inline cv::Mat QImage2Mat(const QImage& image) {
	cv::Mat mt(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
	return mt;
}

#endif // MAT2QIMAGE_H


