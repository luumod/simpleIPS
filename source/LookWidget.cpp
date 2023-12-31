﻿#include "Include/Widget/LookWidget.h"
#include "Include/Widget/ShowImgWidget.h"
#include "Include/assist/Common.h"
#include "Include/Widget/widget.h"
#include "Include/Widget/ShowImgWidget.h"
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

LookWidget* LookWidget::look = nullptr;

Widget* get() {
	return Widget::getInstance();
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

LookWidget *LookWidget::get(QWidget* parent)
{
    if (!look){
        look = new LookWidget(parent);
    }
    return look;
}

void LookWidget::reset(const QPixmap& pixmap)
{
	this->resize(pixmap.size());
	lab_img->setPixmap(pixmap);
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
		QPoint currentPos;
		currentPos = ev->pos();
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

		// 产生图片
		QPixmap select_pixmap = ((QLabel*)scrollArea->widget())->pixmap().copy(adjustedRect);

		//新产生一个窗口
		if (!handle_widget) {
			handle_widget = new ShowImgWidget(this);
			connect(handle_widget, &ShowImgWidget::signal_file_reload, this, [=]() {
				this->hide();
				});
		}
		handle_widget->lab_img->setPixmap(select_pixmap);
		handle_widget->show();
	}
	QWidget::mouseReleaseEvent(ev);
}
