#include <opencv2/opencv.hpp>

int main() {
    // 读取图像
    cv::Mat image = cv::imread("dog.png");
    if (image.empty()) {
        std::cerr << "无法加载图像" << std::endl;
        return -1;
    }

    // 定义Gamma值
    double gamma = 3.0; // 根据需要调整Gamma值

    // 缩放像素值范围到0到255
    cv::Mat normalizedImage;
    cv::normalize(image, normalizedImage, 0, 255, cv::NORM_MINMAX);

    // 将归一化后的图像转换为浮点型
    cv::Mat floatImage;
    //浮点型Mat是浮点类型
    normalizedImage.convertTo(floatImage, CV_32F);

     // 使用cv::pow进行幂运算
     cv::Mat gammaCorrected;
     cv::pow(floatImage / 255.0, gamma, gammaCorrected);

     cv::Mat cvImage8Bit;
     gammaCorrected.convertTo(cvImage8Bit, CV_8UC3, 255.0);

     cv::imshow("1", cvImage8Bit);

     // 显示结果
    cv::imshow("origin Corrected Image", image);
    cv::imshow("Gamma Corrected Image", gammaCorrected);
    cv::waitKey(0);

    return 0;
}
