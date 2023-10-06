#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

class HistImage {
public:
    HistImage():ori_mt(cv::imread("../resource/bigImages/2.png",cv::ImreadModes::IMREAD_GRAYSCALE)) {
        cv::imshow("ori_mt", ori_mt);
    }
    HistImage(cv::Mat mt) :ori_mt(mt) {}
    cv::Mat getHist(cv::Mat mt) {
        channel = mt.channels();
        histSize = 256;
        float r[2] = {0,256};
        const float* range = { r };
        cv::Mat hist;

        cv::calcHist(&mt, 1, 0, cv::Mat(), hist, 1, &histSize, &range);
        return hist;
    }
    void drawGrayHist(const std::string& title, cv::Mat mt = cv::Mat()) {
        cv::Mat hist;
        if (mt.empty()) {
            hist = getHist(this->ori_mt);
        }
        else {
            hist = getHist(mt);
        }

        auto histHeight = 400;
        auto histWidth = 512;
        //创建直方图
        cv::Mat histogram(histHeight, histWidth, CV_8UC3, cv::Scalar(255, 255, 255));

        auto x_oneWidth = cvRound((double)histWidth / histSize);

        //归一化直方图
        cv::normalize(hist, hist, 0, histHeight,cv::NORM_MINMAX);
        //[0,255]
        for (int i = 0; i < histSize; i++) {
            auto x_oneHeight = cvRound(hist.at<float>(i));
            cv::rectangle(histogram, cv::Point(i * x_oneWidth, histHeight),cv::Point(i*x_oneWidth + x_oneWidth -1,histHeight-x_oneHeight),cv::Scalar(0,0,0));
        }
        cv::imshow(title, histogram);
    }
    void equalizeHist() {
        cv::Mat tMt;
        cv::equalizeHist(ori_mt, tMt);
        cv::imshow("equalize_gray_mt",tMt);
        // 创建一个三通道图像，将均衡化后的灰度图像复制到每个通道
        cv::Mat ori = cv::imread("../resource/bigImages/2.png");

        cv::imshow("ori Image", ori);

        // 合并均衡化后的灰度图像与原始彩色图像的颜色通道
        std::vector<cv::Mat> channels;
        cv::split(ori, channels);
        //channels[1] = tMt; // 使用均衡化后的灰度图像替换蓝色通道

        for (int i = 0; i < 3; i++)
        {
            channels[i] = tMt.clone(); // 使用均衡化后的灰度图像替换每个通道
        }

        cv::merge(channels, ori);

        cv::imshow("Colorized Image", ori);
    }
private:
    cv::Mat ori_mt;
    int channel = 0;
    int dims = 0;
    int histSize = 0;
};

int main()
{
    // 读取图像
    HistImage hist;
    hist.drawGrayHist("origin");
    hist.equalizeHist();
    cv::waitKey(0);


    return 0;
}
