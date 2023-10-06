#ifndef LABELIMG_H_
#define LABELIMG_H_

#include <QMainWindow>
#include <QLabel>
#include <QWidget>
class Widget;
class QMouseEvent;
class QRubberBand;
class QPushButton;
class QLabel;
class QPoint;
class QScrollArea;
class LookWidget;
class QAction;
class QMenu;
class QAction;
class HandleJieWidget;
class ShowImgWidget;

Widget* get();

/*
主图片Label
*/

class Main_Label :public QLabel {
	Q_OBJECT
public:
	Main_Label(QWidget* parent = nullptr);
	~Main_Label();
protected:
	/**/
private:
};

/*
截取图片的Widget
*/

class LookWidget :public QMainWindow {
	Q_OBJECT
public:
	LookWidget(QWidget* parent = nullptr);
	~LookWidget();

protected:
	void mousePressEvent(QMouseEvent* ev)override;
	void mouseMoveEvent(QMouseEvent* ev)override;
	void mouseReleaseEvent(QMouseEvent* ev)override;

public:
	ShowImgWidget* handle_widget = nullptr; //单独显示的截图

	QLabel* lab_img = nullptr;
	QScrollArea* scrollArea = nullptr;
	QRubberBand* rubber = nullptr;
	QPoint origin;
	bool isSelecting_ = false;
	bool isMoving = false;
};

#endif // !LABELIMG_H_
