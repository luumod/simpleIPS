#include "../include/widget.h"
#include "../include/belongsToEnum.h"
#include "../include/Mat2QImage.h"
#include "../include/Blur.h"
#include "../include/Threshold.h"
#include "../include/Form.h"
#include "../include/Connected.h"
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
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>


Widget::Widget(QWidget* parent)
	:QMainWindow(parent),
	ori_mt(cv::imread("../resource/dog.png")),
	ori_img(Mat2QImage(ori_mt))
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
	resize(880,780);
	this->move(200,50);

	ori_mt.copyTo(temp_mt);

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

	stack_tools->addWidget(create_GUIConnected()); //6
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
	delete blur;
	blur = nullptr;
	
	delete threshold;
	threshold = nullptr;
	
	delete morphology;
	morphology = nullptr;

	delete connected;
	connected = nullptr;
}

void Widget::initFunction()
{
	//初始化
	blur = new Blur(ori_mt); 
	threshold = new Threshold(ori_mt);
	morphology = new Form(ori_mt);
	connected = new Connected(ori_mt);
}

void Widget::onClicked_buttonGroup_blur(QAbstractButton* btn) 
{
	blur->savePoint();

	if (mode) {

	}
	else {
		onTriggered_action_blur_restore();
	}

	stack_tools->setVisible(true);
	int id = btngroup_blur->id(btn); //获取按下的按钮的id

	now = id;  //获取当前位置

	stack_tools->setCurrentIndex(id - BLUR::Average); //0-0 1-0 2-0 3-0

	QList<QAbstractButton*> btns = btngroup_blur->buttons();
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
}

void Widget::onClicked_buttonGroup_threshold(QAbstractButton* btn)
{
	threshold->savePoint();

	int id = btngroup_threshold->id(btn); //获取按下的按钮的id
	now = id;  //获取当前位置

	int type = cv::ThresholdTypes(id - THRESHOLD::Binary);
	//选择当前阈值模式
	threshold->current_choice = type;


	onTriggered_action_threshold_restore();

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
}

void Widget::onClicked_buttonGroup_form(QAbstractButton* btn)
{
	morphology->savePoint();

	int id = btngroup_form->id(btn);
	now = id;  //获取当前位置

	if (mode) {

	}
	else {
		onTriggered_action_morphology_restore();
	}
	int type = cv::MorphTypes(id - FORM::Erode);
	if (type == cv::MorphTypes::MORPH_HITMISS) {
		if (morphology->_mt.type() != CV_8UC1) {
			//只有CV_8UC1可以使用MORPH_HITMISS
			type = 0;
		}
	}
	morphology->current_choice = type;

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
			setIndexPageSliderValue();
			m_btn->setStyleSheet("background-color:blue");
		}
	}
}

void Widget::onClicked_buttonGroup_connected(QAbstractButton* btn)
{
	int id = btngroup_connected->id(btn);
	connected->current_choice = id - CONNECTED::TYPE1; // 0 1

	if (mode) {

	}
	else {
		onTriggered_action_connected_restore();
	}

	stack_tools->setVisible(true);
	stack_tools->setCurrentIndex(6);
	QList<QAbstractButton*> btns = btngroup_connected->buttons();
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
}

void Widget::onClicked_action_openFile()
{
	QString fileName = QFileDialog::getOpenFileName(nullptr, "选择文件", ".",	"图像文件(*.png *.jpg)");
	if (!fileName.isEmpty()) {
		qDebug() << "选择的文件名：" << fileName;
		ori_mt = cv::imread(fileName.toLocal8Bit().data());
		ori_mt.copyTo(temp_mt);
		//存图片
		QImage img = Mat2QImage(ori_mt);
		ori_img = img.copy();
		lab_img->setPixmap(QPixmap::fromImage(img));


		//更新
		blur->update(ori_mt);
		threshold->update(ori_mt);
		morphology->update(ori_mt);
		connected->update(ori_mt);
		clearAllSliderValue();
	}
	else {
		QMessageBox::warning(nullptr, tr("警告"), "图片文件打开失败!");
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

void Widget::onTriggered_action_allRestore()
{
	//清除所有的数据信息
	blur->restore();
	threshold->restore();
	morphology->restore();
	connected->restore();

	clearAllSliderValue();

	lab_img->setPixmap(QPixmap::fromImage(ori_img));
}

void Widget::onTriggered_action_blur_restore()
{
	/*
	在模糊操作选择的时候，如果更换了新的选择，则需要清除所有的修改，重新载入一张原图
	*/
	blur->restore();	
	for (int i = 0; i < 4; i++) {
		setIndexPageSliderValue(i); //滑块置零
	}
	lab_img->setPixmap(QPixmap::fromImage(ori_img));
}

void Widget::onTriggered_action_threshold_restore() {
	/*
	在阈值操作选择的时候，如果更换了新的选择，则需要清除所有的修改，重新载入一张原图
	*/
	threshold->restore();
	setIndexPageSliderValue(4);
	lab_img->setPixmap(QPixmap::fromImage(ori_img));
}

void Widget::onTriggered_action_morphology_restore()
{
	morphology->restore();
	setIndexPageSliderValue(5);
	lab_img->setPixmap(QPixmap::fromImage(ori_img));
}

void Widget::onTriggered_action_connected_restore()
{
	/*
	当切换不同的操作的时候，恢复原图重新操作
	*/
	connected->restore();
	setIndexPageSliderValue(6);
	lab_img->setPixmap(QPixmap::fromImage(ori_img));
}

void Widget::onTriggered_action_process(){
	//点击此开始创作模式
	//图片清除，重新开始，所有滑块归零
	clearAllSliderValue();

	mode = true;
	blur->mode = true;
	threshold->mode = true;
	morphology->mode = true;
	lab_img->setPixmap(QPixmap::fromImage(ori_img));

	if (mode) {
		toolbox_side->setCurrentIndex(0);
		stack_tools->setCurrentIndex(0);

		//阈值化操作对图片的影响过大，在混合加工模式下禁止使用
		toolbox_side->setItemEnabled(1, false);
		//连通性分析也无法使用
		toolbox_side->setItemEnabled(3, false);
	}
}

void Widget::onTriggered_action_undo()
{
	//仅仅在创作者模式下生效：触发时会将图片置回到未操作前的位置

	if (mode) {
		if (belongsToEnum<BLUR>(now)) {
			blur->returnPoint();
			lab_img->setPixmap(QPixmap::fromImage(Mat2QImage(blur->_mt)));
		}
		else if (belongsToEnum<FORM>(now)) {
			morphology->returnPoint();
			lab_img->setPixmap(QPixmap::fromImage(Mat2QImage(morphology->_mt)));
		}
		//清除滑块的值
		setIndexPageSliderValue();
	}
	else {
		onTriggered_action_allRestore();
	}
	
}

void Widget::setIndexPageSliderValue(int index)
{
	/*
	获取所有的指定的QSlider 控件并且清除值
	*/
	//传递默认的index，则选择当前页
	if (index == -1) {
		index = stack_tools->currentIndex();
	}
	//否则就是第index页
	QWidget* currentPage = stack_tools->widget(index);
	QList<QWidget*> ls = currentPage->findChildren<QWidget*>();

	for (auto& x : ls) {
		if (QSlider* slider = qobject_cast<QSlider*>(x)) {
			if (slider->objectName() == tr("threshold_value")) {
				slider->setValue(128);
			}
			else if (x->objectName() == tr("maxValue")) {
				slider->setValue(255);
			}
			else {
				slider->setValue(slider->minimum());
			}
		}
		else if (QComboBox* comb = qobject_cast<QComboBox*>(x)) {
				comb->setCurrentIndex(0);
		}
		
	}
}

void Widget::clearAllSliderValue()
{
	//清除所有的滑块的值
	for (int i = 0; i < stack_tools->count(); i++) {
		setIndexPageSliderValue(i);
	}
}


void Widget::createAction()
{
	//关闭窗口
	action_exit = new QAction(tr("退出"), this);
	action_exit->setStatusTip(tr("退出程序"));
	action_exit->setShortcut(tr("Ctrl+E"));
	connect(action_exit, &QAction::triggered, this, [=]() {
		this->close();
	});

	//打开文件
	action_open = new QAction(tr("打开文件"), this);
	action_open->setStatusTip(tr("选择并且打开一个图片"));
	action_open->setShortcut(tr("Ctrl+O"));
	connect(action_open, &QAction::triggered, this,&Widget::onClicked_action_openFile);

	//保存图片
	action_save = new QAction(tr("保存图片"), this);
	action_save->setStatusTip(tr("保存此图片"));
	action_save->setShortcut(tr("Ctrl+S"));
	connect(action_save, &QAction::triggered, this, &Widget::onTriggered_action_saveFile);

	//重置图片
	action_restore = new QAction(tr("重置图片"), this);
	action_restore->setStatusTip(tr("重置此图片，取消所有加工"));
	action_restore->setIcon(QIcon("../resource/restore.png"));
	action_restore->setShortcut(tr("Ctrl+Z"));
	connect(action_restore, &QAction::triggered, this, &Widget::onTriggered_action_allRestore);

	//开始制作模式
	action_begin = new QAction(tr("图片加工"),this);
	action_begin->setStatusTip(tr("图片加工模式，可以混合修改图片"));
	action_begin->setIcon(QIcon("../resource/begin.png"));
	connect(action_begin, &QAction::triggered, this, &Widget::onTriggered_action_process);

	//撤销
	action_return = new QAction(tr("撤销"), this);
	action_return->setStatusTip(tr("撤销此操作"));
	action_return->setIcon(QIcon("../resource/return.png"));
	connect(action_return, &QAction::triggered, this, &Widget::onTriggered_action_undo);
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
	toolbar1->addAction(action_begin);
	toolbar1->addAction(action_restore);
	toolbar1->addAction(action_return);
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


	//-----------------连通区域分析操作-----------------------------
	btngroup_connected = new QButtonGroup(this);
	btngroup_connected->setExclusive(true);//互斥

	connect(btngroup_connected, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_connected);
	
	QGridLayout* gird_connected = new QGridLayout;
	gird_connected->addWidget(createToolBtnItemWidget(tr("连通区域分析①"), CONNECTED::TYPE1),0,0);
	gird_connected->addWidget(createToolBtnItemWidget(tr("连通区域分析②"), CONNECTED::TYPE2),1,0);
	gird_connected->setRowStretch(4, 10);
	gird_connected->setColumnStretch(1, 10);

	QWidget* widget_connected = new QWidget;
	widget_connected->setLayout(gird_connected);



	//创建ToolBox
	toolbox_side = new QToolBox(this);
	toolbox_side->setMinimumWidth(200);
	toolbox_side->setMaximumWidth(200);
	toolbox_side->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	toolbox_side->addItem(widget_blur, "图像模糊操作");
	toolbox_side->addItem(widget_threshold, "图像阈值操作");
	toolbox_side->addItem(widget_from, "图像形态化操作");
	toolbox_side->addItem(widget_connected, "图像连通分析");
	toolbox_side->setCurrentIndex(0); 


	////获取切换后的toolbox索引
	connect(toolbox_side, &QToolBox::currentChanged, this, [=](int value) {
		preToolBoxIndex = curToolBoxIndex;
		curToolBoxIndex = value;
		
		//保存离开前一个窗口时的 cv::mat
		if (preToolBoxIndex == 0) {
			blur->_mt.copyTo(temp_mt);
		}
		else if (preToolBoxIndex == 1) {
			threshold->_mt.copyTo(temp_mt);
		}
		else if (preToolBoxIndex == 2) {
			morphology->_mt.copyTo(temp_mt);
		}
		else if (preToolBoxIndex == 3) {
			connected->_mt.copyTo(temp_mt);
		}

		if (mode) {
			if (value == 0) {
				//blur
				temp_mt.copyTo(blur->_mt);
			}
			//else if (value == 1) {
			//	temp_mt.copyTo(threshold->_mt);
			//}
			else if (value == 2) {
				temp_mt.copyTo(morphology->_mt);
			}
			//else if (value == 3) {
			//	temp_mt.copyTo(connected->_mt);
			//}
		}
	});
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
	else if (belongsToEnum<CONNECTED>(id)) {
		btngroup_connected->addButton(btn, id); //连通性分析
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
	slider1->setRange(1, 50);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(1, 50);
	slider2->setSizePolicy(QSizePolicy::Expanding,  QSizePolicy::Fixed);
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(1, 50);
	slider3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("kernel"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, blur, [=](int value) {
		if (slider2->value() == slider2->minimum() && slider3->value() == slider3->minimum()) {
			blur->anchorX = blur->anchorY = blur->avg_Ksize / 2;
		}
	//qInfo() << value;
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
	connect(slider2, &QSlider::sliderMoved, blur, [=](int value) {
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
	connect(slider3, &QSlider::sliderMoved, blur, [=](int value) {
		blur->onTriggered_slider3_valueChange_avgBlur(value);
		lab_img->setPixmap(QPixmap::fromImage(blur->_img));
		});

	QWidget* adj_avgBlur_s3 = new QWidget;
	adj_avgBlur_s3->setLayout(hlayout3);

	//QPushButton* btn1 = new QPushButton("撤销");
	//btn1->addAction(action_return);
	//connect(btn1, &QPushButton::clicked, action_return, &QAction::triggered);
	

	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_avgBlur_s1);
	vLayout->addWidget(adj_avgBlur_s2);
	vLayout->addWidget(adj_avgBlur_s3);
	//vLayout->addWidget(btn1);

	//整体最后再组成一个QWidget
	QWidget* adj_avgBlur = new QWidget;
	adj_avgBlur->setLayout(vLayout);

	return adj_avgBlur;
}

//-----------------高斯GUI----------------------------
QWidget* Widget::create_GUIGaussianBlur()
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(1, 41);
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
	connect(slider2, &QSlider::sliderMoved, blur, [=](int value) {
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
	connect(slider3, &QSlider::sliderMoved, blur, [=](int value) {
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


//----------------中值GUI----------------------------
QWidget* Widget::create_GUIMedianBlur()
{
	QSlider* slider = new QSlider(Qt::Horizontal);
	slider->setRange(1, 41);
	slider->setSingleStep(2);

	connect(slider, &QSlider::sliderMoved, blur, [=](int value) {
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



//----------------双边GUI----------------------------
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
	connect(slider2, &QSlider::sliderMoved, blur, [=](int value) {
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
	connect(slider3, &QSlider::sliderMoved, blur, [=](int value) {
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

//----------------阈值GUI----------------------------
QWidget* Widget::create_GUIThreshoild()
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(0, 255);
	slider1->setValue(128);
	slider1->setObjectName("threshold_value");
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(0, 255);
	slider2->setValue(255);
	slider2->setObjectName("maxValue");
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
	connect(slider2, &QSlider::sliderMoved, threshold, [=](int value) {
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

//----------------形态学GUI----------------------------
QWidget* Widget::create_GUIMorphology()
{
	//Kernel
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(1, 100);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//anthorX
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(1,  50);
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//anthorY
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(1,  50);
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
	connect(slider2, &QSlider::sliderMoved, morphology, [=](int value) {
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
	connect(slider3, &QSlider::sliderMoved, morphology, [=](int value) {
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


//----------------连通区域分析GUI----------------------------
QWidget* Widget::create_GUIConnected()
{
	QComboBox* comb1 = new QComboBox; //选择 connectivity = 8 or 4
	comb1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	comb1->setObjectName(tr("connectivity"));
	comb1->addItem(tr("8"));
	comb1->addItem(tr("4"));
	comb1->setEditable(false);
	connect(comb1, &QComboBox::currentTextChanged, this, [=](const QString& text) {
		connected->onTriggered_Comb1_currentTextChanged_connectivtiy(text);
		lab_img->setPixmap(QPixmap::fromImage(connected->_img));
		
	});
	QComboBox* comb2 = new QComboBox;  //选择算法
	comb2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	comb2->setObjectName(tr("ccltype"));
	comb2->addItem(tr("default"));
	comb2->addItem(tr("CC_WU"));
	comb2->addItem(tr("CCL_GRANA"));
	comb2->addItem(tr("CCL_BOLELLI"));
	comb2->addItem(tr("CCL_SAUF"));
	comb2->addItem(tr("CCL_BBDT"));
	comb2->addItem(tr("CCL_SPAGHETTI"));
	comb2->setEditable(false);
	connect(comb2, &QComboBox::currentTextChanged, this, [=](const QString& text) {
		connected->onTriggered_Comb2_currentTextChanged_ccltype(text);
		lab_img->setPixmap(QPixmap::fromImage(connected->_img));
	});

	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("邻域"));
	hlayout1->addWidget(comb1);
	hlayout1->setAlignment(Qt::AlignmentFlag::AlignCenter);

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("联通算法"));
	hlayout2->addWidget(comb2);
	hlayout2->setAlignment(Qt::AlignmentFlag::AlignCenter);

	QWidget* w1 = new QWidget;
	w1->setLayout(hlayout1);

	QWidget* w2 = new QWidget;
	w2->setLayout(hlayout2);

	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->addWidget(w1);
	vlayout->addWidget(w2);

	QWidget* adj_connected = new QWidget;
	adj_connected->setLayout(vlayout);

	return adj_connected;
}