#include "Widget/ShowImgWidget.h"
#include "assist/QImage2Mat.h"
#include "Widget/widget.h"
#include "assist/Res.h"
#include "Widget/LookWidget.h"
#include <QMenu>
#include <QAction>
#include <QScrollArea>
#include <QLabel>
#include <QFileDialog>
#include <QMenuBar>

ShowImgWidget::ShowImgWidget(QWidget* parent)
	:QMainWindow(parent)
{
	this->setWindowTitle("截取的图片");

	lab_img = new QLabel(this);
	createAction();
	createMenuBar();

	this->setCentralWidget(lab_img);
}

ShowImgWidget::~ShowImgWidget() {}

void ShowImgWidget::createMenuBar()
{
	menu_file = menuBar()->addMenu(tr("文件"));
	menu_file->addAction(action_save);
	menu_file->addAction(action_reload);
}

void ShowImgWidget::createAction()
{
	action_save = new QAction("保存", this);
	connect(action_save, &QAction::triggered, this, [=]() {
		QString FileName = QFileDialog::getSaveFileName(nullptr, "save image", ".", "Images(*.png *.bmp *.jpg)");
	if (!FileName.isEmpty()) {
		lab_img->pixmap().save(FileName);
	}
		});

	action_reload = new QAction("加载到程序", this);
	connect(action_reload, &QAction::triggered, this, [=]() {
		QImage image = lab_img->pixmap().toImage();

		cv::Mat	mt = QImage2Mat(image);

		//首先保存到文件 
		QString FileName = QFileDialog::getSaveFileName(nullptr, "save image", ".", "Images(*.png *.bmp *.jpg)");
		if (!FileName.isEmpty()) {
			lab_img->pixmap().save(FileName);
		}

		Widget* widget = Widget::getInstance();
		//然后再加载到程序,从文件中加载
		widget->reload_Resources_ScrollArea(FileName);

		//通知父窗口隐藏，自己也隐藏
		this->hide();
		emit signal_file_reload();
		});
}
