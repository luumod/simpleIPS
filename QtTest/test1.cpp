#include <QtWidgets>

class ImageCropper : public QMainWindow
{

public:
    ImageCropper(QWidget* parent = nullptr)
        : QMainWindow(parent)
    {
        // 设置主窗口属性
        setWindowTitle("Image Cropper");
        centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        // 创建布局
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);
        scrollArea = new QScrollArea(centralWidget);
        label = new QLabel(scrollArea);

        // 设置QScrollArea属性
        scrollArea->setWidget(label);
        scrollArea->setWidgetResizable(true);
        layout->addWidget(scrollArea);

        // 设置QRubberBand
        rubberBand = new QRubberBand(QRubberBand::Rectangle, label);
        rubberBand->setGeometry(QRect(0, 0, 0, 0));

        // 设置图片
        label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        label->setScaledContents(true);
        label->setPixmap(QPixmap("../resource/bigImages/2.png")); // 替换成你的图片路径

        // 激活鼠标事件
        label->setMouseTracking(true);

        // 连接信号和槽
        connect(label, SIGNAL(mousePressEvent(QMouseEvent*)), this, SLOT(startSelection(QMouseEvent*)));
        connect(label, SIGNAL(mouseMoveEvent(QMouseEvent*)), this, SLOT(updateSelection(QMouseEvent*)));
        connect(label, SIGNAL(mouseReleaseEvent(QMouseEvent*)), this, SLOT(endSelection(QMouseEvent*)));
    }

private slots:
    void startSelection(QMouseEvent* event)
    {
        // 开始选择截取，记录起始坐标
        selectionStart = event->pos();
        rubberBand->setGeometry(QRect(selectionStart, QSize()));
        rubberBand->show();
    }

    void updateSelection(QMouseEvent* event)
    {
        // 更新选择截取区域
        selectionEnd = event->pos();
        rubberBand->setGeometry(QRect(selectionStart, selectionEnd).normalized());
    }

    void endSelection(QMouseEvent* event)
    {
        // 完成选择截取
        selectionEnd = event->pos();
        rubberBand->setGeometry(QRect(selectionStart, selectionEnd).normalized());

        // 获取选择截取的区域
        QRect selectedRect = rubberBand->geometry();

        // 获取截取区域的坐标和大小
        int x = selectedRect.x();
        int y = selectedRect.y();
        int width = selectedRect.width();
        int height = selectedRect.height();

        // 这里可以执行你的截取操作，例如保存截取的部分或进行其他处理
        // 你可以使用label->pixmap()->copy(selectedRect)来获得选择截取的图像部分
        QLabel* lab = new QLabel;
        lab->setPixmap(label->pixmap().copy(selectedRect));
        lab->show();

        // 隐藏QRubberBand
        rubberBand->hide();
    }

private:
    QWidget* centralWidget;
    QScrollArea* scrollArea;
    QLabel* label;
    QRubberBand* rubberBand;
    QPoint selectionStart;
    QPoint selectionEnd;
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    ImageCropper window;
    window.show();
    return app.exec();
}
