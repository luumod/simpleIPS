#include "Include/video/framefilter.h"
#include "Include/video/frameoperator.h"

frameFilter* frameFilter::filt = nullptr;
frameFilter *frameFilter::getInstance()
{
    if (!filt){
        filt = new frameFilter;
    }
    return filt;
}

cv::Mat frameFilter::filter(const cv::Mat &mat)
{
    auto optor = frameOperator::getInstance();
    optor->initMat(mat);

    for (auto& x:tasks){
        switch (x.opt) {
        case Type::Blur:
            optor->blur(x.params[0]);
            break;
        case Type::Bright:
            optor->bright(x.params[0]);
            break;
        default:
            break;
        }
    }

    return optor->dst;
}

void frameFilter::addTask(const Task& task)
{
    mutex.lock();
    tasks.push_back(task);
    mutex.unlock();
}

void frameFilter::clearTask()
{
    mutex.lock();
    tasks.clear();
    mutex.unlock();
}

frameFilter::frameFilter()
{

}
