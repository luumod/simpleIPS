#include "../include/widget.h"
#include "../include/belongsToEnum.h"
#include "../include/Mat2QImage.h"
#include "../include/Blur.h"
#include "../include/Threshold.h"
#include "../include/Form.h"
#include <QDebug>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <opencv2/core/utils/logger.hpp>
#include <QToolButton>
#include <QButtonGroup>
#include <QToolBox>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QApplication>
#include <QProgressBar>
#include <QSlider>
#include <QGroupBox>
#include <QFormLayout>
#include <QPainter>
#include <QStackedWidget>
#include <QSizePolicy>
#include <QToolBar>


Widget::Widget(QWidget* parent)
	:QMainWindow(parent),
	mt(cv::imread("../resource/dog.png")),
	ori_img(Mat2QImage(mt))
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
	resize(880,780);
	this->move(200,50);
	initFunction();

	lab_img = new QLabel;

	createAction();
	createMenu();
	createToolBar();
	createToolBox();

	//处理如何设置上下文菜单CustomContextMenu：发出customContextMenuRequested信号
	lab_img->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(lab_img, &QLabel::customContextMenuRequested, this, [=](const QPoint& pos) {
		context_menu->exec(QCursor::pos());
		});
	
	lab_img->setPixmap(QPixmap::fromImage(ori_img));	
	lab_img->setScaledContents(true);
	lab_img->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	//右侧布局添加图片与调整栏
	vlayout_right = new QVBoxLayout;
	vlayout_right->addWidget(lab_img,4);

	//很多的调整框
	stack_tools = new QStackedWidget;
	stack_tools->addWidget(create_GUIAvgBlur());     //0
	stack_tools->addWidget(create_GUIGaussianBlur());//1
	stack_tools->addWidget(create_GUIMedianBlur());  //2
	stack_tools->addWidget(create_GUIBilateralBlur());//3
	stack_tools->addWidget(create_GUIThreshoild());	//4
	stack_tools->addWidget(create_GUIMorphology());	//5
	stack_tools->setVisible(false);

	vlayout_right->addWidget(stack_tools, 1);

	//右侧主窗口
	QWidget* picture_show = new QWidget;
	picture_show->setLayout(vlayout_right);
	picture_show->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	//主程序布局
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(toolbox_side); //左侧主窗口
	layout->addWidget(picture_show); //右侧主窗口

	QWidget* mainWindow = new QWidget(this);
	mainWindow->setLayout(layout);

	this->setCentralWidget(mainWindow);
}

Widget::~Widget()
{
	//内存释放
}

void Widget::initFunction()
{
	//初始化
	blur = new Blur(mt); 
	threshold = new Threshold(mt);
	morphology = new Form(mt);
}

void Widget::onClicked_buttonGroup_blur(QAbstractButton* btn) 
{
	stack_tools->setVisible(true);
	int id = btngroup_blur->id(btn); //获取按下的按钮的id
	if (id == BLUR::Average) {
		//均值
		stack_tools->setCurrentIndex(0);
	}
	else if (id == BLUR::Gaussian) {
		//高斯
		stack_tools->setCurrentIndex(1);
	}
	else if (id == BLUR::Median) {
		//中值
		stack_tools->setCurrentIndex(2);
	}
	else if (id == BLUR::Bilateral) {
		stack_tools->setCurrentIndex(3);
	}
	QList<QAbstractButton*> btns = btngroup_blur->buttons();
	for (auto& m_btn : btns) {
		if (m_btn == btn) {
			m_btn->setChecked(true);
			m_btn->setStyleSheet("background-color:yellow");
		}
		else {
			m_btn->setChecked(false);
			clearCurrentPageSliderValue();
			m_btn->setStyleSheet("background-color:blue");
		}
	}
	onTriggered_action_restore();
}

void Widget::onClicked_buttonGroup_threshold(QAbstractButton* btn)
{
	stack_tools->setVisible(true);
	stack_tools->setCurrentIndex(4);
	QList<QAbstractButton*> btns = btngroup_threshold->buttons();
	for (auto& m_btn : btns) {
		if (m_btn == btn) {
			m_btn->setChecked(true);
			m_btn->setStyleSheet("background-color:yellow");
		}
		else {
			m_btn->setChecked(false);
			m_btn->setStyleSheet("background-color:blue");
		}
	}
	int id = btngroup_threshold->id(btn); //获取按下的按钮的id

	int type = cv::ThresholdTypes(id-THRESHOLD::Binary);
	//选择当前阈值模式
	threshold->current_choice = type;
	onTriggered_action_restore();
}

void Widget::onClicked_buttonGroup_form(QAbstractButton* btn)
{
	stack_tools->setVisible(true);
	stack_tools->setCurrentIndex(5);
	QList<QAbstractButton*> btns = btngroup_form->buttons();
	for (auto& m_btn : btns) {
		if (m_btn == btn) {
			m_btn->setChecked(true);
			m_btn->setStyleSheet("background-color:yellow");
		}
		else {
			m_btn->setChecked(false);
			clearCurrentPageSliderValue();
			m_btn->setStyleSheet("background-color:blue");
		}
	}
	int id = btngroup_form->id(btn); //获取按下的按钮的id
	int type = cv::MorphTypes(id - FORM::Erode);
	if (type == cv::MorphTypes::MORPH_HITMISS) {
		if (mt.type() != CV_8UC1){
			//只有CV_8UC1可以使用MORPH_HITMISS
			type = 0;
		}
	}

	morphology->current_choice = type;
	//每次切换一个图片的时候之前的效果清空
	onTriggered_action_restore();

}

void Widget::onClicked_action_openFile()
{
	QString fileName = QFileDialog::getOpenFileName(nullptr, "选择文件", ".",	"图像文件(*.png *.jpg) 所有(*.*)");
	if (!fileName.isEmpty()) {
		qDebug() << "选择的文件名：" << fileName;
		mt = cv::imread(fileName.toLocal8Bit().data());
		//存图片
		QImage img = Mat2QImage(mt);
		QPixmap pic = QPixmap::fromImage(img);
		lab_img->setPixmap(pic);
	}
	else {
		qDebug() << "未选择任何文件，加载默认图片~~~";
		mt = cv::imread("../resource/dog.png");
	}
}

void Widget::onTriggered_action_saveFile()
{
	//保存加工后的图片
	QString FileName = QFileDialog::getSaveFileName(nullptr, "save image", ".", "Images(*.png *.bmp *.jpg)");
	if (!FileName.isEmpty()) {
		lab_img->pixmap().save(FileName);
	}
}

void Widget::onTriggered_action_restore()
{
	//恢复加载的原图
	lab_img->setPixmap(QPixmap::fromImage(ori_img));
}

void Widget::onTriggered_action_begin(){
	//点击此开始绘制模式
	diy = true;
}


void Widget::clearCurrentPageSliderValue()
{
	/*
	获取所有的指定的QSlider 控件并且清除值
	*/
	int index = stack_tools->currentIndex();
	QWidget* currentPage = stack_tools->widget(index);
	QList<QSlider*> ls = currentPage->findChildren<QSlider*>();

	for (auto& x : ls) {
		x->setValue(x->minimum());
	}
}

void Widget::createAction()
{
	//关闭窗口
	action_exit = new QAction(tr("退出"), this);
	action_exit->setShortcut(tr("Ctrl+C"));
	connect(action_exit, &QAction::triggered, this, [=]() {
		this->close();
	});

	//打开文件
	action_open = new QAction(tr("打开文件"), this);
	action_open->setShortcut(tr("Ctrl+O"));
	connect(action_open, &QAction::triggered, this,&Widget::onClicked_action_openFile);

	//保存图片
	action_save = new QAction(tr("保存图片"), this);
	action_save->setShortcut(tr("Ctrl+S"));
	connect(action_save, &QAction::triggered, this, &Widget::onTriggered_action_saveFile);

	//重置图片
	action_restore = new QAction(tr("回到原图"), this);
	action_restore->setIcon(QIcon("../resource/restore.png"));
	action_restore->setShortcut(tr("Ctrl+Z"));
	connect(action_restore, &QAction::triggered, this, &Widget::onTriggered_action_restore);

	//开始制作模式
	action_begin = new QAction(tr("开始制作图片"),this);
	action_begin->setIcon(QIcon("../resource/begin.png"));
	action_restore->setShortcut(tr("Ctrl+"));
	connect(action_restore, &QAction::triggered, this, &Widget::onTriggered_action_begin);
}

void Widget::createMenu()
{
	//图片上下文菜单
	context_menu = new QMenu(lab_img);
	context_menu->addAction(action_save);
	context_menu->addAction(action_exit);
	context_menu->addAction(action_restore);

	//菜单栏
	menu_file = menuBar()->addMenu(tr("&文件"));
	menu_file->addAction(action_open);
	menu_file->addAction(action_save);
	menu_file->addSeparator();
	menu_file->addAction(action_exit);

	menu_edit = menuBar()->addMenu(tr("&编辑"));
	menu_edit->addAction(action_restore);
	
}

void Widget::createToolBar()
{
	toolbar1 = addToolBar(tr("file"));
	toolbar1->addAction(action_restore);
	toolbar1->addAction(action_begin);
}


void Widget::createToolBox()
{
	//-----------------模糊操作-----------------------------
	btngroup_blur = new QButtonGroup(this);
	btngroup_blur->setExclusive(true);
	//连接信号
	connect(btngroup_blur, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_blur);

	
	QGridLayout* grid_blur = new QGridLayout;
	grid_blur->addWidget(createToolBtnItemWidget("均值模糊", BLUR::Average),0,0);
	grid_blur->addWidget(createToolBtnItemWidget("高斯模糊", BLUR::Gaussian),0,1);
	grid_blur->addWidget(createToolBtnItemWidget("中值模糊", BLUR::Median),1,0);
	grid_blur->addWidget(createToolBtnItemWidget("双边滤波", BLUR::Bilateral),1,1);

	grid_blur->setRowStretch(3, 10);
	grid_blur->setColumnStretch(0, 5);
	grid_blur->setColumnStretch(2, 5);

	QWidget* widget_blur = new QWidget;
	widget_blur->setLayout(grid_blur);

	//-----------------阈值化操作-----------------------------
	btngroup_threshold = new QButtonGroup(this);
	btngroup_threshold->setExclusive(true);
	//连接信号
	connect(btngroup_threshold, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_threshold);

	QGridLayout* grid_threshold = new QGridLayout;
	grid_threshold->addWidget(createToolBtnItemWidget("二进制化", THRESHOLD::Binary), 0, 0);
	grid_threshold->addWidget(createToolBtnItemWidget("反二进制为零", THRESHOLD::Binary_inv), 0, 1);
	grid_threshold->addWidget(createToolBtnItemWidget("截断", THRESHOLD::Trunc), 1, 0);
	grid_threshold->addWidget(createToolBtnItemWidget("阈值化为零", THRESHOLD::Tozero), 1, 1);
	grid_threshold->addWidget(createToolBtnItemWidget("反阈值化为零", THRESHOLD::Tozero_inv), 2, 0);

	grid_threshold->setRowStretch(4, 10);
	grid_threshold->setColumnStretch(2, 10);

	QWidget* widget_threshold = new QWidget;
	widget_threshold->setLayout(grid_threshold);


	//-----------------形态化操作-----------------------------
	btngroup_form = new QButtonGroup(this);
	btngroup_form->setExclusive(true);
	//连接信号
	connect(btngroup_form, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_form);

	QGridLayout* grid_form = new QGridLayout;
	grid_form->addWidget(createToolBtnItemWidget("膨胀", FORM::Erode), 0, 0);
	grid_form->addWidget(createToolBtnItemWidget("腐蚀", FORM::Dilate), 0, 1);
	grid_form->addWidget(createToolBtnItemWidget("开运算", FORM::Open), 1, 0);
	grid_form->addWidget(createToolBtnItemWidget("闭运算", FORM::Close), 1, 1);
	grid_form->addWidget(createToolBtnItemWidget("梯度", FORM::Gradient), 2, 0);
	grid_form->addWidget(createToolBtnItemWidget("顶帽", FORM::Tophat), 2, 1);
	grid_form->addWidget(createToolBtnItemWidget("黑帽", FORM::Blackhat), 3, 0);
	grid_form->addWidget(createToolBtnItemWidget("随机", FORM::Hitmiss), 3, 1);

	grid_form->setRowStretch(4, 10);
	grid_form->setColumnStretch(1, 10);

	QWidget* widget_from = new QWidget;
	widget_from->setLayout(grid_form);


	//创建ToolBox
	toolbox_side = new QToolBox(this);
	toolbox_side->setMinimumWidth(200);
	toolbox_side->setMaximumWidth(200);
	toolbox_side->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	toolbox_side->addItem(widget_blur, "图像模糊操作");
	toolbox_side->addItem(widget_threshold, "图像阈值操作");
	toolbox_side->addItem(widget_from, "图像形态化操作");
}

QWidget* Widget::createToolBtnItemWidget(const QString& text, int id)
{
	QToolButton* btn = new QToolButton;
	//btn->setIcon()
	btn->setStyleSheet("background-color: blue");
	btn->resize(48, 48);
	btn->setCheckable(false);

	if (belongsToEnum<BLUR>(id)) {
		btngroup_blur->addButton(btn, id); //绑定模糊与id
	}
	else if (belongsToEnum<THRESHOLD>(id)) {
		btngroup_threshold->addButton(btn, id); //绑定阈值与id
	}
	else if (belongsToEnum<FORM>(id)) {
		btngroup_form->addButton(btn, id); //绑定形态学与id
	}
	

	QGridLayout* grid = new QGridLayout;
	grid->addWidget(btn, 0, 0, Qt::AlignHCenter);
	grid->addWidget(new QLabel(text), 1, 0, Qt::AlignHCenter);

	QWidget* wid = new QWidget;
	wid->setLayout(grid);

	return wid;
}


//-----------------均值GUI----------------------------
QWidget* Widget::create_GUIAvgBlur() //1：均值 2：高斯
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(3, 40);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(1, 40);
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(1, 40);
	slider3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("kernel"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, blur, [=](int value) {
		if (slider2->value() == slider2->minimum() && slider3->value() == slider3->minimum()) {
			blur->anchorX = blur->anchorY = blur->avg_Ksize / 2;
		}
		blur->onTriggered_slider1_valueChange_avgBlur(value);
		lab_img->setPixmap(QPixmap::fromImage(blur->_img));
	});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_avgBlur_s1 = new QWidget;
	adj_avgBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("sigmaX"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::valueChanged, blur, [=](int value) {
		blur->onTriggered_slider2_valueChange_avgBlur(value);
		lab_img->setPixmap(QPixmap::fromImage(blur->_img));
		});

	QWidget* adj_avgBlur_s2 = new QWidget;
	adj_avgBlur_s2->setLayout(hlayout2);

	//--------------------------------

	QHBoxLayout* hlayout3 = new QHBoxLayout;
	hlayout3->addWidget(new QLabel("sigmaY"));
	hlayout3->addWidget(slider3);
	//Y偏移连接信号
	connect(slider3, &QSlider::valueChanged, blur, [=](int value) {
		blur->onTriggered_slider3_valueChange_avgBlur(value);
		lab_img->setPixmap(QPixmap::fromImage(blur->_img));
		});

	QWidget* adj_avgBlur_s3 = new QWidget;
	adj_avgBlur_s3->setLayout(hlayout3);

	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_avgBlur_s1);
	vLayout->addWidget(adj_avgBlur_s2);
	vLayout->addWidget(adj_avgBlur_s3);

	//整体最后再组成一个QWidget
	QWidget* adj_avgBlur = new QWidget;
	adj_avgBlur->setLayout(vLayout);

	return adj_avgBlur;
}

//-----------------高斯GUI----------------------------
QWidget* Widget::create_GUIGaussianBlur()
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(3, 41);
	slider1->setSingleStep(2);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(1, 40);
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(1, 40);
	slider3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("ksize"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, blur, [=](int value) {

		if (value % 2 == 0) {
			value += 1;
		}
		if (slider2->value() == slider2->minimum() && slider3->value() == slider3->minimum()) {
			blur->sigmaX = blur->sigmaY = blur->gas_Ksize / 2;
		}
	blur->onTriggered_slider1_valueChange_gaussianBlur(value);
	lab_img->setPixmap(QPixmap::fromImage(blur->_img));
		});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_gasBlur_s1 = new QWidget;
	adj_gasBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("sigmaX"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::valueChanged, blur, [=](int value) {
		blur->onTriggered_slider2_valueChange_gaussianBlur(value);
		lab_img->setPixmap(QPixmap::fromImage(blur->_img));
		});

	QWidget* adj_gasBlur_s2 = new QWidget;
	adj_gasBlur_s2->setLayout(hlayout2);

	//--------------------------------

	QHBoxLayout* hlayout3 = new QHBoxLayout;
	hlayout3->addWidget(new QLabel("sigmaY"));
	hlayout3->addWidget(slider3);
	//Y偏移连接信号
	connect(slider3, &QSlider::valueChanged, blur, [=](int value) {
		blur->onTriggered_slider3_valueChange_gaussianBlur(value);
	lab_img->setPixmap(QPixmap::fromImage(blur->_img));
		});

	QWidget* adj_gasBlur_s3 = new QWidget;
	adj_gasBlur_s3->setLayout(hlayout3);

	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_gasBlur_s1);
	vLayout->addWidget(adj_gasBlur_s2);
	vLayout->addWidget(adj_gasBlur_s3);

	//整体最后再组成一个QWidget
	QWidget* adj_gasBlur = new QWidget;
	adj_gasBlur->setLayout(vLayout);

	return adj_gasBlur;
}



QWidget* Widget::create_GUIMedianBlur()
{
	QSlider* slider = new QSlider(Qt::Horizontal);
	slider->setRange(3, 41);
	slider->setSingleStep(2);

	connect(slider, &QSlider::valueChanged, blur, [=](int value) {
		if (value % 2 == 0) {
			value += 1;
		}
		blur->onTriggered_slider_valueChange_medianBlur(value);
		lab_img->setPixmap(QPixmap::fromImage(blur->_img));
	});


	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(new QLabel("ksize"));
	hLayout->addWidget(slider);

	QWidget* adj_meanBlur = new QWidget;
	adj_meanBlur->setLayout(hLayout);

	return adj_meanBlur;
}


QWidget* Widget::create_GUIBilateralBlur()
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(1,100);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(0,255);
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(0,300);
	slider3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("ksize"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, blur, [=](int value) {
		blur->onTriggered_slider1_valueChange_bilateralBlur(value);
		lab_img->setPixmap(QPixmap::fromImage(blur->_img));
	});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_binBlur_s1 = new QWidget;
	adj_binBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("sigmaX"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::valueChanged, blur, [=](int value) {
		blur->onTriggered_slider2_valueChange_bilateralBlur(value);
		lab_img->setPixmap(QPixmap::fromImage(blur->_img));
		});

	QWidget* adj_binBlur_s2 = new QWidget;
	adj_binBlur_s2->setLayout(hlayout2);

	//--------------------------------

	QHBoxLayout* hlayout3 = new QHBoxLayout;
	hlayout3->addWidget(new QLabel("sigmaY"));
	hlayout3->addWidget(slider3);
	//Y偏移连接信号
	connect(slider3, &QSlider::valueChanged, blur, [=](int value) {
		blur->onTriggered_slider3_valueChange_bilateralBlur(value);
		lab_img->setPixmap(QPixmap::fromImage(blur->_img));
		});

	QWidget* adj_binBlur_s3 = new QWidget;
	adj_binBlur_s3->setLayout(hlayout3);

	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_binBlur_s1);
	vLayout->addWidget(adj_binBlur_s2);
	vLayout->addWidget(adj_binBlur_s3);

	//整体最后再组成一个QWidget
	QWidget* adj_bilateralBlur = new QWidget;
	adj_bilateralBlur->setLayout(vLayout);


	return adj_bilateralBlur;
}

QWidget* Widget::create_GUIThreshoild()
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(0, 255);
	slider1->setValue(128);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(0, 255);
	slider2->setValue(255);
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("threshold"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, threshold, [=](int value) {
		threshold->onTriggered_slider1_valueChanged_thresholdValue(value);
		lab_img->setPixmap(QPixmap::fromImage(threshold->_img));
		});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_binBlur_s1 = new QWidget;
	adj_binBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("maxval"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::valueChanged, threshold, [=](int value) {
		threshold->onTriggered_slider2_valueChanged_maxValue(value);
		lab_img->setPixmap(QPixmap::fromImage(threshold->_img));		
		});

	QWidget* adj_binBlur_s2 = new QWidget;
	adj_binBlur_s2->setLayout(hlayout2);

	//--------------------------------


	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_binBlur_s1);
	vLayout->addWidget(adj_binBlur_s2);

	//整体最后再组成一个QWidget
	QWidget* adj_threshold = new QWidget;
	adj_threshold->setLayout(vLayout);

	return adj_threshold;
}

QWidget* Widget::create_GUIMorphology()
{
	//Kernel
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(3, 40);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//anthorX
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(1,40);
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//anthorY
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(1, 40);
	slider3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("Kernel"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, morphology, [=](int value) {
		morphology->onTriggered_slider1_valueChanged_kernel(value);
		lab_img->setPixmap(QPixmap::fromImage(morphology->_img));
		});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_binBlur_s1 = new QWidget;
	adj_binBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("anchorX"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::valueChanged, morphology, [=](int value) {
		morphology->onTriggered_slider2_valueChanged_anchorX(value);
		lab_img->setPixmap(QPixmap::fromImage(morphology->_img));
		});

	QWidget* adj_binBlur_s2 = new QWidget;
	adj_binBlur_s2->setLayout(hlayout2);

	//--------------------------------

	QHBoxLayout* hlayout3 = new QHBoxLayout;
	hlayout3->addWidget(new QLabel("anchorY"));
	hlayout3->addWidget(slider3);
	//Y偏移连接信号
	connect(slider3, &QSlider::valueChanged, morphology, [=](int value) {
		morphology->onTriggered_slider3_valueChanged_anchorY(value);
		lab_img->setPixmap(QPixmap::fromImage(morphology->_img));
		});

	QWidget* adj_binBlur_s3 = new QWidget;
	adj_binBlur_s3->setLayout(hlayout3);

	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_binBlur_s1);
	vLayout->addWidget(adj_binBlur_s2);
	vLayout->addWidget(adj_binBlur_s3);

	//整体最后再组成一个QWidget
	QWidget* adj_morphology = new QWidget;
	adj_morphology->setLayout(vLayout);

	return adj_morphology;
}