#include <opencv2/opencv.hpp>

cv::Mat contrastLinearBroaden(cv::Mat ori) {
    //对比度线性展宽 (f1,f2) --> (g1,g2)
    cv::Mat grayMat = ori;
    //cv::cvtColor(ori, grayMat, cv::COLOR_BGR2GRAY);

    double f1, f2;
    cv::minMaxLoc(grayMat, &f1, &f2);
    double g1 = 0.0, g2 = 255.0;

    double k1 = g1 / f1;
    double k2 = (g2 - g1) / (f2 - f1);
    double k3 = (255 - g2) / (255 - f2);

    for (int i = 0; i < grayMat.rows; i++) {
        for (int j = 0; j < grayMat.cols; j++) {
            uchar pix = grayMat.at<uchar>(i, j);
            if (pix >= 0 && pix < f1) {
                grayMat.at<uchar>(i, j) =cv::saturate_cast<uchar>( k1 * pix);
            }
            else if (pix>=f1 && pix<=f2) {
                grayMat.at<uchar>(i, j) =cv::saturate_cast<uchar>( k2 * (pix - f1) + g1);
            }
            else if (pix > f2 && pix <= 255) {
                grayMat.at<uchar>(i, j) =cv::saturate_cast<uchar>( k3 * (pix - f2) + g2);
            }
        }
    }
    return grayMat;
}

int main() {
    // 读取图像
    cv::Mat image = cv::imread("../resource/testImages/106.png");

    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    cv::Mat ch1 = contrastLinearBroaden(channels[0]);
    cv::Mat ch2 = contrastLinearBroaden(channels[1]);
    cv::Mat ch3 = contrastLinearBroaden(channels[2]);
    cv::Mat res;
    cv::merge(channels, res);
    cv::imshow("2", image);
    cv::imshow("1", res);
    cv::waitKey(0);
    return 0;
}
