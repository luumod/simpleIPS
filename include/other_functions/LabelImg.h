#ifndef LABELIMG_H_
#define LABELIMG_H_

#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QPixmap>
#include <QMainWindow>
#include <QDialog>

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

Widget* get();

/*
对主图片的操作
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
	HandleJieWidget* handle_widget = nullptr;
	QLabel* lab_img = nullptr;
	QScrollArea* scrollArea = nullptr;
	QRubberBand* rubber = nullptr;
	QPoint origin;
	bool isSelecting_ = false;
	bool isMoving = false;
};

/*
保存截取图片的窗口
*/

class HandleJieWidget :public QMainWindow {
	Q_OBJECT
public:
	HandleJieWidget(QWidget* parent = nullptr);
	~HandleJieWidget();

	void createMenuBar();
	void createAction();
protected:

public:
	QAction* action_save = nullptr;
	QMenu* menu_file = nullptr;

	QLabel* lab_img = nullptr;
};

#endif // !LABELIMG_H_
