#include <QApplication>
#include <QMainWindow>
#include <QSlider>
#include <iostream>
#include <QLabel>
#include <QBoxLayout>
#include <QListWidget>

const int W_ICONSIZE = 100; //图片宽度
const int H_ICONSIZE = 100; //图片高度
const int H_ITEMSIZE = 110; //单元项高度(因为单元项既包括图片又包括文本)

class Widget :public QMainWindow {
public:
	Widget(QMainWindow* parent = nullptr)
		:QMainWindow(parent){
		this->resize(640, 480);

		QLabel* mainImageLabel = new QLabel;
		mainImageLabel->setPixmap(QPixmap("../resource/dog.png").scaled(640, 480));
		// 创建子图片 QLabel，并加载子图片
		QLabel* subImageLabel = new QLabel(mainImageLabel); // 将子图片添加到主图片的父控件上
		subImageLabel->setPixmap(QPixmap("../resource/dog.png").scaled(100, 100));


		// 设置子图片在主图片上的位置
		subImageLabel->move(this->width() - 100, this->height() - 100);

		// 使用布局管理器将主图片和子图片添加到窗口中
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(mainImageLabel);

		QWidget* w = new QWidget;
		w->setLayout(layout);

		this->setCentralWidget(w);

	}
private:
	QLabel* lab = nullptr;
};


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Widget w;
	w.show();

	return a.exec();
}