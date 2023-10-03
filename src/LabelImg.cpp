#include "../include/other_functions/LabelImg.h"
#include "../include/widget.h"

#include <QMouseEvent>
#include <QHBoxLayout>
#include <QRubberBand>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QPixmap>
#include <QLabel>
#include <QRect>


Widget* get() {
	return Widget::getInstance();
}

/*
Main_Label
*/

Main_Label::Main_Label(QWidget* parent)
	:QLabel(parent)
{
	this->setScaledContents(true);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	this->setObjectName("main_lab_img");
	this->setContextMenuPolicy(Qt::CustomContextMenu);

	rubber = new QRubberBand(QRubberBand::Line, this);
}

Main_Label::~Main_Label()
{
	if (selectedRegionDisplay) {
		delete selectedRegionDisplay;
		selectedRegionDisplay = nullptr;
	}
}

void Main_Label::mouseDoubleClickEvent(QMouseEvent* ev)
{

	QWidget::mouseDoubleClickEvent(ev);
}


void Main_Label::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		isSelecting_ = true;
		origin = ev->pos();
		rubber->setGeometry(QRect(origin, QSize()));
		rubber->show();
	}
	//如果之前存在，则重新设置
	if (selectedRegionDisplay && selectedRegionDisplay->isVisible()) {
		selectedRegionDisplay->hide();
	}

	QWidget::mousePressEvent(ev);
}

void Main_Label::mouseMoveEvent(QMouseEvent* ev)
{
	if (isSelecting_) {
		isMoving = true;
		rubber->setGeometry(QRect(origin, ev->pos()).normalized());
	}
	QWidget::mouseMoveEvent(ev);
}

void Main_Label::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton && isSelecting_ && isMoving) {
		isSelecting_ = false;
		isMoving = false;
		QRect rubber_area = rubber->geometry();
		rubber->hide();

		QPixmap select_pixmap = get()->lab_img->pixmap().copy(rubber_area);
		// 在新窗口中显示所选区域
		if (!selectedRegionDisplay) {
			selectedRegionDisplay = new LookWidget;
			selectedRegionDisplay->setWindowTitle("所选区域");
		}
		selectedRegionDisplay->setLab_img(select_pixmap);
		selectedRegionDisplay->show();
	}
	QWidget::mouseReleaseEvent(ev);
}


Sub_Label::Sub_Label(QWidget* parent)
	:QLabel(parent)
{
	
}

Sub_Label::~Sub_Label() {
	if (curr_img) {
		delete curr_img;
		curr_img = nullptr;
	}
}

void Sub_Label::enterEvent(QEnterEvent* ev)
{
	if (curr_img) {
		delete curr_img;
		curr_img = nullptr;
	}
	curr_img = new QPixmap(get()->lab_img->pixmap());
	Widget::getInstance()->lab_img->setPixmap(get()->sub_lab_img->pixmap());
}

void Sub_Label::leaveEvent(QEvent* ev)
{
	if (curr_img) {
		get()->lab_img->setPixmap(*curr_img);
	}
}

void Sub_Label::mousePressEvent(QMouseEvent* ev)
{
	get()->on_action_previewToNormal_triggered();
}




/*
LookWidget
*/

LookWidget::LookWidget(QWidget* parent)
	:QMainWindow(parent)
{
	createAction();
	createMenuBar();
}

void LookWidget::createAction()
{
	action_save = new QAction(tr("保存图片"), this);
	action_save->setStatusTip(tr("保存此图片"));
	action_save->setShortcut(tr("Ctrl+S"));
	connect(action_save, &QAction::triggered, this, [=]() {
		//保存加工后的图片
		QString FileName = QFileDialog::getSaveFileName(nullptr, "save image", ".", "Images(*.png *.bmp *.jpg)");
		if (!FileName.isEmpty()) {
			auto lab = this->findChild<QLabel*>();
			if (!lab->pixmap().isNull()) {
				lab->pixmap().save(FileName);
			}
		}
		});
}

void LookWidget::createMenuBar()
{
	QMenu* menu = menuBar()->addMenu("文件");
	menu->addAction(action_save);
}

void LookWidget::setLab_img(QPixmap select_pixmap)
{
	this->setMinimumSize(select_pixmap.size());
	this->setMaximumSize(select_pixmap.size());

	QLabel* label = new QLabel;
	label->setPixmap(select_pixmap);
	label->setAlignment(Qt::AlignCenter);
	label->setContentsMargins(0, 0, 0, 0);
	QHBoxLayout* lay = new QHBoxLayout;
	lay->addWidget(label);
	lay->setContentsMargins(1, 1, 1, 1);
	QWidget* w = new QWidget;
	w->setLayout(lay);

	this->setCentralWidget(w);
}