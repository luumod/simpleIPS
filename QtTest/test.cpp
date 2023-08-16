#include <QApplication>
#include <QWidget>
#include <QSlider>

class Widget :public QWidget {
public:
	Widget(QWidget* parent = nullptr)
		:QWidget(parent){
		s = new QSlider(this);
		connect(s, &QSlider::valueChanged, this, [=](int value) {
			qInfo() << "valueChanged: " << value;
			});
		connect(s, &QSlider::sliderMoved, this, [=](int value) {
			qInfo() << "sliderMoved: " << value;
			});


		s->setValue(10);
	}
private:
	QSlider* s = nullptr;
};


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Widget w;
	w.show();

	return a.exec();
}