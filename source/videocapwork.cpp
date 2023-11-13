#include "Include/video/videocapwork.h"
#include "Include/video/framefilter.h"
#include <QThread>
#include <QDebug>
#include <iostream>
#include <QMessageBox>

static int i =1;
VideoCapWork* VideoCapWork::videoCapWork = nullptr;
VideoCapWork::VideoCapWork()
{
    videoCap.open("videos/3.mp4");
    // 检查视频是否成功打开
    if (!videoCap.isOpened()) {
        std::cerr << "Error: Unable to open the video file." << std::endl;
        exit(0);
    }
    fps = videoCap.get(cv::CAP_PROP_FPS);
    fps_count = videoCap.get(cv::CAP_PROP_FRAME_COUNT );
    width = videoCap.get(cv::CAP_PROP_FRAME_WIDTH);
    height = videoCap.get(cv::CAP_PROP_FRAME_HEIGHT);
    qInfo()<<"width: "<<width << "height: "<<height;
}

VideoCapWork *VideoCapWork::getInstance()
{
    if (!videoCapWork){
        videoCapWork = new VideoCapWork;
    }
    return videoCapWork;
}

VideoCapWork::~VideoCapWork()
{
    videoCap.release();
}

bool VideoCapWork::startExportVideo(const std::string& fileName)
{
    if (!videoCap.isOpened()){
        return false;
    }
    videoWriter.open(fileName,cv::VideoWriter::fourcc('X','2','6','4'),fps,cv::Size(width,height));
    if (!videoWriter.isOpened()){
        std::cerr<<"视频保存器打开失败" <<'\n';
        return false;
    }
    isWriter = true; //开始导出
    return true;
}

bool VideoCapWork::stopExportVideo()
{
    isWriter = false;
    videoWriter.release();
    return true;
}

void VideoCapWork::captureVideoFrame()
{
    cv::Mat frame;

    while (true) {//读取每一帧

        mutex.lock();

        if (isStop){
            break;
        }

        if (pauseVideo){ //手动阻塞线程
            mutex.unlock();
            cv::waitKey(50);
            continue;
        }

        if (!videoCap.read(frame) || frame.empty()){
            mutex.unlock();
            cv::waitKey(50);
            //播放完成
            QMessageBox::information(nullptr, "播放完成", "视频播放已经完成！");
            break;
        }
        //原始图片
        emit havingVideoFrame(frame);

        //进行操作
        cv::Mat dstFrame = frameFilter::getInstance()->filter(frame); //dst

        qInfo()<<"还在工作" << i++;
        //目标操作图片
        emit havingVideoFrameDst(dstFrame);

        if (isWriter){
           videoWriter.write(dstFrame);
        }

        cv::waitKey(1000 / fps); //1s 内每帧的刷新速率
        mutex.unlock();
    }
}
