#ifndef CAPTUREWIDGET_H_
#define CAPTUREWIDGET_H_

#include <QMainWindow>
#include "LookWidget.h"
#include <QWidget>
class QLabel;
class QPaintEvent;
class QMouseEvent;
class QRect;
class QPixmap;

/*
全屏幕截图Widget
*/

class CaptureWidget:public QMainWindow {
public:
	CaptureWidget(QWidget* parent =nullptr);
	~CaptureWidget();

	void createMenuBar();
	void createAction();

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
public:
	QAction* action_esc = nullptr;

	ShowImgWidget* handle_widget = nullptr; //单独显示的截图
	QLabel* lab_img = nullptr; 
	QRubberBand* rubber = nullptr;
	QPoint origin;
	bool isSelecting_ = false;
	bool isMoving = false;
	double scale = 1.0;
};

#endif // !CAPTUREWIDGET_H_
