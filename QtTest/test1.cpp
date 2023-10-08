#include <QtWidgets>

class Res :public QObject {
public:
    
    Res(QObject* parent = nullptr) 
        :QObject(parent){
        QString fileName = QFileDialog::getOpenFileName(nullptr, "choice file", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), "file(*.png *.jpg)");
        if (!fileName.isEmpty()) {
            qInfo() << fileName;
        }
    }
};


class Widget :public QMainWindow {
public:
    static Widget* widget;

    static Widget* getInstance() {
        if (widget == nullptr) {
            widget = new Widget;
        }
        return widget;
    }

    Widget(QWidget* parent = nullptr)
        :QMainWindow(parent){
        res = new Res(this);
    }
private:
    Res* res = nullptr;
};

Widget* Widget::widget = nullptr;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Widget* w = Widget::getInstance();
    w->show();

    QObject::connect(&app, &QApplication::aboutToQuit, [=]() {delete w;});

    return app.exec();
}
