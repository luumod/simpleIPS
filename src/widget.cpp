#include "../include/widget.h"
#include "../include/Common.h"
#include "../include/assist/belongsToEnum.h"
#include "../include/assist/Mat2QImage.h"

#include "../include/opencv_functions/Blur.h"
#include "../include/opencv_functions/Threshold.h"
#include "../include/opencv_functions/Morphology.h"
#include "../include/opencv_functions/Connected.h"
#include "../include/opencv_functions/Contours.h"
#include "../include/opencv_functions/BaseOperate.h"
#include "../include/opencv_functions/Showeffect.h"

#include "../include/other_functions/LabelImg.h"
#include "../include/other_functions/DrawWidget.h"
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
#include <QPalette>
#include <QColorDialog>
#include <QFile>
#include <QStatusBar>
#include <QActionGroup>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QDialog>
#include <QMouseEvent>

Widget* Widget::widget = nullptr;

Widget* Widget::getInstance() {
	if (widget == nullptr) {
		widget = new Widget;
	}
	return widget;
}

Widget::Widget(QMainWindow* parent)
	:QMainWindow(parent),
	root_mt(cv::imread("../resource/testImages/122.png"))
{
	root_mt.copyTo(inter_mt);
	root_mt.copyTo(curr_mt);
	curr_img = Mat2QImage(curr_mt);
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
	//窗口固定大小
	this->setFixedSize(910, 780);
	this->move(200,20);

	lab_img = new Main_Label;
	lab_img->setObjectName("main_ab");

	initFunction();
	createAction();
	createMenu();
	createToolBar();
	createToolBox();
	createStatusBar();

	//处理如何设置上下文菜单CustomContextMenu：发出customContextMenuRequested信号
	lab_img->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(lab_img, &QLabel::customContextMenuRequested, this, [=](const QPoint& pos) {
		context_menu->exec(QCursor::pos());
		});
	
	auto main_tpic = QPixmap::fromImage(curr_img);
	auto main_pic = main_tpic;
	//main_tpic.scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	lab_img->setPixmap(main_pic);	
	lab_img->setScaledContents(true);
	lab_img->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	sub_lab_img = new Sub_Label(lab_img); //子图片设置依附在父图片上
	sub_lab_img->setPixmap(lab_img->pixmap().scaled(200, 200));
	sub_lab_img->move(0,0);
	sub_lab_img->raise();  //提升：实现依附的关键
	sub_lab_img->setVisible(false); //隐藏

	//右侧布局添加图片与调整栏
	vlayout_right = new QVBoxLayout;
	vlayout_right->addWidget(lab_img);

	//很多的调整框 
	ls_dlg_avg.push_back(new QDialog);
	ls_dlg_avg[0]->setLayout(create_GUIAvgBlur());
	all_dlg.push_back(ls_dlg_avg[0]);

	ls_dlg_avg.push_back(new QDialog);
	ls_dlg_avg[1]->setLayout(create_GUIGaussianBlur());
	all_dlg.push_back(ls_dlg_avg[1]);

	ls_dlg_avg.push_back(new QDialog);
	ls_dlg_avg[2]->setLayout(create_GUIMedianBlur());
	all_dlg.push_back(ls_dlg_avg[2]);

	ls_dlg_avg.push_back(new QDialog);
	ls_dlg_avg[3]->setLayout(create_GUIBilateralBlur());
	all_dlg.push_back(ls_dlg_avg[3]);

	dlg_threshold = new QDialog;
	dlg_threshold->setLayout(create_GUIThreshold());
	all_dlg.push_back(dlg_threshold);

	dlg_morphology = new QDialog;
	dlg_morphology->setLayout(create_GUIMorphology());
	all_dlg.push_back(dlg_morphology);

	dlg_connected = new QDialog;
	dlg_connected->setLayout(create_GUIConnected());
	all_dlg.push_back(dlg_connected);

	dlg_contours = new QDialog;
	dlg_contours->setLayout(create_GUIContours());
	all_dlg.push_back(dlg_contours);

	dlg_showeffect = new QDialog;
	dlg_showeffect->setLayout(create_GUIShow());
	all_dlg.push_back(dlg_showeffect);
	

	for (auto& x : all_dlg) {
		x->setGeometry(this->rect().x() + this->width(), this->rect().y() + 100, 200, 200);
		x->setWindowFlags(x->windowFlags() | Qt::WindowStaysOnTopHint);
	}

	//右侧主窗口
	QWidget* picture_show = new QWidget;
	picture_show->setLayout(vlayout_right);
	picture_show->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	//主程序布局
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(toolbox_side); //左侧主窗口
	layout->addWidget(picture_show); //右侧主窗口

	QWidget* mainWindow = new QWidget(this);
	mainWindow->setLayout(layout);

	this->setCentralWidget(mainWindow);

	//连接信号
	connect(this, &Widget::modeChanged, this, [=]() {
		});


	//读取QSS美化
	//外部加载
	QFile qssFile("../resource/qss/meihua.css");
	if (qssFile.open(QFile::OpenModeFlag::ReadOnly)) {
		this->setStyleSheet(qssFile.readAll());
	}
}

Widget::~Widget()
{
	//销毁所有dialog
	for (auto& x : all_dlg) {
		delete x;
		x = nullptr;
	}

	//内存释放
	for (auto& x : ls) {
		delete x;
		x = nullptr;
	}
	while (!undo_sta.empty()) {
		undo_sta.pop();
	}



}

void Widget::mousePressEvent(QMouseEvent* ev)
{
	hideAllDialog(nullptr);
}

void Widget::initFunction()
{
	//初始化
	ls.push_back(blur = new Blur);
	ls.push_back(threshold = new Threshold);
	ls.push_back(morphology = new Morphology);
	ls.push_back(connected = new Connected);
	ls.push_back(contours = new Contours);
	ls.push_back(showeffect = new Showeffect);
	ls.push_back(img_base = new BaseOperate);
}

void Widget::onClicked_buttonGroup_blur(QAbstractButton* btn) 
{
	int id = btngroup_blur->id(btn); //获取按下的按钮的id
	now_operation = id;  //一定首先获取当前位置
	now_dialog = 0;

	if (mode) {
		if (sub_lab_img->isVisible()) {
			//如果我操作了图片，但是没有将预览的效果应用到主图片上，则说明此次修改无效。
			//返回上一个保存点
			returnPoint();
			sub_lab_img->setVisible(false);
		}
		//先恢复再保存
		savePoint();
		clearAllWidgetValue();
	}
	else {
		restore_cutOperation();
	}
	

	hideAllDialog(ls_dlg_avg[id - BLUR::Average]);
	ls_dlg_avg[id - BLUR::Average]->open();//0-0 1-0 2-0 3-0

	QList<QAbstractButton*> btns = btngroup_blur->buttons();
	for (auto& m_btn : btns) {
		if (m_btn == btn) {
			m_btn->setChecked(true);
		}
	}
}

void Widget::onClicked_buttonGroup_threshold(QAbstractButton* btn)
{
	int id = btngroup_threshold->id(btn); //获取按下的按钮的id
	now_operation = id;  //获取当前位置
	now_dialog = 1;

	int type = cv::ThresholdTypes(id - THRESHOLD::Binary);

	//选择当前阈值模式
	threshold->current_choice = type;

	if (mode) {
		if (sub_lab_img->isVisible()) {
			returnPoint();
			sub_lab_img->setVisible(false);
		}
		savePoint();
		clearAllWidgetValue();
	}
	else {
		restore_cutOperation();
	}

	hideAllDialog(dlg_threshold);
	dlg_threshold->open();

	QList<QAbstractButton*> btns = btngroup_threshold->buttons();
	for (auto& m_btn : btns) {
		if (m_btn == btn) {
			m_btn->setChecked(true);
		}
	}
}

void Widget::onClicked_buttonGroup_morphology(QAbstractButton* btn)
{
	int id = btngroup_form->id(btn);
	now_operation = id;  //获取当前位置
	now_dialog = 2;
	

	if (mode) {
		if (sub_lab_img->isVisible()) {
			returnPoint();
			sub_lab_img->setVisible(false);
		}
		savePoint();
		clearAllWidgetValue();
	}
	else {
		restore_cutOperation();
	}

	int type = cv::MorphTypes(id - FORM::Erode);
	if (type == cv::MorphTypes::MORPH_HITMISS) {
		if (curr_mt.type() != CV_8UC1) {
			//只有CV_8UC1可以使用MORPH_HITMISS
			type = 0;
		}
	}
	//选择当前形态学操作模式
	morphology->current_choice = type;

	hideAllDialog(dlg_morphology);
	dlg_morphology->open();

	QList<QAbstractButton*> btns = btngroup_form->buttons();
	for (auto& m_btn : btns) {
		if (m_btn == btn) {
			m_btn->setChecked(true);
		}
	}
}

void Widget::onClicked_buttonGroup_connected(QAbstractButton* btn)
{
	int id = btngroup_connected->id(btn);
	now_operation = id;  //获取当前位置
	now_dialog = 3;
	
	//选择当前连通性分析操作模式
	connected->current_choice = id - CONNECTED::CONNECTED_TYPE1; // 0 1

	if (mode) {
		if (sub_lab_img->isVisible()) {
			returnPoint();
			sub_lab_img->setVisible(false);
		}
		savePoint();
		clearAllWidgetValue();
	}
	else {
		restore_cutOperation();
	}

	hideAllDialog(dlg_connected);
	dlg_connected->open();

	QList<QAbstractButton*> btns = btngroup_connected->buttons();
	for (auto& m_btn : btns) {
		if (m_btn == btn) {
			m_btn->setChecked(true);
		}
	}
}

void Widget::onClicked_buttonGroup_contours(QAbstractButton* btn)
{
	int id = btngroup_contours->id(btn);
	now_operation = id;  //获取当前位置
	now_dialog = 4;

	if (mode) {
		if (sub_lab_img->isVisible()) {
			returnPoint();
			sub_lab_img->setVisible(false);
		}
		savePoint();
		clearAllWidgetValue();
	}
	else {
		restore_cutOperation();
	}

	hideAllDialog(dlg_contours);
	dlg_contours->open();

	QList<QAbstractButton*> btns = btngroup_contours->buttons();
	for (auto& m_btn : btns) {
		if (m_btn == btn) {
			m_btn->setChecked(true);
		}
	}
}

void Widget::onClicked_buttonGroup_show(QAbstractButton* btn)
{
	int id = btngroup_show->id(btn);
	now_operation = id;  //获取当前位置
	now_dialog = 5;

	if (mode) {
		if (sub_lab_img->isVisible()) {
			returnPoint();
			sub_lab_img->setVisible(false);
		}
		savePoint();
		clearAllWidgetValue();
	}
	else {
		restore_cutOperation();
	}

	hideAllDialog(dlg_showeffect);
	dlg_showeffect->open();

	QList<QAbstractButton*> btns = btngroup_show->buttons();
	for (auto& m_btn : btns) {
		if (m_btn == btn) {
			m_btn->setChecked(true);
		}
	}
}

void Widget::onTriggered_action_openFile()
{
	QString fileName = QFileDialog::getOpenFileName(nullptr, "选择文件", ".",	"图像文件(*.png *.jpg)");
	if (!fileName.isEmpty()) {
		auto t_ori_mt = cv::imread(fileName.toLocal8Bit().data());
		root_mt = t_ori_mt;
		root_mt.copyTo(inter_mt);
		root_mt.copyTo(preview_mt);
		root_mt.copyTo(curr_mt);

		//存图片
		curr_img = Mat2QImage(curr_mt);
		lab_img->setPixmap(QPixmap::fromImage(curr_img));

		//更新数值
		for (auto& x : ls) {
			x->initialize();
		}
		clearAllWidgetValue();
		sub_lab_img->setVisible(false);
	}
	else {
		QMessageBox::warning(nullptr, tr("警告"), "图片文件打开失败!");
	}
}

void Widget::onTriggered_action_saveToFile()
{
	//保存加工后的图片
	QString FileName = QFileDialog::getSaveFileName(nullptr, "save image", ".", "Images(*.png *.bmp *.jpg)");
	if (!FileName.isEmpty()) {
		lab_img->pixmap().save(FileName);
	}
}

void Widget::onTriggered_action_allRestore()
{
	/*
	重置所有操作至原始图片
	*/
	//清除所有的数据信息
	for (auto& x : ls) {
		x->initialize();
	}
	sub_lab_img->setVisible(false);

	clearAllWidgetValue();

	updateFromRoot();
}

void Widget::onTriggered_action_previewToNormal()
{
	//预览图点击确定转换为正常图 lan_img
	if (mode) {
		//点击确定，预览图片消失
		sub_lab_img->setVisible(false);
		lab_img->setPixmap(QPixmap::fromImage(curr_img));
	}
}

void Widget::onTriggered_ColorDialog_choice(const QColor& color)
{
	//选择颜色
	contours->onTriggered_Color_currentTextChanged_contoursColor(color);
}

void Widget::restore_cutOperation()
{
	sub_lab_img->setVisible(false);
	clearAllWidgetValue();

	updateFromIntermediate();
}

void Widget::onTriggered_action_process(){
	//点击此开始创作模式
	//图片清除，重新开始，所有滑块归零
	if (!mode) {

		//开启预览
		sub_lab_img->setVisible(false);

		mode = true;

		statusLab->setText(tr("混合加工模式"));

		//状态改变
		emit modeChanged();
	}
	else {
		mode = false;
		statusLab->setText(tr("默认模式"));

		//状态改变
		emit modeChanged();
	}
	clearAllWidgetValue();
	//数据清空
	updateFromIntermediate();
}

void Widget::onTriggered_action_undo()
{
	//仅仅在创作者模式下生效：触发时会将图片置回到未操作前的位置

	if (mode) {
		returnPoint();
		sub_lab_img->setPixmap(QPixmap::fromImage(curr_img.scaled(200, 200)));
		lab_img->setPixmap(QPixmap::fromImage(curr_img));
		//清除滑块的值
		setIndexPageWidgetValue();
	}
	else {
		onTriggered_action_allRestore();
	}
}


void Widget::savePoint()
{
	//每次切换操作时自动保存当前图片作为保存点
	curr_mt.copyTo(preview_mt);
	undo_sta.push(curr_mt);
}

void Widget::returnPoint()
{
	//读取存档保存点
	//获取当前栈顶的mat
	if (!undo_sta.empty()) {
		//修改当前显示的mt与图片
		curr_mt = undo_sta.top();
		curr_img = Mat2QImage(curr_mt);
		undo_sta.pop();
	}
}

void Widget::hideAllDialog(QDialog* currDialog)
{
	for (auto& x : all_dlg) {
		if (x != currDialog) {
			x->accept();
		}
	}
}

void Widget::setIndexPageWidgetValue(int index)
{
	/*
	获取所有的指定的QSlider 控件并且清除值
	*/
	//传递默认的index，则选择当前页
	if (index == -1) {
		index = now_dialog; 
	}
	
	//否则就是第index页
	QList<QWidget*> ls = all_dlg[index]->findChildren<QWidget*>();

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

void Widget::clearAllWidgetValue()
{
	//清除所有的滑块的值
	for (int i = 0; i < all_dlg.count(); i++) { // (0 1 2 3) 4 5 6 7
		setIndexPageWidgetValue(i);
	}
}

void Widget::updateFromIntermediate()
{
	inter_mt.copyTo(curr_mt);	 //修改当前图片
	inter_mt.copyTo(preview_mt); //修改快照图片
	curr_img = Mat2QImage(curr_mt);
	lab_img->setPixmap(QPixmap::fromImage(curr_img));
}

void Widget::updateFromRoot()
{
	root_mt.copyTo(inter_mt); //重置inter_mt
	updateFromIntermediate();
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
	connect(action_open, &QAction::triggered, this,&Widget::onTriggered_action_openFile);

	//保存图片
	action_save = new QAction(tr("保存图片"), this);
	action_save->setStatusTip(tr("保存此图片"));
	action_save->setShortcut(tr("Ctrl+S"));
	connect(action_save, &QAction::triggered, this, &Widget::onTriggered_action_saveToFile);

	//重置图片
	action_restore = new QAction(tr("重置图片"), this);
	action_restore->setStatusTip(tr("重置此图片，取消所有加工"));
	action_restore->setIcon(QIcon("../resource/restore.png"));
	action_restore->setShortcut(tr("Ctrl+Shift+Z"));
	connect(action_restore, &QAction::triggered, this, &Widget::onTriggered_action_allRestore);

	//开始制作模式
	action_begin = new QAction(tr("图片加工"),this);
	action_begin->setStatusTip(tr("图片加工模式，可以混合修改图片"));
	action_begin->setIcon(QIcon("../resource/begin.png"));
	action_begin->setCheckable(true);
	connect(action_begin, &QAction::triggered, this, &Widget::onTriggered_action_process);

	//撤销
	action_return = new QAction(tr("撤销"), this);
	action_return->setShortcut(tr("Ctrl+Z"));
	action_return->setStatusTip(tr("撤销此操作"));
	action_return->setIcon(QIcon("../resource/return.png"));
	connect(action_return, &QAction::triggered, this, &Widget::onTriggered_action_undo);

	action_preview = new QAction(tr("确定"),this);
	action_preview->setIcon(QIcon("../resource/previewOk.png"));
	action_preview->setStatusTip(tr("确定操作"));
	connect(action_preview, &QAction::triggered, this, &Widget::onTriggered_action_previewToNormal);

	action_draw = new QAction(tr("绘画"), this);
	action_draw->setIcon(QIcon("../resource/draw.png"));
	action_draw->setStatusTip(tr("绘图操作"));
	connect(action_draw, &QAction::triggered, this, [=]() {
		widget_draw = new DrawWidget;
		widget_draw->show();
		widget_draw->setAttribute(Qt::WA_DeleteOnClose);
		});

	//颜色
	colorDialog = new QColorDialog(this);
	connect(colorDialog, &QColorDialog::currentColorChanged,
		this, &Widget::onTriggered_ColorDialog_choice);

	action_cvtColor_group = new QActionGroup(this);
	action_cvtColor_group->addAction(action_ori = new QAction("转换为原图", this));
	action_cvtColor_group->addAction(action_hls = new QAction("转换为HLS格式",this));
	action_cvtColor_group->addAction(action_hsv = new QAction("转换为HSV格式", this));
	action_cvtColor_group->addAction(action_rgb = new QAction("转换为RGB格式", this));
	action_cvtColor_group->addAction(action_lab = new QAction("转换为LAB格式", this));
	connect(action_cvtColor_group, &QActionGroup::triggered, this, [=](QAction* action) {
		img_base->cvtColor(action);
		});

	action_rotate_group = new QActionGroup(this);
	action_rotate_group->addAction(action_right90 = new QAction(tr("顺时针旋转90度"), this));
	action_rotate_group->addAction(action_right180 = new QAction(tr("顺时针旋转180度"), this));
	action_rotate_group->addAction(action_right270 = new QAction(tr("逆时针旋转90度"),this));
	connect(action_rotate_group, &QActionGroup::triggered, this, [=](QAction* action) {
		if (action == action_right90) {
			img_base->onTriggered_picture_rotate90();
		}
		else if (action == action_right180) {
			img_base->onTriggered_picture_rotate180();
		}
		else if (action == action_right270) {
			img_base->onTriggered_picture_rotate270();
		}
		});


	action_flip_group = new QActionGroup(this);
	action_flip_group->addAction(action_flip0 = new QAction(tr("垂直翻转"),this));
	action_flip_group->addAction(action_flip1 = new QAction(tr("水平翻转"), this));
	action_flip_group->addAction(action_flip_1 = new QAction(tr("垂直水平翻转"), this));
	connect(action_flip_group, &QActionGroup::triggered, this, [=](QAction* action) {
		if (action == action_flip0) {
			img_base->onTriggered_picture_flip0();
		}
		else if (action == action_flip1) {
			img_base->onTriggered_picture_flip1();
		}
		else if (action == action_flip_1) {
			img_base->onTriggered_picture_flip_1();
		}
		});

	action_mark = new QAction(tr("图像对比度提高"),this);
	connect(action_mark, &QAction::triggered, this, [=]() {
		img_base->onTriggered_picture_mask();
		});
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

	//图片格式转换
	menu_convert = menuBar()->addMenu(tr("&转换"));
	menu_convert->addAction(action_ori);
	menu_convert->addAction(action_hls);
	menu_convert->addAction(action_hsv);
	menu_convert->addAction(action_lab);
	menu_convert->addAction(action_rgb);

	//图像旋转
	menu_reverse = menuBar()->addMenu(tr("&旋转"));
	menu_reverse->addAction(action_right90);
	menu_reverse->addAction(action_right180);
	menu_reverse->addAction(action_right270);

	//图像翻转
	menu_flip = menuBar()->addMenu(tr("翻转"));
	menu_flip->addAction(action_flip0);
	menu_flip->addAction(action_flip1);
	menu_flip->addAction(action_flip_1);
	
	//对比度
	menu_mark = menuBar()->addMenu(tr("掩膜"));
	menu_mark->addAction(action_mark);
}

void Widget::createToolBar()
{
	toolbar1 = addToolBar(tr("file"));
	toolbar1->addAction(action_begin);
	toolbar1->addAction(action_restore);
	toolbar1->addAction(action_return);
	toolbar1->addAction(action_preview);
	toolbar1->addAction(action_draw);

}



void Widget::createToolBox()
{
	//-----------------模糊操作-----------------------------
	btngroups.push_back(btngroup_blur = new QButtonGroup(this));
	btngroup_blur->setExclusive(true);
	//连接信号
	connect(btngroup_blur, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_blur);

	
	QGridLayout* grid_blur = new QGridLayout;
	QWidget* a1 = createToolBtnItemWidget("均值模糊", BLUR::Average, "../resource/avg.png");
	a1->setStatusTip(tr("均值滤波：取周围像素平均值进行滤波，可以帮助消除图像尖锐噪声，实现图像平滑，模糊等功能"));
	grid_blur->addWidget(a1, 0, 0);

	QWidget* a2 = createToolBtnItemWidget("高斯模糊", BLUR::Gaussian, "../resource/gaussian.png");
	a2->setStatusTip(tr("高斯滤波：线性平滑滤波，适用于消除高斯噪声，广泛应用于图像处理的减噪过程。"));
	grid_blur->addWidget(a2,0,1);
	QWidget* a3 = createToolBtnItemWidget("中值模糊", BLUR::Median, "../resource/zhong.png");
	a3->setStatusTip(tr("中值滤波：非线性平滑滤波，它将每一像素点的灰度值设置为该点某邻域窗口内的所有像素点灰度值的中值。"));
	grid_blur->addWidget(a3,1,0);
	QWidget* a4 = createToolBtnItemWidget("双边滤波", BLUR::Bilateral, "../resource/shuangbian.png");
	a4->setStatusTip(tr("双边滤波：有很好的边缘保护效果，即可以在去噪的同时，保护图像的边缘特性。"));
	grid_blur->addWidget(a4,1,1);

	grid_blur->setRowStretch(3, 10);
	grid_blur->setColumnStretch(0, 5);
	grid_blur->setColumnStretch(2, 5);

	QWidget* widget_blur = new QWidget;
	widget_blur->setLayout(grid_blur);

	//-----------------阈值化操作-----------------------------
	btngroups.push_back(btngroup_threshold = new QButtonGroup(this));
	btngroup_threshold->setExclusive(true);
	//连接信号
	connect(btngroup_threshold, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_threshold);

	QGridLayout* grid_threshold = new QGridLayout;
	QWidget* b1 = createToolBtnItemWidget("二进制化", THRESHOLD::Binary, "../resource/2.png");
	b1->setStatusTip(tr("二进制阈值化：大变最大，小变0"));
	grid_threshold->addWidget(b1, 0, 0);

	QWidget* b2 = createToolBtnItemWidget("反二进制为零", THRESHOLD::Binary_inv, "../resource/f2.png");
	b2->setStatusTip(tr("反二进制阈值化：大变0，小变最大"));
	grid_threshold->addWidget(b2, 0, 1);

	QWidget* b3 = createToolBtnItemWidget("截断", THRESHOLD::Trunc, "../resource/jieduan.png");
	b3->setStatusTip(tr("截断阈值化：大变阈值，小不变"));
	grid_threshold->addWidget(b3, 1, 0);

	QWidget* b4 = createToolBtnItemWidget("阈值化为零", THRESHOLD::Tozero, "../resource/0.png");
	b4->setStatusTip(tr("阈值化为0：大不变，小变0"));
	grid_threshold->addWidget(b4, 1, 1);

	QWidget* b5 = createToolBtnItemWidget("反阈值化为零", THRESHOLD::Tozero_inv, "../resource/f0.png");
	b5->setStatusTip(tr("反阈值化为0：大变0，小不变"));
	grid_threshold->addWidget(b5, 2, 0);

	grid_threshold->setRowStretch(4, 10);
	grid_threshold->setColumnStretch(2, 10);

	QWidget* widget_threshold = new QWidget;
	widget_threshold->setLayout(grid_threshold);


	//-----------------形态化操作-----------------------------
	btngroups.push_back(btngroup_form = new QButtonGroup(this));
	btngroup_form->setExclusive(true);
	//连接信号
	connect(btngroup_form, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_morphology);

	QGridLayout* grid_form = new QGridLayout;
	QWidget* c1 = createToolBtnItemWidget("膨胀", FORM::Erode, "../resource/pengzhang.png");
	c1->setStatusTip(tr("膨胀：取每个位置领域内最大值，膨胀后输出图像的总体亮度的平均值比起原图会有所升高"));
	grid_form->addWidget(c1, 0, 0);

	QWidget* c2 = createToolBtnItemWidget("腐蚀", FORM::Dilate, "../resource/fushi.png");
	c2->setStatusTip(tr("腐蚀：取每个位置领域内最小值，腐蚀后输出图像的总体亮度的平均值比起原图会有所降低"));
	grid_form->addWidget(c2, 0, 1);

	QWidget* c3 = createToolBtnItemWidget("开运算", FORM::Open, "../resource/kai.png");
	c3->setStatusTip(tr("开运算：先腐蚀再膨胀，可在纤细点出分离物体。有助于消除噪音"));
	grid_form->addWidget(c3, 1, 0);

	QWidget* c4 = createToolBtnItemWidget("闭运算", FORM::Close, "../resource/bi.png");
	c4->setStatusTip(tr("闭运算：先膨胀后腐蚀，可以排除前景对象中的小孔或对象上的小黑点"));
	grid_form->addWidget(c4, 1, 1);

	QWidget* c5 = createToolBtnItemWidget("梯度", FORM::Gradient, "../resource/tidu.png");
	c5->setStatusTip(tr("梯度运算：膨胀图与腐蚀图之差，可以用于保留目标物体的边缘轮廓"));
	grid_form->addWidget(c5, 2, 0);

	QWidget* c6 = createToolBtnItemWidget("顶帽", FORM::Tophat, "../resource/dingmao.png");
	c6->setStatusTip(tr("顶帽运算：	原图与开运算图之差，分离比邻近点亮的斑块，用于突出原图像中比周围亮的区域"));
	grid_form->addWidget(c6, 2, 1);

	QWidget* c7 = createToolBtnItemWidget("黑帽", FORM::Tophat, "../resource/dingmao.png");
	c7->setStatusTip(tr("黑帽运算：闭运算图与原图差，分离比邻近点暗的斑块，突出原图像中比周围暗的区域"));
	grid_form->addWidget(c7, 3, 0);

	grid_form->addWidget(createToolBtnItemWidget("随机", FORM::Hitmiss, "../resource/suiji.png"), 3, 1);

	grid_form->setRowStretch(4, 10);
	grid_form->setColumnStretch(1, 10);

	QWidget* widget_from = new QWidget;
	widget_from->setLayout(grid_form);


	//-----------------连通区域分析操作-----------------------------
	btngroups.push_back(btngroup_connected = new QButtonGroup(this));
	btngroup_connected->setExclusive(true);//互斥

	connect(btngroup_connected, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_connected);
	
	QGridLayout* gird_connected = new QGridLayout;

	QWidget* d1 = createToolBtnItemWidget(tr("连通区域分析①"), CONNECTED::CONNECTED_TYPE1, "../resource/liantongkuai.png");
	d1->setStatusTip(tr("连通区域：找到图片之间的连通块，并且用不同颜色标记。"));
	gird_connected->addWidget(d1,0,0);

	QWidget* d2 = createToolBtnItemWidget(tr("连通区域分析②"), CONNECTED::CONNECTED_TYPE2, "../resource/liantongkuais.png");
	d2->setStatusTip(tr("连通区域：找到图片之间的连通块，并且使用方框进行标记。"));
	gird_connected->addWidget(d2,1,0);
	gird_connected->setRowStretch(4, 10);
	gird_connected->setColumnStretch(1, 10);

	QWidget* widget_connected = new QWidget;
	widget_connected->setLayout(gird_connected);

	//-----------------轮廓绘制操作-----------------------------
	btngroups.push_back(btngroup_contours = new QButtonGroup(this));
	btngroup_contours->setExclusive(true);

	connect(btngroup_contours, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_contours);

	QGridLayout* gird_contours = new QGridLayout;

	QWidget* e1 = createToolBtnItemWidget(tr("轮廓绘制操作"), CONTOURS::CONTOURS_TYPE1, "../resource/lunkuo.png");
	e1->setStatusTip(tr("轮廓绘制：绘制图像的轮廓信息"));
	gird_contours->addWidget(e1, 0, 0);

	gird_contours->setRowStretch(4, 10);
	gird_contours->setColumnStretch(1, 10);

	QWidget* widget_contours = new QWidget;
	widget_contours->setLayout(gird_contours);

	//-----------------图像效果增强------------------------------
	btngroups.push_back(btngroup_show = new QButtonGroup(this));
	btngroup_show->setExclusive(true);

	connect(btngroup_show, &QButtonGroup::buttonClicked, this, &Widget::onClicked_buttonGroup_show);

	QGridLayout* gird_effect = new QGridLayout;

	QWidget* f1 = createToolBtnItemWidget(tr("亮度调整"), SHOW::LIGHT, "../resource/light.png");
	f1->setStatusTip(tr("图像亮度调整：增强图像的亮度"));
	gird_effect->addWidget(f1, 0, 0);

	gird_effect->setRowStretch(4, 10);
	gird_effect->setColumnStretch(1, 10);

	QWidget* widget_effect= new QWidget;
	widget_effect->setLayout(gird_effect);

	//-----------------创建ToolBox-----------------
	toolbox_side = new QToolBox(this);
	toolbox_side->setMinimumWidth(200);
	toolbox_side->setMaximumWidth(200);
	toolbox_side->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	toolbox_side->addItem(widget_blur, "图像模糊操作");
	toolbox_side->setFrameShape(QFrame::StyledPanel);
	toolbox_side->addItem(widget_threshold, "图像阈值操作");
	toolbox_side->addItem(widget_from, "图像形态化操作");
	toolbox_side->addItem(widget_connected, "图像连通分析");
	toolbox_side->addItem(widget_contours, "图像轮廓分析");
	toolbox_side->addItem(widget_effect, "图像效果增强");
	toolbox_side->setCurrentIndex(0); 


	////获取切换后的toolbox索引
	connect(toolbox_side, &QToolBox::currentChanged, this, [=](int value) {
		preToolBoxIndex = curToolBoxIndex;
		curToolBoxIndex = value;

		//	清除之前页面上的选项
		auto btns = btngroups[preToolBoxIndex]->buttons();
		btngroups[preToolBoxIndex]->setExclusive(false);
		for (auto& x : btns) {
			x->setChecked(false);
		}
		btngroups[preToolBoxIndex]->setExclusive(true);
	
		if (mode) {
			//图片数据继承下来
		}
		else {
			restore_cutOperation();
		}
	});
}

void Widget::createStatusBar()
{
	auto bar = statusBar();
	statusBar()->addWidget(statusLab = new QLabel("默认模式"));
}

QWidget* Widget::createToolBtnItemWidget(const QString& text, int id, const QString& fileName)
{
	QToolButton* btn = new QToolButton;
	btn->setObjectName("function_tbtn");
	btn->resize(48, 48);
	btn->setCheckable(true);
	btn->setChecked(false);
	if (!fileName.isEmpty()) {
		btn->setIcon(QIcon(fileName));
		btn->setIconSize(btn->size());
	}

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
	else if (belongsToEnum<CONTOURS>(id)) {
		btngroup_contours->addButton(btn, id); //轮廓绘制
	}
	else if (belongsToEnum<CVTCOLOR>(id)) {
		btngroup_cvtColor->addButton(btn, id); //图像转换
	}
	else if (belongsToEnum<SHOW>(id)) {
		btngroup_show->addButton(btn, id);	  //效果增强
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

template <typename Type>
QHBoxLayout* Widget::create_Edit_hLayout(const QString& filter, const QString& text, Type* t)
{
	/*
	在C++模板函数中，当使用类型模板参数时，传递给模板函数的参数会根据其类型进行实例化。在您的例子中，模板参数 Type 是一个类型参数，而不是一个变量参数。
	当您传递 blur 变量给模板函数时，实际上是根据 blur 的类型进行模板实例化，并生成相应的函数代码。在模板函数内部，t 参数是根据实例化的类型来创建的，而不是直接引用传递。
	这意味着 t 的内存地址与 blur 的内存地址可能不同，因为它们是不同的对象。尽管在语义上它们可能表示相同的值，但它们是不同的实例。
	如果您希望 t 和 blur 具有相同的内存地址，可以将 blur 作为指针参数传递给模板函数，并在函数内部使用指针进行操作
	*/
	QLineEdit* edit = new QLineEdit;
	edit->setPlaceholderText(text);
	edit->setFixedWidth(edit->fontMetrics().boundingRect(edit->placeholderText()).width()+20);
	//设置验证器
	QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression(filter), edit);
	edit->setValidator(validator);
	connect(edit, &QLineEdit::returnPressed, this, [=]() {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		QList<QString> lStr = edit->text().split(" ");
		(*t)->onReturnPressed_Edit(lStr);
		});
	QPushButton* ok_btn = new QPushButton("确定");
	connect(ok_btn, &QPushButton::clicked, this, [=]() {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		QString str = edit->text();
		int pos = 0;
		auto state = validator->validate(str, pos);
		//必须首先合法
		if (state == QValidator::Acceptable) {
			QList<QString> lStr = edit->text().split(" ");
			(*t)->onReturnPressed_Edit(lStr);
		}
		});

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addStretch(1);
	hLayout->addWidget(edit);
	hLayout->addSpacing(5);
	hLayout->addWidget(ok_btn);
	hLayout->addStretch(1);
	hLayout->setSpacing(0);

	return hLayout;
}


//-----------------均值GUI----------------------------
QHBoxLayout* Widget::create_GUIAvgBlur() //1：均值 2：高斯
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(AVG_BLUR_MIN_SIZE, AVG_BLUR_MAX_SIZE);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(AVG_BLUR_MIN_SIZE, AVG_BLUR_MAX_SIZE);
	slider2->setSizePolicy(QSizePolicy::Expanding,  QSizePolicy::Fixed);
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(AVG_BLUR_MIN_SIZE, AVG_BLUR_MAX_SIZE);
	slider3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("kernel"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, this, [=](int value) {
		if (slider2->value() == slider2->minimum() && slider3->value() == slider3->minimum()) {
			blur->anchorX = blur->anchorY = blur->avg_Ksize / 2;
		}
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		blur->onTriggered_slider1_valueChange_avgBlur(value);
	});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_avgBlur_s1 = new QWidget;
	adj_avgBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("sigmaX"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		blur->onTriggered_slider2_valueChange_avgBlur(value);
	});

	QWidget* adj_avgBlur_s2 = new QWidget;
	adj_avgBlur_s2->setLayout(hlayout2);

	//--------------------------------

	QHBoxLayout* hlayout3 = new QHBoxLayout;
	hlayout3->addWidget(new QLabel("sigmaY"));
	hlayout3->addWidget(slider3);
	//Y偏移连接信号
	connect(slider3, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		blur->onTriggered_slider3_valueChange_avgBlur(value);
		});

	QWidget* adj_avgBlur_s3 = new QWidget;
	adj_avgBlur_s3->setLayout(hlayout3);

	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_avgBlur_s1);
	vLayout->addWidget(adj_avgBlur_s2);
	vLayout->addWidget(adj_avgBlur_s3);
	vLayout->setSpacing(0);

	QVBoxLayout* vLayout_2 = new QVBoxLayout;
	vLayout_2->addLayout(vLayout);
	vLayout_2->addLayout(create_Edit_hLayout("\\d+\\s\\d+\\s\\d+", "KSize X Y",&blur));
	vLayout_2->setSpacing(0);

	QHBoxLayout* hboxl = new QHBoxLayout;
	hboxl->addLayout(vLayout_2);

	return hboxl;
}

//-----------------高斯GUI----------------------------
QHBoxLayout* Widget::create_GUIGaussianBlur()
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(GAUSSIAN_BLUR_MIN_SIZE, GAUSSIAN_BLUR_MAX_SIZE);
	slider1->setSingleStep(2);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(1, GAUSSIAN_BLUR_SIGMAX);
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(1, GAUSSIAN_BLUR_SIGMAY);
	slider3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("ksize"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, this, [=](int value) {

		if (value % 2 == 0) {
			value += 1;
		}
		if (slider2->value() == slider2->minimum() && slider3->value() == slider3->minimum()) {
			blur->sigmaX = blur->sigmaY = blur->gas_Ksize / 2;
		}
		if (mode) {
			sub_lab_img->setVisible(true);
		}
	blur->onTriggered_slider1_valueChange_gaussianBlur(value);
		});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_gasBlur_s1 = new QWidget;
	adj_gasBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("sigmaX"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		blur->onTriggered_slider2_valueChange_gaussianBlur(value);
		});

	QWidget* adj_gasBlur_s2 = new QWidget;
	adj_gasBlur_s2->setLayout(hlayout2);

	//--------------------------------

	QHBoxLayout* hlayout3 = new QHBoxLayout;
	hlayout3->addWidget(new QLabel("sigmaY"));
	hlayout3->addWidget(slider3);
	//Y偏移连接信号
	connect(slider3, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		blur->onTriggered_slider3_valueChange_gaussianBlur(value);
		});

	QWidget* adj_gasBlur_s3 = new QWidget;
	adj_gasBlur_s3->setLayout(hlayout3);

	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_gasBlur_s1);
	vLayout->addWidget(adj_gasBlur_s2);
	vLayout->addWidget(adj_gasBlur_s3);

	QVBoxLayout* vLayout_2 = new QVBoxLayout;
	vLayout_2->addLayout(vLayout);
	vLayout_2->addLayout(create_Edit_hLayout("\\d+\\s\\d+\\s\\d+", "KSize X Y",&blur));
	vLayout_2->setSpacing(0);

	QHBoxLayout* hboxl = new QHBoxLayout;
	hboxl->addLayout(vLayout_2);

	return hboxl;
}


//----------------中值GUI----------------------------
QHBoxLayout* Widget::create_GUIMedianBlur()
{
	QSlider* slider = new QSlider(Qt::Horizontal);
	slider->setRange(1, MEDIAN_BLUR_MAX);
	slider->setSingleStep(2);

	connect(slider, &QSlider::sliderMoved, this, [=](int value) {
		if (value % 2 == 0) {
			value += 1;
		}
	if (mode) {
		sub_lab_img->setVisible(true);
	}
		blur->onTriggered_slider_valueChange_medianBlur(value);
	});


	QHBoxLayout* hLayout2 = new QHBoxLayout;
	hLayout2->addWidget(new QLabel("ksize"));
	hLayout2->addWidget(slider);

	QVBoxLayout* vLayout_2 = new QVBoxLayout;
	vLayout_2->addLayout(hLayout2);
	vLayout_2->addLayout(create_Edit_hLayout("\\d+", "KSize",&blur));
	vLayout_2->setSpacing(0);

	QHBoxLayout* hboxl = new QHBoxLayout;
	hboxl->addLayout(vLayout_2);

	return hboxl;
}



//----------------双边GUI----------------------------
QHBoxLayout* Widget::create_GUIBilateralBlur()
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(1, BILATERAL_BLUR_MAX_SIZE);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(0, BILATERAL_BLUR_MAX_COLOR);
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(0, BILATERAL_BLUR_MAX_SPACE);
	slider3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("ksize"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		blur->onTriggered_slider1_valueChange_bilateralBlur(value);
	});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_binBlur_s1 = new QWidget;
	adj_binBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("sigmaX"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		blur->onTriggered_slider2_valueChange_bilateralBlur(value);
		});

	QWidget* adj_binBlur_s2 = new QWidget;
	adj_binBlur_s2->setLayout(hlayout2);

	//--------------------------------

	QHBoxLayout* hlayout3 = new QHBoxLayout;
	hlayout3->addWidget(new QLabel("sigmaY"));
	hlayout3->addWidget(slider3);
	//Y偏移连接信号
	connect(slider3, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		blur->onTriggered_slider3_valueChange_bilateralBlur(value);
		});

	QWidget* adj_binBlur_s3 = new QWidget;
	adj_binBlur_s3->setLayout(hlayout3);

	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_binBlur_s1);
	vLayout->addWidget(adj_binBlur_s2);
	vLayout->addWidget(adj_binBlur_s3);

	QVBoxLayout* vLayout_2 = new QVBoxLayout;
	vLayout_2->addLayout(vLayout);
	vLayout_2->addLayout(create_Edit_hLayout("\\d+\\s\\d+\\s\\d+", "bin_d sigmaColor sigmaSpace", &blur));
	vLayout_2->setSpacing(0);

	QHBoxLayout* hboxl = new QHBoxLayout;
	hboxl->addLayout(vLayout_2);
	return hboxl;
}

//----------------阈值GUI----------------------------
QHBoxLayout* Widget::create_GUIThreshold()
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
	connect(slider1, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		threshold->onTriggered_slider1_valueChanged_thresholdValue(value);
		});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_binBlur_s1 = new QWidget;
	adj_binBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("maxval"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		threshold->onTriggered_slider2_valueChanged_maxValue(value);
		});

	QWidget* adj_binBlur_s2 = new QWidget;
	adj_binBlur_s2->setLayout(hlayout2);

	//--------------------------------


	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_binBlur_s1);
	vLayout->addWidget(adj_binBlur_s2);

	QVBoxLayout* vLayout_2 = new QVBoxLayout;
	vLayout_2->addLayout(vLayout);
	vLayout_2->addLayout(create_Edit_hLayout("\\d+\\s\\d+", "threshold_value maxVal",&threshold));
	vLayout_2->setSpacing(0);

	QHBoxLayout* hboxl = new QHBoxLayout;
	hboxl->addLayout(vLayout_2);

	return hboxl;

}

//----------------形态学GUI----------------------------
QHBoxLayout* Widget::create_GUIMorphology()
{
	//Kernel
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(1, 50);
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//anthorX
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(1,  20);
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//anthorY
	QSlider* slider3 = new QSlider(Qt::Horizontal);
	slider3->setRange(1,  20);
	slider3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//水平布局管理两个控件：label和滑块
	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("Kernel"));
	hlayout1->addWidget(slider1);
	connect(slider1, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		morphology->onTriggered_slider1_valueChanged_kernel(value);
		});

	//将每一个水平布局放到一个窗口中，便于进行隐藏
	QWidget* adj_binBlur_s1 = new QWidget;
	adj_binBlur_s1->setLayout(hlayout1);

	//--------------------------------

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("anchorX"));
	hlayout2->addWidget(slider2);
	//X偏移连接信号
	connect(slider2, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		morphology->onTriggered_slider2_valueChanged_anchorX(value);
		});

	QWidget* adj_binBlur_s2 = new QWidget;
	adj_binBlur_s2->setLayout(hlayout2);

	//--------------------------------

	QHBoxLayout* hlayout3 = new QHBoxLayout;
	hlayout3->addWidget(new QLabel("anchorY"));
	hlayout3->addWidget(slider3);
	//Y偏移连接信号
	connect(slider3, &QSlider::sliderMoved, this, [=](int value) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		morphology->onTriggered_slider3_valueChanged_anchorY(value);
		});

	QWidget* adj_binBlur_s3 = new QWidget;
	adj_binBlur_s3->setLayout(hlayout3);

	//将三个QWidget添加到一个垂直布局
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(adj_binBlur_s1);
	vLayout->addWidget(adj_binBlur_s2);
	vLayout->addWidget(adj_binBlur_s3);

	QVBoxLayout* vLayout_2 = new QVBoxLayout;
	vLayout_2->addLayout(vLayout);
	vLayout_2->addLayout(create_Edit_hLayout("\\d+\\s\\d+\\s\\d+\\s\\d+", "Kernel X Y iters",&morphology));
	vLayout_2->setSpacing(0);

	QHBoxLayout* hboxl = new QHBoxLayout;
	hboxl->addLayout(vLayout_2);

	return hboxl;
}


//----------------连通区域分析GUI----------------------------
QHBoxLayout* Widget::create_GUIConnected()
{
	QComboBox* comb1 = new QComboBox; //选择 connectivity = 8 or 4
	comb1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	comb1->setObjectName(tr("connectivity"));
	comb1->addItem(tr("8"));
	comb1->addItem(tr("4"));
	comb1->setEditable(false);
	connect(comb1, &QComboBox::activated, this, [=](int index) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		connected->onTriggered_Comb1_currentTextChanged_connectivtiy(index);
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
	connect(comb2, &QComboBox::activated, this, [=](int index) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		connected->onTriggered_Comb2_currentTextChanged_ccltype(index);
	});

	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("邻域"));
	hlayout1->addWidget(comb1);
	hlayout1->setAlignment(Qt::AlignmentFlag::AlignCenter);

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("联通算法"));
	hlayout2->addWidget(comb2);
	hlayout2->setAlignment(Qt::AlignmentFlag::AlignCenter);

	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->addLayout(hlayout1);
	vlayout->addLayout(hlayout2);

	QHBoxLayout* hboxl = new QHBoxLayout;
	hboxl->addLayout(vlayout);

	return hboxl;
}

//----------------图像轮廓分析GUI----------------------------
QHBoxLayout* Widget::create_GUIContours()
{
	QComboBox* comb1 = new QComboBox; // mode
	comb1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	comb1->setObjectName(tr("contours_mode"));
	comb1->addItem(tr("RETR_EXTERNAL"));
	comb1->addItem(tr("RETR_LIST"));
	comb1->addItem(tr("RETR_CCOMP"));
	comb1->addItem(tr("RETR_TREE"));
	comb1->setEditable(false);
	connect(comb1, &QComboBox::activated, this, [=](int index) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		contours->onTriggered_Comb1_currentTextChanged_contoursMode(index);
		});

	QComboBox* comb2 = new QComboBox;  //method
	comb2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	comb2->setObjectName(tr("contours_method"));
	comb2->addItem(tr("CHAIN_APPROX_NONE"));
	comb2->addItem(tr("CHAIN_APPROX_SIMPLE"));
	comb2->addItem(tr("CHAIN_APPROX_TC89_L1"));
	comb2->addItem(tr("CHAIN_APPROX_TC89_KCOS"));
	comb2->setEditable(false);
	connect(comb2, &QComboBox::activated, this, [=](int index) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		contours->onTriggered_Comb2_currentTextChanged_contoursMethod(index);
		});

	QComboBox* comb3 = new QComboBox;  //绘制线的粗细
	comb3->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	comb3->setObjectName(tr("contours_thick"));
	for (int i = 1; i <= 10; i++) {
		comb3->addItem(QString("%1").arg(i));
	}

	connect(comb3, &QComboBox::activated, this, [=](int index) {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
	contours->onTriggered_Comb3_currentTextChanged_contoursThick(index);
		});

	QPushButton* btn = new QPushButton;
	btn->setText("绘制图像凸包");
	connect(btn, &QPushButton::clicked, this, [=]() {
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		contours->onClicked_btn_convexHull();
	});

	QToolButton* color = new QToolButton;
	color->setIcon(QPixmap("../resource/colors.png"));
	color->setMinimumSize(48, 48);
	color->setMaximumSize(48, 48);
	color->setIconSize(color->size());
	connect(color, &QToolButton::clicked, this, [=]() {
		colorDialog->setMinimumSize(375, 375);
		colorDialog->setMaximumSize(653, 498);
		colorDialog->setGeometry(810, 306, 375, 375);
		colorDialog->show(); //弹出颜色框
		if (mode) {
			sub_lab_img->setVisible(true);
			}
		});

	QGridLayout* grid = new QGridLayout;
	grid->addWidget(color, 0, 0, Qt::AlignHCenter);
	grid->addWidget(new QLabel("绘制颜色"), 1, 0, Qt::AlignHCenter);

	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("轮廓检索模式"));
	hlayout1->addWidget(comb1);
	hlayout1->setAlignment(Qt::AlignmentFlag::AlignCenter);

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("轮廓逼近方法"));
	hlayout2->addWidget(comb2);
	hlayout2->setAlignment(Qt::AlignmentFlag::AlignCenter);
	
	QHBoxLayout* hlayout3 = new QHBoxLayout;
	hlayout3->addWidget(new QLabel("绘制线宽度"));
	hlayout3->addWidget(comb3);
	hlayout3->setAlignment(Qt::AlignmentFlag::AlignCenter);

	QWidget* w1 = new QWidget;
	w1->setLayout(hlayout1);

	QWidget* w2 = new QWidget;
	w2->setLayout(hlayout2);

	QWidget* w3 = new QWidget;
	w3->setLayout(hlayout3);

	QWidget* w4 = new QWidget;
	w4->setLayout(grid);
	QVBoxLayout* vlayoutRight = new QVBoxLayout;
	vlayoutRight->addWidget(w4);

	QVBoxLayout* vlayoutLeft = new QVBoxLayout;
	vlayoutLeft->addWidget(w1);
	vlayoutLeft->addWidget(w2);
	vlayoutLeft->addWidget(w3);
	vlayoutLeft->addWidget(btn);

	QHBoxLayout* hlayout = new QHBoxLayout;
	hlayout->addLayout(vlayoutLeft);
	hlayout->addLayout(vlayoutRight);

	QHBoxLayout* hboxl = new QHBoxLayout;
	hboxl->addLayout(hlayout);

	return hboxl;
}


//----------------图像效果增强GUI----------------------------
QVBoxLayout* Widget::create_GUIShow()
{
	QSlider* slider1 = new QSlider(Qt::Horizontal);
	slider1->setRange(1,100);
	slider1->setObjectName("bright_value");
	slider1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	
	QSlider* slider2 = new QSlider(Qt::Horizontal);
	slider2->setRange(1, 100);
	slider2->setObjectName("bright_value");
	slider2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(slider1, &QSlider::sliderMoved, this, [=](int value) {
		slider2->setValue(slider2->minimum());
	if (mode) {
		sub_lab_img->setVisible(true);
	}
	showeffect->onTriggered_slider_valueChange_brighten(value);
		});


	connect(slider2, &QSlider::sliderMoved, this, [=](int value) {
		value = -value;	
	slider1->setValue(slider1->minimum());
		if (mode) {
			sub_lab_img->setVisible(true);
		}
		showeffect->onTriggered_slider_valueChange_brighten(value);
		});


	QHBoxLayout* hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel("亮度增加"));
	hlayout1->addWidget(slider1);

	QHBoxLayout* hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel("亮度降低"));
	hlayout2->addWidget(slider2);

	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->addLayout(hlayout1);
	vlayout->addLayout(hlayout2);

	return vlayout;
}