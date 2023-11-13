#include "Include/video/videowidget.h"
#include <QPainter>
#include <QDebug>
#include <QThread>
#include <QPaintEvent>
VideoWidget::VideoWidget(QWidget* parent) : QOpenGLWidget(parent){

}

VideoWidget::~VideoWidget()
{
    if (buf){
        delete[] buf;
        buf = nullptr;
    }
}

void VideoWidget::getMat(const cv::Mat &mat)
{
    //模糊操作
    if (img.isNull()){
        QImage temp(mat.data,mat.cols,mat.rows,mat.step,QImage::Format_BGR888);
        QImage ttemp = temp.scaled(this->size(),Qt::KeepAspectRatio);
        width = ttemp.width();
        height=  ttemp.height();
        width = (width + 3) & ~3;  // 将width调整为最小的4的倍数
        height = (height + 3) & ~3;
        //QImage尺寸会自动调整为4的倍数
        drawPos = QPoint(this->size().width()/2 - width /2 ,this->size().height()/2 - height/2);
        buf = new uchar[width*height*3];
        img=QImage(buf,width,height,QImage::Format_BGR888);
    }

    cv::Mat dst;
    cv::resize(mat,dst,cv::Size(img.width(),img.height())); //缩放
    memcpy(img.bits(),dst.data,dst.rows*dst.cols*dst.elemSize());
    update();
}

void VideoWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter;
    painter.begin(this);
    painter.drawImage(drawPos ,img); //绘制目标操作后图片
    painter.end();
}

void VideoWidget::initializeGL()
{
    // 设置清空颜色为白色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
