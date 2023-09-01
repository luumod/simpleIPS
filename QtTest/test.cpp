#include <QApplication>
#include <QWidget>
#include <QToolBox>
#include <QToolButton>
#include <QGridLayout>
#include <QLabel>
#include <QButtonGroup>
#include <opencv2/opencv.hpp>
#include <QPixmap>

class Widget:public QWidget {
public:
	Widget(QWidget* parent = nullptr):QWidget(parent) {
		this->resize(300, 300);
		QPixmap p("D:\\于良浩\\1173012900\\FileRecv\\MobileFile\\IMG_20230831_202813.jpg");
		QPixmap pp = p.scaled(200, 200, Qt::KeepAspectRatio);
		QLabel* lab = new QLabel;
		lab->setPixmap(pp);
		lab->show();
	}
	~Widget() {}
private:
};

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	
	Widget w;
	w.show();

	return a.exec();
}