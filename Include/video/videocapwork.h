#ifndef VIDEOCAPWORK_H
#define VIDEOCAPWORK_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QMutex>


class VideoCapWork:public QObject
{
    Q_OBJECT
public:
    static VideoCapWork* videoCapWork;
    static VideoCapWork* getInstance();
    ~VideoCapWork();
private:
    VideoCapWork();

public:
    bool startExportVideo(const std::string& fileName);
    bool stopExportVideo();

    void openVideo(const std::string& name);
public slots:
    void captureVideoFrame();
signals:
    void havingVideoFrame(const cv::Mat& mat);
    void havingVideoFrameDst(const cv::Mat& mat);
    void sendFrameToProcessTask(const cv::Mat& mat);
public:
    cv::VideoCapture videoCap;
    cv::VideoWriter videoWriter;
    double fps; //视频帧率
    double fps_count; //视频帧数
    int width,height;
    bool isWriter = false;
    bool pauseVideo = false;
    bool isStop = false; //已经停止
    QMutex mutex;
};

#endif // VIDEOCAPWORK_H
