#include "Include/video/videocapwork.h"
#include "Include/video/framefilter.h"
#include "Include/Widget/widget.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QDebug>
#include <iostream>
#include <QMessageBox>

static int i =1;
VideoCapWork* VideoCapWork::videoCapWork = nullptr;
VideoCapWork::VideoCapWork()
{
}

bool VideoCapWork::openVideo(const std::string& name){
    if (videoCap.isOpened()){
        videoCap.release();
    }
    videoCap.open(name);
    // 检查视频是否成功打开
    if (!videoCap.isOpened()) {
        return false;
    }
    fps = videoCap.get(cv::CAP_PROP_FPS);
    fps_count = videoCap.get(cv::CAP_PROP_FRAME_COUNT );
    width = videoCap.get(cv::CAP_PROP_FRAME_WIDTH);
    height = videoCap.get(cv::CAP_PROP_FRAME_HEIGHT);
    return true;
}

bool VideoCapWork::cutVideo(const std::string& name)
{
    //切换视频
    return openVideo(name);
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
    if (videoCap.isOpened()){
        videoCap.release();
    }
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
    //qInfo()<<"读取线程：" <<QThread::currentThreadId();

    while (true) {//读取每一帧
        mutex.lock();
        if (isStop){ //关闭视频
            mutex.unlock();
            break;
        }

        if (isCut){  //切换视频
            mutex.unlock();
            cv::waitKey(50);
            cutChangeFinished = true;
            continue;
        }

        if (isPause){ //手动阻塞线程
            mutex.unlock();
            cv::waitKey(50);
            continue;
        }

        if (!videoCap.isOpened()){
            mutex.unlock();
            break;
        }

        if (!videoCap.read(frame) || frame.empty()){
            mutex.unlock();
            break;
        }

        if (cutChangeFinished){ //视频选择完成
            needRefresh = true;
            cutChangeFinished = false;
            emit cutRefresh(); //发送需要刷新的信号
        }

        //原始图片
        emit havingVideoFrame(frame);

        //进行操作
        cv::Mat dstFrame = frameFilter::getInstance()->filter(frame); //dst

        //目标操作图片
        emit havingVideoFrameDst(dstFrame);

        if (isWriter){
           videoWriter.write(dstFrame);
        }

        cv::waitKey(1000 / fps); //1s 内每帧的刷新速率
        mutex.unlock();
    }
}
