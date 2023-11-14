#ifndef FRAMEFILTER_H
#define FRAMEFILTER_H

#include <vector>
#include <QObject>
#include <QMutex>
#include <opencv2/opencv.hpp>

enum class Type{ Blur,Bright};

struct Task{
    Type opt;//当前操作类型
    std::vector<int> params;
};

class frameFilter
{

public:
    static frameFilter* filt;
    static frameFilter* getInstance();
    cv::Mat filter(const cv::Mat& mat);
    void addTask(const Task& task);
    void clearTask();

private:
    frameFilter();
    std::vector<Task> tasks;

    QMutex mutex;
};

#endif // FRAMEFILTER_H
