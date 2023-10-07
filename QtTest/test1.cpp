#include <QtWidgets>
#include <QPixmap>
#include <QScreen>
#include <QMouseEvent>

class Screenshot : public QWidget {
public:
    Screenshot(QWidget* parent =nullptr) :QWidget(parent){
        setWindowTitle("Screenshot App");
        setGeometry(100, 100, 800, 600);
        setCursor(Qt::CrossCursor);

        screenshotLabel.setAlignment(Qt::AlignCenter);
        screenshotLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(&screenshotLabel);

        screenshotInProgress = false;

        //connect(this, &Screenshot::regionSelected, this, &Screenshot::captureScreenshot);
    }

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            screenshotStartPoint = event->pos();
            screenshotInProgress = true;
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (screenshotInProgress) {
            screenshotEndPoint = event->pos();
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton && screenshotInProgress) {
            screenshotEndPoint = event->pos();
            screenshotInProgress = false;
         //   emit regionSelected();

            captureScreenshot();
        }
    }

    void paintEvent(QPaintEvent* event) override {
        if (screenshotInProgress) {
            QPixmap screenshot = originalScreenshot;
            QPainter painter(&screenshot);
            painter.setPen(Qt::red);
            painter.drawRect(QRect(screenshotStartPoint, screenshotEndPoint));
            screenshotLabel.setPixmap(screenshot);
        }
    }

private:
    void captureScreenshot() {
        if (!originalScreenshot.isNull()) {
            int x = qMin(screenshotStartPoint.x(), screenshotEndPoint.x());
            int y = qMin(screenshotStartPoint.y(), screenshotEndPoint.y());
            int width = qAbs(screenshotEndPoint.x() - screenshotStartPoint.x());
            int height = qAbs(screenshotEndPoint.y() - screenshotStartPoint.y());

            QPixmap screenshot = originalScreenshot.copy(x, y, width, height);
            screenshot.save("F:/code/qt_opencv/myPhotoshopApp/bin/test");  // 保存截图为图片文件，你可以自定义文件名和路径
            screenshotLabel.setPixmap(screenshot.scaled(screenshotLabel.size(), Qt::KeepAspectRatio));
        }
    }

public:
    QLabel screenshotLabel;
    bool screenshotInProgress;
    QPoint screenshotStartPoint;
    QPoint screenshotEndPoint;
    QPixmap originalScreenshot;

//signals:
//    void regionSelected();
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Screenshot screenshotApp;
    screenshotApp.show();
    

    QScreen* screen = QGuiApplication::primaryScreen();
    auto logicalDpi = screen->logicalDotsPerInch();

    // 计算缩放比例
    float scale = static_cast<float>(logicalDpi) / 96.0;

    qInfo() << logicalDpi << scale;


    if (screen) {
        screenshotApp.originalScreenshot = screen->grabWindow(0);
        screenshotApp.screenshotLabel.setPixmap(screenshotApp.originalScreenshot);
    }

    return app.exec();
}
