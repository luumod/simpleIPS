#include "../include/Widget/CaptureWidget.h"
#include "../include/Widget/ShowImgWidget.h"
#include "../include/assist/config.h"
#include <QApplication>
#include <QClipboard>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QTimer>
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
#include <QRect>
#include <QMainWindow>
#include <QScrollBar>
#include <QMessageBox>

CaptureWidget::CaptureWidget(QWidget* parent)
	:QMainWindow(parent)
{
	createAction();
	createMenuBar();
	this->setStyleSheet("border: 2px solid red");

	lab_img = new QLabel(this);
	this->setCentralWidget(lab_img);
	rubber = new QRubberBand(QRubberBand::Line, lab_img);
	update();
}

CaptureWidget::~CaptureWidget()
{
	if (handle_widget) {
		delete handle_widget;
		handle_widget = nullptr;
	}
}

void CaptureWidget::createMenuBar()
{
}

void CaptureWidget::createAction()
{
	action_esc = new QAction("退出",this);
	action_esc->setShortcut(QKeySequence(Qt::Key_Escape));
	connect(action_esc, &QAction::triggered, this, [=]() {
		this->hide(); 
		});
}

void CaptureWidget::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		isSelecting_ = true;
		origin = lab_img->mapFromGlobal(ev->pos());
		rubber->setGeometry(QRect(origin, QSize()));
		rubber->show();
	}
	if (ev->button() == Qt::RightButton) {
		isSelecting_ = true;
		origin = lab_img->mapFromGlobal(ev->pos());
		rubber->setGeometry(QRect(origin, QSize()));
		rubber->show();
	}
	QWidget::mousePressEvent(ev);
}

void CaptureWidget::mouseMoveEvent(QMouseEvent* ev)
{
	if (isSelecting_) {
		isMoving = true;
		QPoint currentPos;
		currentPos = lab_img->mapFromGlobal(ev->pos());
		rubber->setGeometry(QRect(origin, currentPos).normalized());
		update();
	}
	QWidget::mouseMoveEvent(ev);
}

void CaptureWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton && isSelecting_ && isMoving) {
		isSelecting_ = false;
		isMoving = false;
		QRect rubber_area = rubber->geometry();
		rubber->hide();
		QRect dst_area = QRect(rubber_area.topLeft() * scale,rubber_area.size());

		// 产生图片
		QPixmap select_pixmap = lab_img->pixmap().copy(dst_area);

		//保存到剪切板
		QClipboard* clip = QApplication::clipboard();
		clip->setPixmap(select_pixmap);

		//提示
		QMessageBox* messageBox = new QMessageBox;
		messageBox->setText("图片已保存到剪切板");
		messageBox->setWindowTitle("提示");
		messageBox->setIcon(QMessageBox::Information);
		messageBox->show();
		messageBox->setStandardButtons(QMessageBox::NoButton);

		QTimer::singleShot(500, messageBox, [=]() {
			messageBox->deleteLater();
			});
		this->hide();
	}
	else if (ev->button() == Qt::RightButton && isSelecting_ && isMoving) {
		//右键打开新的窗口
		isSelecting_ = false;
		isMoving = false;
		QRect rubber_area = rubber->geometry();
		rubber->hide();
		QRect dst_area = QRect(rubber_area.topLeft() * scale, rubber_area.size());

		// 产生图片
		QPixmap select_pixmap = lab_img->pixmap().copy(dst_area);
		this->hide();

		//新产生一个窗口
		if (!handle_widget) {
			handle_widget = new ShowImgWidget;
		}
		handle_widget->lab_img->setPixmap(select_pixmap);
		handle_widget->show();
	}
	QWidget::mouseReleaseEvent(ev);
}