#ifndef QIMAGE2MAT_H_
#define QIMAGE2MAT_H_

#include <opencv2/opencv.hpp>
#include <QImage>

inline cv::Mat QImage2Mat(const QImage& image)
{
    // 检查输入的 QImage 是否为空
    if (image.isNull()) {
        return cv::Mat();
    }

    // 根据 QImage 的格式确定 OpenCV 的图像类型和颜色通道顺序
    int cvType;
    int cvColorOrder;
    switch (image.format()) {
    case QImage::Format_RGB888:
        cvType = CV_8UC3;
        cvColorOrder = cv::COLOR_RGB2BGR;
        break;
    case QImage::Format_BGR888:
        cvType = CV_8UC3;
        cvColorOrder = cv::COLOR_BGR2RGB;
        break;
    case QImage::Format_Indexed8:
        cvType = CV_8UC1;
        cvColorOrder = -1; // 无需颜色通道顺序转换
        break;
    default:
        // 不支持的图像格式
        return cv::Mat();
    }

    // 创建一个与 QImage 大小和类型匹配的 OpenCV 矩阵
    cv::Mat mat(image.height(), image.width(), cvType, const_cast<uchar*>(image.bits()), image.bytesPerLine());

    // 如果需要，进行颜色通道顺序转换
    if (cvColorOrder != -1) {
        cv::cvtColor(mat, mat, cvColorOrder);
    }

    // 复制数据（如果需要深拷贝，请使用 mat.clone()）
    return mat.clone();
}

#endif