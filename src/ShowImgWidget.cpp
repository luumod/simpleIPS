#include "../include/Widget/ShowImgWidget.h"
#include <QMenu>
#include <QAction>
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
}