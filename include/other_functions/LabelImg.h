#ifndef LABELIMG_H_
#define LABELIMG_H_

#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QPixmap>
#include <QMainWindow>
class Widget;
class QMouseEvent;
class QRubberBand;

class QPoint;
class LookWidget;
class QAction;
/*
实现对于预览图片的操作
*/

Widget* get();

class Sub_Label :public QLabel {
	Q_OBJECT
public:
	using QLabel::QLabel;
	~Sub_Label();
protected:
	void enterEvent(QEnterEvent* ev)override;
	void leaveEvent(QEvent* ev)override;
	void mousePressEvent(QMouseEvent* ev)override;
private:
	QPixmap* curr_img = nullptr;
};

/*
对主图片的操作
*/

class Main_Label :public QLabel {
	Q_OBJECT
public:
	Main_Label(QWidget* parent = nullptr);
	~Main_Label();
protected:
	void mouseDoubleClickEvent(QMouseEvent* ev)override;

	void mousePressEvent(QMouseEvent* ev)override;
	void mouseMoveEvent(QMouseEvent* ev)override;
	void mouseReleaseEvent(QMouseEvent* ev)override;
private:
	QRubberBand* rubber = nullptr;
	QPoint origin;
	bool isSelecting_ = false;
	bool isMoving = false;
	LookWidget* selectedRegionDisplay = nullptr;

};

class LookWidget :public QMainWindow {
	Q_OBJECT
public:
	LookWidget(QWidget* parent = nullptr);
	void createAction();
	void createMenuBar();
	void setLab_img(QPixmap select_pixmap);
	
	QAction* action_save = nullptr;
	QLabel* lab_img = nullptr;
};

#endif // !LABELIMG_H_
