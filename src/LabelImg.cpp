#include "../include/Widget/LabelImg.h"
#include "../include/Widget/ShowImgWidget.h"
#include "../include/assist/Common.h"
#include "../include/widget.h"

#include <QMouseEvent>
#include <QHBoxLayout>
#include <QRubberBand>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QPixmap>
#include <QLabel>
#include <QRect>
#include <QScrollArea>
#include <QPushButton>
#include <QMainWindow>
#include <QScrollBar>

#define DEBUG 0

Widget* get() {
	return Widget::getInstance();
}

/*
Main_Label
*/

Main_Label::Main_Label(QWidget* parent)
	:QLabel(parent)
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);
}

Main_Label::~Main_Label()
{
	
}

/*
LookWidget
*/

LookWidget::LookWidget(QWidget* parent)
	:QMainWindow(parent)
{
	this->setWindowTitle("源图片");

	scrollArea = new QScrollArea(this);
	lab_img = new QLabel(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(lab_img);

	this->setCentralWidget(scrollArea);

	rubber = new QRubberBand(QRubberBand::Line, scrollArea);
}



LookWidget::~LookWidget() {
	if (handle_widget) {
		delete handle_widget;
		handle_widget = nullptr;
	}
}

void LookWidget::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		isSelecting_ = true;
		origin = ev->pos();
		rubber->setGeometry(QRect(origin, QSize()));
		rubber->show();
	}
	QWidget::mousePressEvent(ev);
}

void LookWidget::mouseMoveEvent(QMouseEvent* ev)
{
	if (isSelecting_) {
		isMoving = true;
		QPoint currentPos = ev->pos();
		
		rubber->setGeometry(QRect(origin, currentPos).normalized());
	}
	QWidget::mouseMoveEvent(ev);
}

void LookWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton && isSelecting_ && isMoving) {
		isSelecting_ = false;
		isMoving = false;
		QRect rubber_area = rubber->geometry();
		rubber->hide();


		// 获取截取区域
		QScrollBar* horizontalScrollBar = scrollArea->horizontalScrollBar();
		QScrollBar* verticalScrollBar = scrollArea->verticalScrollBar();

		// 计算滚动条偏移量
		int horizontalOffset = horizontalScrollBar->value();
		int verticalOffset = verticalScrollBar->value();

		// 调整截取区域坐标
		QRect adjustedRect(rubber_area.x() + horizontalOffset, rubber_area.y() + verticalOffset, rubber_area.width(), rubber_area.height());

#if DEBUG
		qInfo() << "偏移: " << horizontalOffset << verticalOffset;
		qInfo() << "原始: " << rubber_area;
		qInfo() << "修正: " << adjustedRect;
#endif
		// 产生图片
		QPixmap select_pixmap = ((QLabel*)scrollArea->widget())->pixmap().copy(adjustedRect);

		//新产生一个窗口
		if (!handle_widget) {
			handle_widget = new ShowImgWidget(this);
			
		}
		handle_widget->lab_img->setPixmap(select_pixmap);
		handle_widget->show();
	}
	QWidget::mouseReleaseEvent(ev);
}
