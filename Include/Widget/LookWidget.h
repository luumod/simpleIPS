#ifndef LOOKWIDGET_H_
#define LOOKWIDGET_H_

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

/*
截取图片的Widget: static
*/

class LookWidget :public QMainWindow {
	Q_OBJECT
    //带菜单栏的独立Widget
private:
    LookWidget(QWidget* parent = nullptr);
    static LookWidget* look;

public:
    ~LookWidget();
    static LookWidget* get(QWidget* parent = nullptr);

	void reset(const QPixmap& pixmap);
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
