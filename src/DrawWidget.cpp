#include "../include/DrawWidget.h"
#include "../include/Enums.h"
#include "../include/GraphicsScene.h"
#include "../include/widget.h"
#include <QButtonGroup>
#include <QPushButton>
#include <QGraphicsView>
#include <QToolButton>
#include <QGridLayout>
#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QDebug>
#include <QGraphicsPixmapItem>

DrawWidget::DrawWidget(QWidget* parent) : QMainWindow(parent) {

	//添加场景
	scene = new GraphicsScene(this);
	scene->setSceneRect(0, 0, 400, 400);

	QGraphicsPixmapItem* pixItem = new QGraphicsPixmapItem;
	QPixmap pixmap = QPixmap::fromImage(get()->img);
	if (!pixmap.isNull()) {
		// 获取场景和图像的大小
		QRectF sceneRect = scene->sceneRect();
		QSize imageSize = pixmap.size();

		// 计算缩放比例
		qreal scaleX = sceneRect.width() / imageSize.width();
		qreal scaleY = sceneRect.height() / imageSize.height();
		qreal scale = qMin(scaleX, scaleY);

		// 设置缩放后的图像
		pixItem->setPixmap(pixmap.scaled(imageSize * scale));
		scene->addItem(pixItem);
	}

	QGraphicsView* view = new QGraphicsView(scene);
	view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->show();

	//添加布局
	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->addWidget(createChoice(),1);
	vlayout->addWidget(view,4);
	QPushButton* save = new QPushButton;
	vlayout->addWidget(save, 1);
	connect(save, &QPushButton::clicked, this, [=]() {
		/*
		保存diy后的场景图片
		*/
		QString saveName = QFileDialog::getSaveFileName(nullptr, "save image", ".", "Images(*.png *.bmp *.jpg)");	 
		QImage img(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
		img.fill(Qt::white);
		QPainter painter(&img);
		scene->render(&painter);

		if (!saveName.isEmpty()) {
			img.save(saveName);
		}
		});

	QWidget* mainWindow = new QWidget;
	mainWindow->setLayout(vlayout);

	// 设置主窗口为应用程序的主窗口
	this->setCentralWidget(mainWindow);
}

DrawWidget::~DrawWidget()
{
}

QWidget* DrawWidget::createToolBtnItemWidget(const QString& text, int id, const QString& fileName)
{
	//创建每一个选项Widget
	QToolButton* btn = new QToolButton;
	group->addButton(btn,id);
	btn->setObjectName("draw_tbtn");
	btn->resize(48, 48);
	btn->setCheckable(true);
	btn->setChecked(false);
	if (!fileName.isEmpty()) {
		btn->setIcon(QIcon(fileName));
		btn->setIconSize(btn->size());
	}
	QGridLayout* grid = new QGridLayout;
	grid->addWidget(btn, 0, 0, Qt::AlignHCenter);
	QLabel* textlab = new QLabel(text);
	textlab->setObjectName("textLab");
	grid->addWidget(textlab, 1, 0, Qt::AlignHCenter);
	QWidget* wid = new QWidget;
	wid->setLayout(grid);

	return wid;
}

void DrawWidget::initialize()
{
	
}

QWidget* DrawWidget::createChoice()
{
	group = new QButtonGroup(this);
	QGridLayout* grid_layout = new QGridLayout;

	grid_layout->addWidget(createToolBtnItemWidget("线性", SHAPE::Line, ""), 0, 0, Qt::AlignHCenter);
	grid_layout->addWidget(createToolBtnItemWidget("矩形", SHAPE::Rect, ""), 0, 1, Qt::AlignHCenter);
	grid_layout->addWidget(createToolBtnItemWidget("椭圆形", SHAPE::Eillipse, ""), 0, 2, Qt::AlignHCenter);
	grid_layout->addWidget(createToolBtnItemWidget("画笔", SHAPE::Path, ""), 0, 3, Qt::AlignHCenter);
	grid_layout->setVerticalSpacing(0);

	connect(group, &QButtonGroup::buttonClicked, this, &DrawWidget::onClicked_choiceShape);

	QWidget* widget_choice = new QWidget;
	widget_choice->setLayout(grid_layout);

	return widget_choice;
}

QWidget* DrawWidget::createTools()
{
	QWidget* widget_tools = new QWidget;
	return widget_tools;
}

void DrawWidget::onClicked_choiceShape(QAbstractButton* btn) {
	scene->type = SHAPE(group->id(btn));
	/*switch (scene->type)
	{
	case SHAPE::Line:
		scene->line = new QGraphicsLineItem;
		break;
	case SHAPE::Eillipse:
		scene->eill = new QGraphicsEllipseItem;
		break;
	case SHAPE::Rect:
		scene->rectItem = new QGraphicsRectItem;
		break;
	case SHAPE::Path:
		scene->pathItem = new QGraphicsPathItem;
		break;
	}*/
}
