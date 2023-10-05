#include <QMainWindow>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QWidget>
#include <QWheelEvent>
#include <QApplication>
#include <QPixmap>

class ImageViewer : public QMainWindow {
public:
    ImageViewer(QWidget* parent = nullptr) : QMainWindow(parent) {
        // 创建滚动区域
        scrollArea = new QScrollArea;
        scrollArea->setBackgroundRole(QPalette::Dark);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFixedSize(700, 700);
        setCentralWidget(scrollArea);

        // 创建标签用于显示图片
        image = QPixmap(tr(("../resource/bigImages/2.png")));

        imageLabel = new QLabel;
        //imageLabel->setScaledContents(true);
        //一开始为空
        //imageLabel->setBackgroundRole(QPalette::Dark);
        scrollArea->setWidget(imageLabel);

        // 连接滚动条变化事件
        connect(scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &ImageViewer::updateThumbnail);
        connect(scrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, this, &ImageViewer::updateThumbnail);

        // 初始化
        scaleFactor = 1.0;
        maxScale =qMax( qMax(scrollArea->size().height() / image.size().height(), scrollArea->size().width() / image.size().width()),qMax(image.size().height() / scrollArea->size().height(), image.size().width() / scrollArea->size().width()) );
    }

protected:
    void wheelEvent(QWheelEvent* event) override {
        if (event->modifiers() & Qt::ControlModifier) {
            int delta = event->angleDelta().y();
            double scaleAmount = delta > 0 ? 1.1 : 0.9;
            //限制最大缩放
            if (scaleFactor* scaleAmount < maxScale) {
                scaleFactor *= scaleAmount;
                updateThumbnail();
            }
        }
        QMainWindow::wheelEvent(event);
    }

private slots:
    void updateThumbnail() {
        if (!image.isNull()) {
            // 缩放图像
            QPixmap scaledPixmap = image.scaled(image.size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            imageLabel->setPixmap(scaledPixmap);
            // 检查是否需要启用滚动条
            bool needScrollbars = scaledPixmap.size().width() > scrollArea->size().width() || scaledPixmap.size().height() > scrollArea->size().height();
            scrollArea->setHorizontalScrollBarPolicy(needScrollbars ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
            scrollArea->setVerticalScrollBarPolicy(needScrollbars ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
        }
    }

private:
    QScrollArea* scrollArea;
    QLabel* imageLabel;
    QPixmap image;
    double scaleFactor;
    double maxScale;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ImageViewer viewer;
    viewer.show();

    return app.exec();
}
