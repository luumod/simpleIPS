#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <QMutex>
#include <QPoint>

class VideoWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget();
public slots:
    void getMat(const cv::Mat& mat);
protected:
    void paintEvent(QPaintEvent *e)override;
    void initializeGL()override;
public:
    QImage img;
    int width,height;
    QPoint drawPos;
private:
    uchar* buf = nullptr;
    QMutex mutex;
};

#endif // VIDEOWIDGET_H
