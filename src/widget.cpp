#include "../include/widget_include_files.h"
#include "opencv2/core/utils/logger.hpp"

//单例对象
Widget* Widget::widget = nullptr;

//获取单例对象
Widget* Widget::getInstance() {
	if (widget == nullptr) {
		//消除Debug信息
		cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

		widget = new Widget;
	}
	return widget;
}

Widget::Widget(QWidget* parent)
	:QMainWindow(parent)
	, res(new Res("dog.png", this))
{
	init_readJson();		//读取配置文件
	init_WidgetInfo();		//设置主窗口信息	
	init_OpencvFunctions();	//初始化opencv操作函数
	init_Optsdialog();		//创建opencv操作函数数值调整框
	createAction();			//创建行为
	createToolBar();		//创建工具栏
	createMenu();			//创建菜单
	createToolBox();		//创建左侧操作区域与GUI界面
	createStatusBar();		//创建状态栏
	init_Label();			//预处理图片显示
	init_WidgetLayout();	//设置主窗口布局

	all_dlgs.resize(10);

	//加载主题
	if (config.win_theme == "light") {
		on_action_theme_triggered(0);
	}
	else if (config.win_theme == "dark") {
		on_action_theme_triggered(1);
	}
}

Widget::~Widget()
{
	//销毁所有dialog
	for (auto& x : all_dlgs) {
		delete x;
		x = nullptr;
	}
	all_dlgs.clear();

	//内存释放
	for (auto& x : Opts) {
		delete x;
		x = nullptr;
	}
	Opts.clear();

	if (all_screen) {
		delete all_screen;
		all_screen = nullptr;
	}

	while (!undo_sta.empty()) {
		undo_sta.pop();
	}
	delete res;
	res = nullptr;
}

void Widget::init_readJson()
{
	//读取json文件
	QFile jsonFile("config.json");
	if (!jsonFile.open(QIODevice::ReadOnly)) {
		//没有这个json配置文件，则创建一个
		QJsonObject jsonObj;
		jsonObj["win_title"] = config.win_title;
		jsonObj["win_location_x"] = config.win_location_x;
		jsonObj["win_location_y"] = config.win_location_y;
		jsonObj["win_theme"] = config.win_theme;
		jsonObj["win_screen_scale"] = config.win_screen_scale;
		// 创建JSON文档
		QJsonDocument jsonDoc(jsonObj);
		// 将JSON文档写入文件
		QFile file("config.json");
		if (file.open(QFile::WriteOnly | QFile::Text)) {
			QTextStream out(&file);
			out << jsonDoc.toJson();
			file.close();
		}
		else {
			qWarning() << "打开json文件写入失败!" << file.errorString();
		}
	}
	else {
		QJsonDocument json = QJsonDocument::fromJson(jsonFile.readAll());
		if (json.isNull()) {
			qWarning() << "json文件为Null";
			return;
		}
		QJsonObject obj1 = json.object();
		if (obj1.contains("win_title")) {
			config.win_title = obj1.value("win_title").toString();
		}
		if (obj1.contains("win_location_x")) {
			config.win_location_x = obj1.value("win_location_x").toInt();
		}
		if (obj1.contains("win_location_x")) {
			config.win_location_y = obj1.value("win_location_y").toInt();
		}
		if (obj1.contains("win_theme")) {
			config.win_theme = obj1.value("win_theme").toString();
		}
		if (obj1.contains("win_screen_scale")) {
			config.win_screen_scale = obj1.value("win_screen_scale").toDouble();
		}
	}
	
}

void Widget::init_WidgetInfo()
{
	installEventFilter(this);

	this->setWindowTitle(config.win_title);
	// 禁用最大化和最小化按钮
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);

	QScreen* windowScreen = QApplication::primaryScreen();
	this->move(config.win_location_x, config.win_location_y);

}


void Widget::init_Label()
{
	lab_img = LabelBuilder()
		.setObjectName("lab_img")
		.setAlignment(Qt::AlignCenter)
		.setContextMenuPolicy(Qt::CustomContextMenu)
		.setPixmap(QPixmap::fromImage(res->curr_img))
		.create(this);
	//图片上下文菜单
	connect(lab_img, &QLabel::customContextMenuRequested, this, &Widget::on_label_customContextMenuRequested);
}

void Widget::init_Optsdialog()
{
	for (auto& x : all_dlgs) {
		x->setGeometry(this->rect().x() + this->width(), this->rect().y() + 100, 200, 200);
		x->setWindowFlags(x->windowFlags() | Qt::WindowStaysOnTopHint);
	}
}

void Widget::init_WidgetLayout()
{
	//左侧是一个DockWidget，其内部是一个QToolBox
	QDockWidget* dockWidget = new QDockWidget("操作区域", this);
	QLabel* title_lab = new QLabel("操作管理器", dockWidget);
	title_lab->setObjectName("dock_title_lab");
	dockWidget->setTitleBarWidget(title_lab);
	dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	QHBoxLayout* toolBox_layout = new QHBoxLayout;
	toolBox_layout->addWidget(toolbox_side);

	//Dock内部空间
	QWidget* dockContent = new QWidget;
	dockContent->setLayout(toolBox_layout);//添加一个内容

	dockWidget->setWidget(dockContent);

	//添加到mainWindow中
	this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

	//--------------------------------------------------
	//右侧
	//添加一个QScrollArea用于处理不同尺寸的图片
	scrollArea = new QScrollArea;
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidgetResizable(true);
	scrollArea->setContentsMargins(0,0,0,0);
	//滚动窗口添加此图片
	scrollArea->setWidget(lab_img);

	//图片预缩放
	scaledDelta = ori_scaledDelta  = init_scaledImageOk();

	layout_changeToNormal();

}

void Widget::init_OpencvFunctions()
{
	//初始化
	Opts.push_back(blur = new Blur);
	Opts.push_back(threshold = new Threshold);
	Opts.push_back(morphology = new Morphology);
	Opts.push_back(connected = new Connected);
	Opts.push_back(contours = new Contours);
	Opts.push_back(showeffect = new Showeffect);
	Opts.push_back(img_base = new BaseOperate);
}

void Widget::mousePressEvent(QMouseEvent* ev)
{
	hideAllDialog(nullptr);
}

void Widget::moveEvent(QMoveEvent* ev)
{
	config.win_location_x = this->pos().x(); //获取坐标
	config.win_location_y = this->pos().y()	;
}

void Widget::wheelEvent(QWheelEvent* ev)
{
	if (ev->modifiers() & Qt::ControlModifier) { 
		//鼠标 + 滑轮
		double angleDelta = ev->angleDelta().y(); // 120
		if (angleDelta > 0) {

			angleDelta = 1.1; //放大
		}
		else {
			angleDelta = 0.9; //缩小
		}

		scaledDelta *= angleDelta;
		update_Image(scaledDelta);
	}
	QMainWindow::wheelEvent(ev);
}


void Widget::on_label_customContextMenuRequested(const QPoint& pos) {
	context_menu->exec(QCursor::pos());
}

void Widget::on_buttonGroup_everyOpeartions_choice(Object*& op,QButtonGroup* btn_group,QAbstractButton* btn){
	op->current_choice = btn_group->id(btn);

	choice_buttonGroupsBtns();

	int dlg_id = switch_Dialog_id(op->current_choice);
	if (!all_dlgs[dlg_id]) {
		//如果这个操作此前没有做过，则初始化这个操作的Dialog
		choice_GUI_create(dlg_id);
	}
	hideAllDialog(all_dlgs[dlg_id]);
	all_dlgs[dlg_id]->open();
	all_dlgs[dlg_id]->raise();
}

void Widget::on_action_exit_triggered()
{
	this->close();
}

void Widget::on_action_openFile_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(nullptr, "选择文件", ".",	"图像文件(*.png *.jpg)");
	if (!fileName.isEmpty()) {
		reload_Resources_ScrollArea(fileName);		
	}
}

void Widget::on_action_openWorks_triggered()
{
	//清除原始内容
	work_files.clear();
	QString FloderPath = QFileDialog::getExistingDirectory(this, "选择文件夹", "../resource/testImages");
	if (FloderPath.isEmpty()) {
		//取消
		return;
	}
	if (!loadImagesFormFloder(FloderPath)) {//初始化
		//为空！没有找到图片资源
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("打开失败"));
		msgBox.setText(tr("在此文件夹中没有找到任何图片资源，请重新选择文件夹或取消"));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.addButton(tr("继续"), QMessageBox::AcceptRole);
		msgBox.addButton(tr("取消"), QMessageBox::RejectRole);

		// 显示消息框并等待用户响应
		int result = msgBox.exec();

		// 根据用户的选择执行不同的操作
		if (result == QMessageBox::AcceptRole) {
			//重新选择文件夹
			on_action_openWorks_triggered();
			return;
		}
		else if (result == QMessageBox::RejectRole) {
			return; //取消选择
		}
	}
	// 即时加载图片
	reload_Resources_ScrollArea(work_files[work_currentIndex], 1);
	
	//------------------------------
	//切换布局
	layout_changeToWork();

	connect(btn_work_next, &QPushButton::clicked, this, &Widget::on_pushButton_next_clicked);
	connect(btn_work_prev, &QPushButton::clicked, this, &Widget::on_pushButton_prev_clicked);
}

void Widget::on_action_saveFile_triggered()
{
	//保存加工后的图片
	QString FileName = QFileDialog::getSaveFileName(nullptr, "save image", ".", "Images(*.png *.bmp *.jpg)");
	if (!FileName.isEmpty()) {
		res->curr_img.save(FileName);
	}
}

void Widget::on_action_allRestore_triggered()
{
	/*
	重置所有操作至原始图片
	*/
	//清除所有的数据信息
	for (auto& x : Opts) {
		x->initialize();
	}

	clearAllWidgetValue();

	updateFromRoot();
}

void Widget::on_action_previewToNormal_triggered()
{
	//预览图点击确定转换为正常图 lan_img
	if (mode) {
		//lab_img->setPixmap(QPixmap::fromImage(curr_img));
		get()->update_Image(get()->ori_scaledDelta);
	}
}

void Widget::on_action_drawBoard_triggered()
{
	widget_draw = new DrawWidget;
	widget_draw->show();
	widget_draw->setAttribute(Qt::WA_DeleteOnClose);
}

void Widget::on_colorDialog_choice_triggered(const QColor& color)
{
	//选择颜色
	contours->onTriggered_Color_currentTextChanged_contoursColor(color);
}

void Widget::on_actionGroup_cvtColor_triggered(QAction* action)
{
	img_base->cvtColor(action);
}

void Widget::on_actionGroup_rotate_triggered(QAction* action)
{
	if (action == action_right90) {
		img_base->onTriggered_picture_rotate90();
	}
	else if (action == action_right180) {
		img_base->onTriggered_picture_rotate180();
	}
	else if (action == action_right270) {
		img_base->onTriggered_picture_rotate270();
	}
}

void Widget::on_actionGroup_flip_triggered(QAction* action)
{
	if (action == action_flip0) {
		img_base->onTriggered_picture_flip0();
	}
	else if (action == action_flip1) {
		img_base->onTriggered_picture_flip1();
	}
	else if (action == action_flip_1) {
		img_base->onTriggered_picture_flip_1();
	}
}

void Widget::on_action_theme_triggered(int type)
{
	QFile qssFile;
	if (type == 0) {
		qssFile.setFileName("../resource/qss/light.css");
	}
	else if ( type == 1){
		qssFile.setFileName("../resource/qss/dark.css");
	}
	if (qssFile.open(QFile::OpenModeFlag::ReadOnly)) {
		this->setStyleSheet(qssFile.readAll());
	}	
}

void Widget::on_action_jie_triggered()
{
	//图片截取Widget
	if (!look) {
		look = new LookWidget(this);
	}
	look->reset(QPixmap::fromImage(res->curr_img));
	look->show();
}

void Widget::on_action_capture_triggered()
{
	QScreen* screen = QApplication::primaryScreen();
	QPixmap screen_lab = screen->grabWindow(0);

	//获取缩放比例
	config.win_screen_scale = screen->devicePixelRatio();

	if (!all_screen) {
		//需要单独释放
		all_screen = new CaptureWidget;
	}
	all_screen->scale = config.win_screen_scale;
	all_screen->lab_img->setPixmap(screen_lab);
	all_screen->showFullScreen();
	all_screen->show();
}

void Widget::on_action_fileInfo_triggered()
{
	//文件名称
	QDialog* dia = new QDialog;

	QFormLayout* form_name = new QFormLayout;
	auto edit_name = new QLineEdit(res->fileInfo.fileName());
	form_name->addRow(new QLabel("文件名称："), edit_name);
	connect(edit_name, &QLineEdit::editingFinished, this, [=]() {
		//修改文件名字
		QString oldPathName = res->fileInfo.absoluteFilePath();
		QString newPathName = res->fileInfo.absolutePath() + "/" + edit_name->text();
		QFile::rename(oldPathName, newPathName);
		});

	//文件路径
	QFormLayout* form_path = new QFormLayout;
	auto edit_path = new QLabel(res->fileInfo.absoluteFilePath());
	form_path->addRow(new QLabel("文件名称："), edit_path);

	//文件大小
	QFormLayout* form_size = new QFormLayout;
	auto B = res->fileInfo.size() / 1024;
	auto edit_size = new QLabel(QString::number(B) + " Bytes");
	form_size->addRow(new QLabel("文件大小："), edit_size);

	//图像尺寸
	QFormLayout* form_rect = new QFormLayout;
	auto edit_rect = new QLabel(QString::number(res->root_mt.cols) + " * " + QString::number(res->root_mt.rows));
	form_rect->addRow(new QLabel("图像尺寸："), edit_rect);

	//图像色彩模式
	QFormLayout* form_mode = new QFormLayout;
	auto edit_mode = new QLabel(QVariant(res->curr_img.format()).toString());
	form_mode->addRow(new QLabel("图像色彩模式："), edit_mode);

	//图像通道数
	QFormLayout* form_channels = new QFormLayout;
	auto edit_channels = new QLabel(QString::number(res->root_mt.channels()));
	form_channels->addRow(new QLabel("图像通道数："), edit_channels);

	//图像文件格式
	QFormLayout* form_geshi = new QFormLayout;
	auto edit_geshi = new QLabel(QString(res->fileInfo.suffix().toLower()));
	form_geshi->addRow(new QLabel("文件格式:"), edit_geshi);

	QVBoxLayout* v = new QVBoxLayout;
	v->addLayout(form_name);
	v->addLayout(form_path);
	v->addLayout(form_size);
	v->addLayout(form_rect);
	v->addLayout(form_mode);
	v->addLayout(form_channels);
	v->addLayout(form_geshi);

	dia->setLayout(v);
	dia->open();
	dia->setAttribute(Qt::WA_DeleteOnClose);
}

void Widget::on_actionGroup_help_triggered(QAction* action)
{
	if (action == action_help) {
		//打开帮助文档
		QUrl help_url("https://github.com/luumod/myPhotoshopApp");
		if (!QDesktopServices::openUrl(help_url)) {
			qWarning() << "打开帮助文档失败!";
		}
	}
	else if (action == action_aboutme) {
		// 创建关于作者的对话框
		QDialog aboutDialog;
		aboutDialog.raise();
		aboutDialog.setWindowTitle("关于作者");

		// 添加作者信息标签
		QLabel authorLabel("作者：🌈Yuelo");
		authorLabel.setAlignment(Qt::AlignCenter);

		// 添加版权信息标签
		QLabel copyrightLabel("版权所有 © 2023 🌈Yuleo");
		copyrightLabel.setAlignment(Qt::AlignCenter);

		// 添加主页网址标签
		QLabel homepageLabel("<a href='https://www.helloylh.com'>点击打开我的主页</a>");
		homepageLabel.setAlignment(Qt::AlignCenter);
		homepageLabel.setOpenExternalLinks(true); // 允许打开外部链接

		// 添加关闭按钮
		QPushButton closeButton("关闭");
		QObject::connect(&closeButton, &QPushButton::clicked, &aboutDialog, &QDialog::close);

		// 创建垂直布局来组织内容
		QVBoxLayout layout;
		layout.addWidget(&authorLabel);
		layout.addWidget(&homepageLabel);
		layout.addWidget(&copyrightLabel);
		layout.addWidget(&closeButton);

		// 将布局设置为对话框的布局
		aboutDialog.setLayout(&layout);

		// 显示关于作者的对话框
		aboutDialog.exec();
	}	
}

void Widget::on_pushButton_next_clicked()
{
	work_prevIndex = work_currentIndex;
	work_currentIndex++;
	if (work_currentIndex >= work_files.count()) {
		work_currentIndex = 0;
		work_prevIndex = work_files.count() - 1;
	}
	on_checkBox_LeaveAutoSave_clicked();
	work_cutImage();
}

void Widget::on_pushButton_prev_clicked()
{
	work_prevIndex = work_currentIndex;
	work_currentIndex--;
	if (work_currentIndex < 0) {
		work_prevIndex = 0;
		work_currentIndex = work_files.count() - 1;
	}
	on_checkBox_LeaveAutoSave_clicked();
	work_cutImage();
}

void Widget::reload_Resources_ScrollArea(const QString& fileName, int mode)
{
	if (!mode) {
		layout_changeToNormal();
	}
	res->reset(fileName.toLocal8Bit().data());

	lab_img->setPixmap(QPixmap::fromImage(res->curr_img));

	scrollArea->takeWidget();
	scrollArea->setWidget(lab_img);

	//图片尺寸预调整
	scaledDelta = ori_scaledDelta = init_scaledImageOk();

	//更新数值
	for (auto& x : Opts) {
		x->initialize();
	}
	clearAllWidgetValue();
	//所有按钮置为未选中状态
	for (auto& btnGps : btngroups) {
		btnGps->setExclusive(false);
		for (auto& x : btnGps->buttons()) {
			x->setChecked(false);
		}
		btnGps->setExclusive(true);
	}
}

void Widget::on_checkBox_LeaveAutoSave_clicked()
{
	//在切换页面的时候弹出保存提示
	if (cbx_work_autoSave->isChecked()) {
		on_action_saveFile_triggered();
	}
}

bool Widget::loadImagesFormFloder(const QString& floderPath)
{
	QDir dir(floderPath);
	QStringList filters;
	filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp"; // 支持的图片格式
	QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files, QDir::Name);
	if (fileInfoList.empty()) {
		return false;
	}
	for (auto& x : fileInfoList) {
		if (x.exists()) {
			work_files.push_back(x.absoluteFilePath());
		}
	}
	work_currentIndex = work_prevIndex = 0;
	return true;
}

void Widget::choice_buttonGroupsBtns()
{
	if (mode) {
		//先恢复再保存
		savePoint();
		clearAllWidgetValue();
	}
	else {
		restore_cutOperation();
	}
}

void Widget::restore_cutOperation()
{
	clearAllWidgetValue();

	updateFromIntermediate();

}

void Widget::on_action_changeMode_triggered(){
	//点击此开始创作模式
	//图片清除，重新开始，所有滑块归零
	if (!mode) {
		this->mode = true;
		statusLab->setText(tr("混合加工模式"));
	}
	else {
		mode = false;
		statusLab->setText(tr("默认模式"));
	}
	clearAllWidgetValue();
	//数据清空
	updateFromIntermediate();
}

void Widget::on_action_undo_triggered()
{
	//仅仅在创作者模式下生效：触发时会将图片置回到未操作前的位置
	if (mode) {
		returnPoint();
		update_Image(ori_scaledDelta);
		//清除滑块的值
		setIndexPageWidgetValue();
	}
	else {
		on_action_allRestore_triggered();
	}
}


void Widget::savePoint()
{
	//每次切换操作时自动保存当前图片作为保存点
	res->flash_mt = res->curr_mt.clone();
	undo_sta.push(res->curr_mt);
}

void Widget::returnPoint()
{
	//读取存档保存点
	//获取当前栈顶的mat
	if (!undo_sta.empty()) {
		//修改当前显示的mt与图片
		res->curr_mt = undo_sta.top();
		res->curr_img = Mat2QImage(res->curr_mt);
		undo_sta.pop();
	}
}

void Widget::hideAllDialog(QDialog* currDialog)
{
	for (auto& x : all_dlgs) {
		if (x && x != currDialog) {
			x->accept();
		}
	}
}

void Widget::setIndexPageWidgetValue(int index)
{
	//传递默认的index，则选择当前页
	if (index == -1) {
		clearAllWidgetValue();
		return;
	}

	//否则就是第index页
	if (!all_dlgs[index]) {
		return;
	}
	QList<QWidget*> Opts = all_dlgs[index]->findChildren<QWidget*>();

	for (auto& x : Opts) {
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
	for (int i = 0; i < all_dlgs.count(); i++) { // (0 1 2 3) 4 5 6 7 8
		setIndexPageWidgetValue(i);
	}
}

void Widget::updateFromIntermediate()
{
	res->curr_mt = res->inter_mt.clone();
	res->flash_mt = res->inter_mt.clone();
	res->curr_img = Mat2QImage(res->curr_mt);

	//恢复完美缩放
	update_Image(ori_scaledDelta);

	//重置缩放比例
	scaledDelta = ori_scaledDelta;
}

void Widget::updateFromRoot()
{
	res->inter_mt = res->root_mt.clone();
	updateFromIntermediate();
}

void Widget::layout_changeToWork()
{
	btn_work_next = new QPushButton("下一页");
	btn_work_prev = new QPushButton("上一页");
	cbx_work_autoSave = new QCheckBox("离开时自动保存");

	btn_work_layout = new QHBoxLayout;
	btn_work_layout->addWidget(btn_work_prev);
	btn_work_layout->addWidget(btn_work_next);
	btn_work_layout->addWidget(cbx_work_autoSave);

	// 创建主布局
	QVBoxLayout* mainLayout = new QVBoxLayout;
	scrollArea->setFixedSize(SCROLLAREA_WIDTH - 50, SCROLLAREA_HEIGHT - 50);
	mainLayout->addWidget(scrollArea);
	mainLayout->addLayout(btn_work_layout);

	// 创建主窗口
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);
	this->setCentralWidget(centralWidget);
}

void Widget::layout_changeToNormal()
{
	if (!scrollArea) {
		return;
	}
	scrollArea->setFixedSize(SCROLLAREA_WIDTH, SCROLLAREA_HEIGHT);
	this->setCentralWidget(scrollArea);
}

void Widget::createAction()
{
	//关闭窗口
	action_exit = new QAction(tr("退出"), this);
	action_exit->setStatusTip(tr("退出程序"));
	action_exit->setShortcut(tr("Ctrl+E"));
	connect(action_exit, &QAction::triggered, this,&Widget::on_action_exit_triggered);

	//打开文件
	action_open = new QAction(tr("打开文件"), this);
	action_open->setStatusTip(tr("选择并且打开一个图片"));
	action_open->setShortcut(tr("Ctrl+O"));
	connect(action_open, &QAction::triggered, this,&Widget::on_action_openFile_triggered);

	//打开文件夹
	action_works = new QAction(tr("打开文件夹/工作区"), this);
	action_works->setStatusTip(tr("选择并且打开文件夹作为当前工作区域"));
	action_works->setShortcut(tr("Ctrl+F"));
	connect(action_works, &QAction::triggered, this, &Widget::on_action_openWorks_triggered);

	//保存图片
	action_save = new QAction(tr("保存图片"), this);
	action_save->setStatusTip(tr("保存此图片"));
	action_save->setShortcut(tr("Ctrl+S"));
	connect(action_save, &QAction::triggered, this, &Widget::on_action_saveFile_triggered);

	//重置图片
	action_restore = new QAction(tr("重置图片"), this);
	action_restore->setStatusTip(tr("重置此图片，取消所有加工"));
	action_restore->setIcon(QIcon("../resource/assert/restore.png"));
	action_restore->setShortcut(tr("Ctrl+Shift+Z"));
	connect(action_restore, &QAction::triggered, this, &Widget::on_action_allRestore_triggered);

	//开始制作模式
	action_begin = new QAction(tr("图片加工"),this);
	action_begin->setStatusTip(tr("图片加工模式，可以混合修改图片"));
	action_begin->setIcon(QIcon("../resource/assert/begin.png"));
	action_begin->setCheckable(true);
	connect(action_begin, &QAction::triggered, this, &Widget::on_action_changeMode_triggered);

	//撤销
	action_return = new QAction(tr("撤销"), this);
	action_return->setShortcut(tr("Ctrl+Z"));
	action_return->setStatusTip(tr("撤销此操作"));
	action_return->setIcon(QIcon("../resource/assert/return.png"));
	connect(action_return, &QAction::triggered, this, &Widget::on_action_undo_triggered);

	//预览图点击确定
	action_previewOk = new QAction(tr("确定"),this);
	action_previewOk->setIcon(QIcon("../resource/assert/previewOk.png"));
	action_previewOk->setStatusTip(tr("确定操作"));
	connect(action_previewOk, &QAction::triggered, this, &Widget::on_action_previewToNormal_triggered);

	//打开简单绘图板
	action_draw = new QAction(tr("绘画"), this);
	action_draw->setIcon(QIcon("../resource/assert/draw.png"));
	action_draw->setStatusTip(tr("绘图操作"));
	connect(action_draw, &QAction::triggered, this,&Widget::on_action_drawBoard_triggered);

	//轮廓操作时 选择颜色 自动更新
	colorDialog = new QColorDialog(this);
	connect(colorDialog, &QColorDialog::currentColorChanged,
		this, &Widget::on_colorDialog_choice_triggered);

	//颜色转换
	action_cvtColor_group = new QActionGroup(this);
	action_cvtColor_group->addAction(action_ori = new QAction("转换为原图", this));
	action_cvtColor_group->addAction(action_hls = new QAction("转换为HLS格式",this));
	action_cvtColor_group->addAction(action_hsv = new QAction("转换为HSV格式", this));
	action_cvtColor_group->addAction(action_rgb = new QAction("转换为RGB格式", this));
	action_cvtColor_group->addAction(action_lab = new QAction("转换为LAB格式", this));
	connect(action_cvtColor_group, &QActionGroup::triggered, this,&Widget::on_actionGroup_cvtColor_triggered);

	//图片旋转菜单
	action_rotate_group = new QActionGroup(this);
	action_rotate_group->addAction(action_right90 = new QAction(tr("顺时针旋转90度"), this));
	action_rotate_group->addAction(action_right180 = new QAction(tr("顺时针旋转180度"), this));
	action_rotate_group->addAction(action_right270 = new QAction(tr("逆时针旋转90度"),this));
	connect(action_rotate_group, &QActionGroup::triggered, this, &Widget::on_actionGroup_rotate_triggered);


	//图片翻转菜单
	action_flip_group = new QActionGroup(this);
	action_flip_group->addAction(action_flip0 = new QAction(tr("垂直翻转"),this));
	action_flip_group->addAction(action_flip1 = new QAction(tr("水平翻转"), this));
	action_flip_group->addAction(action_flip_1 = new QAction(tr("垂直水平翻转"), this));
	connect(action_flip_group, &QActionGroup::triggered, this, &Widget::on_actionGroup_flip_triggered);

	//图片对比度提高
	action_mark = new QAction(tr("图像对比度提高"),this);
	connect(action_mark, &QAction::triggered, this, [=]() {
		img_base->onTriggered_picture_mask();
		});

	//灰度直方图
	action_hist = new QAction(tr("计算灰度直方图"), this);
	connect(action_hist, &QAction::triggered, this, [=]() {
		img_base->drawGrayHist("image histogram");
		});

	action_fileInfo = new QAction(tr("图片属性"), this);
	connect(action_fileInfo, &QAction::triggered,this,&Widget::on_action_fileInfo_triggered);

	//扩展
	action_light = new QAction(tr("亮色"), this);
	connect(action_light, &QAction::triggered, this, [=]() {
		config.win_theme = "light";
		QMessageBox::information(this, tr("提示"), tr("切换主题成功！请重启程序"));
		});

	action_dark = new QAction(tr("暗色"), this);
	connect(action_dark, &QAction::triggered, this, [=]() {
		config.win_theme = "dark";
		QMessageBox::information(this, tr("提示"), tr("切换主题成功！请重启程序"));
		});

	
	action_jie = new QAction(tr("工作图片截取"), this);
	connect(action_jie, &QAction::triggered, this, &Widget::on_action_jie_triggered);

	action_capture = new QAction(tr("屏幕截图"), this);
	action_capture->setShortcut(tr("Ctrl+Alt+A"));
	connect(action_capture, &QAction::triggered, this, &Widget::on_action_capture_triggered);

	//帮助菜单
	action_help_group = new QActionGroup(this);
	action_help_group->addAction(action_help = new QAction(tr("查看帮助"), this));
	action_help_group->addAction(action_aboutme = new QAction(tr("关于作者"), this));
	connect(action_help_group, &QActionGroup::triggered, this, &Widget::on_actionGroup_help_triggered);

}

void Widget::createMenu()
{
	//图片上下文菜单
	//此处跟随滑动区域，滑动区域在整个窗口中作为主窗口不会消失。
	context_menu = new QMenu(this); 
	context_menu->addAction(action_save);
	context_menu->addAction(action_exit);
	context_menu->addAction(action_restore);
	context_menu->addAction(action_fileInfo);

	//菜单栏
	menu_file = menuBar()->addMenu(tr("&文件"));
	menu_file->addAction(action_open);
	menu_file->addAction(action_works);
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

	menu_func = menuBar()->addMenu(tr("功能"));
	menu_func->addAction(action_hist);
	QMenu* menu_equalize = new QMenu("均衡化图像",this);
	menu_equalize->addAction("灰度图", this, [=]() {img_base->showEqualizedGrayImage(); });
	menu_equalize->addAction("彩色图", this, [=]() {img_base->showEqualizedBGRImage(); });
	menu_func->addMenu(menu_equalize);
	QMenu* menu_contrastLinear = new QMenu("对比度线性展宽", this);
	menu_contrastLinear->addAction("灰度图", this, [=]() {img_base->showContrastLinearBroaden(); });
	menu_contrastLinear->addAction("彩色图", this, [=]() {img_base->showBGRContrastLinearBroaden(); });
	menu_func->addMenu(menu_contrastLinear);
	menu_func->addSeparator();
	menu_func->addAction(action_fileInfo); //图片属性信息

	//扩展
	menu_tools = menuBar()->addMenu(tr("扩展"));
	QMenu* menu_theme = new QMenu(tr("主题"),this);
	menu_theme->addAction(action_light);
	menu_theme->addAction(action_dark);
	menu_tools->addMenu(menu_theme);
	menu_tools->addAction(action_jie); //图片截取
	menu_tools->addAction(action_capture); //屏幕截图

	//帮助
	menu_help = menuBar()->addMenu(tr("帮助"));
	menu_help->addAction(action_help);
	menu_help->addAction(action_aboutme);


}

void Widget::createToolBar()
{
	toolbar1 = addToolBar(tr("file"));
	toolbar1->addAction(action_begin);
	toolbar1->addAction(action_restore);
	toolbar1->addAction(action_return);
	toolbar1->addAction(action_previewOk);
	toolbar1->addAction(action_draw);
}

void Widget::createToolBox()
{
	//-----------------模糊操作-----------------------------
	btngroups.push_back(btngroup_blur = new QButtonGroup(this));
	btngroup_blur->setExclusive(true);
	//连接信号
	connect(btngroup_blur, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		Object* op = blur;
		on_buttonGroup_everyOpeartions_choice(op,btngroup_blur,btn);
		});

	
	QGridLayout* grid_blur = new QGridLayout;
	QWidget* a1 = createToolBtnItemWidget("均值模糊", BLUR::Average, "../resource/assert/avg.png");
	a1->setStatusTip(tr("均值滤波：取周围像素平均值进行滤波，可以帮助消除图像尖锐噪声，实现图像平滑，模糊等功能"));
	grid_blur->addWidget(a1, 0, 0);

	QWidget* a2 = createToolBtnItemWidget("高斯模糊", BLUR::Gaussian, "../resource/assert/gaussian.png");
	a2->setStatusTip(tr("高斯滤波：线性平滑滤波，适用于消除高斯噪声，广泛应用于图像处理的减噪过程。"));
	grid_blur->addWidget(a2,0,1);
	QWidget* a3 = createToolBtnItemWidget("中值模糊", BLUR::Median, "../resource/assert/zhong.png");
	a3->setStatusTip(tr("中值滤波：非线性平滑滤波，它将每一像素点的灰度值设置为该点某邻域窗口内的所有像素点灰度值的中值。"));
	grid_blur->addWidget(a3,1,0);
	QWidget* a4 = createToolBtnItemWidget("双边滤波", BLUR::Bilateral, "../resource/assert/shuangbian.png");
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
	connect(btngroup_threshold, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		Object* op = threshold;
		on_buttonGroup_everyOpeartions_choice(op, btngroup_threshold, btn);
		});

	QGridLayout* grid_threshold = new QGridLayout;
	QWidget* b1 = createToolBtnItemWidget("二进制", THRESHOLD::Binary, "../resource/assert/2.png");
	b1->setStatusTip(tr("二进制阈值化：大变最大，小变0"));
	grid_threshold->addWidget(b1, 0, 0);

	QWidget* b2 = createToolBtnItemWidget("反二进制", THRESHOLD::Binary_inv, "../resource/assert/f2.png");
	b2->setStatusTip(tr("反二进制阈值化：大变0，小变最大"));
	grid_threshold->addWidget(b2, 0, 1);

	QWidget* b3 = createToolBtnItemWidget("截断", THRESHOLD::Trunc, "../resource/assert/jieduan.png");
	b3->setStatusTip(tr("截断阈值化：大变阈值，小不变"));
	grid_threshold->addWidget(b3, 1, 0);

	QWidget* b4 = createToolBtnItemWidget("阈值化为零", THRESHOLD::Tozero, "../resource/assert/0.png");
	b4->setStatusTip(tr("阈值化为0：大不变，小变0"));
	grid_threshold->addWidget(b4, 1, 1);

	QWidget* b5 = createToolBtnItemWidget("反阈值化为零", THRESHOLD::Tozero_inv, "../resource/assert/f0.png");
	b5->setStatusTip(tr("反阈值化为0：大变0，小不变"));
	grid_threshold->addWidget(b5, 2, 0);

	grid_threshold->setRowStretch(3, 1); // 添加一行拉伸因子
	grid_threshold->setColumnStretch(2, 1); // 添加一列拉伸因子

	QWidget* widget_threshold = new QWidget;
	widget_threshold->setLayout(grid_threshold);


	//-----------------形态化操作-----------------------------
	btngroups.push_back(btngroup_form = new QButtonGroup(this));
	btngroup_form->setExclusive(true);
	//连接信号
	connect(btngroup_form, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		Object* op = morphology;
		on_buttonGroup_everyOpeartions_choice(op, btngroup_form, btn);
		});

	QGridLayout* grid_form = new QGridLayout;
	QWidget* c1 = createToolBtnItemWidget("膨胀", FORM::Erode, "../resource/assert/pengzhang.png");
	c1->setStatusTip(tr("膨胀：取每个位置领域内最大值，膨胀后输出图像的总体亮度的平均值比起原图会有所升高"));
	grid_form->addWidget(c1, 0, 0);

	QWidget* c2 = createToolBtnItemWidget("腐蚀", FORM::Dilate, "../resource/assert/fushi.png");
	c2->setStatusTip(tr("腐蚀：取每个位置领域内最小值，腐蚀后输出图像的总体亮度的平均值比起原图会有所降低"));
	grid_form->addWidget(c2, 0, 1);

	QWidget* c3 = createToolBtnItemWidget("开运算", FORM::Open, "../resource/assert/kai.png");
	c3->setStatusTip(tr("开运算：先腐蚀再膨胀，可在纤细点出分离物体。有助于消除噪音"));
	grid_form->addWidget(c3, 1, 0);

	QWidget* c4 = createToolBtnItemWidget("闭运算", FORM::Close, "../resource/assert/bi.png");
	c4->setStatusTip(tr("闭运算：先膨胀后腐蚀，可以排除前景对象中的小孔或对象上的小黑点"));
	grid_form->addWidget(c4, 1, 1);

	QWidget* c5 = createToolBtnItemWidget("梯度", FORM::Gradient, "../resource/assert/tidu.png");
	c5->setStatusTip(tr("梯度运算：膨胀图与腐蚀图之差，可以用于保留目标物体的边缘轮廓"));
	grid_form->addWidget(c5, 2, 0);

	QWidget* c6 = createToolBtnItemWidget("顶帽", FORM::Tophat, "../resource/assert/dingmao.png");
	c6->setStatusTip(tr("顶帽运算：	原图与开运算图之差，分离比邻近点亮的斑块，用于突出原图像中比周围亮的区域"));
	grid_form->addWidget(c6, 2, 1);

	QWidget* c7 = createToolBtnItemWidget("黑帽", FORM::Tophat, "../resource/assert/dingmao.png");
	c7->setStatusTip(tr("黑帽运算：闭运算图与原图差，分离比邻近点暗的斑块，突出原图像中比周围暗的区域"));
	grid_form->addWidget(c7, 3, 0);

	grid_form->setRowStretch(4, 10);
	grid_form->setColumnStretch(1, 10);

	QWidget* widget_from = new QWidget;
	widget_from->setLayout(grid_form);


	//-----------------连通区域分析操作-----------------------------
	btngroups.push_back(btngroup_connected = new QButtonGroup(this));
	btngroup_connected->setExclusive(true);//互斥

	connect(btngroup_connected, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		Object* op = connected;
		on_buttonGroup_everyOpeartions_choice(op, btngroup_connected, btn);
		});
	
	QGridLayout* gird_connected = new QGridLayout;

	QWidget* d1 = createToolBtnItemWidget(tr("连通区域分析①"), CONNECTED::CONNECTED_TYPE1, "../resource/assert/liantongkuai.png");
	d1->setStatusTip(tr("连通区域：找到图片之间的连通块，并且用不同颜色标记。"));
	gird_connected->addWidget(d1,0,0);

	QWidget* d2 = createToolBtnItemWidget(tr("连通区域分析②"), CONNECTED::CONNECTED_TYPE2, "../resource/assert/liantongkuais.png");
	d2->setStatusTip(tr("连通区域：找到图片之间的连通块，并且使用方框进行标记。"));
	gird_connected->addWidget(d2,1,0);
	gird_connected->setRowStretch(4, 10);
	gird_connected->setColumnStretch(1, 10);

	QWidget* widget_connected = new QWidget;
	widget_connected->setLayout(gird_connected);

	//-----------------轮廓绘制操作-----------------------------
	btngroups.push_back(btngroup_contours = new QButtonGroup(this));
	btngroup_contours->setExclusive(true);

	connect(btngroup_contours, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		Object* op = contours;
		on_buttonGroup_everyOpeartions_choice(op, btngroup_contours, btn);
		});

	QGridLayout* gird_contours = new QGridLayout;

	QWidget* e1 = createToolBtnItemWidget(tr("轮廓绘制操作"), CONTOURS::CONTOURS_TYPE1, "../resource/assert/lunkuo.png");
	e1->setStatusTip(tr("轮廓绘制：绘制图像的轮廓信息"));
	gird_contours->addWidget(e1, 0, 0);

	gird_contours->setRowStretch(4, 10);
	gird_contours->setColumnStretch(1, 10);

	QWidget* widget_contours = new QWidget;
	widget_contours->setLayout(gird_contours);

	//-----------------图像效果增强------------------------------
	btngroups.push_back(btngroup_show = new QButtonGroup(this));
	btngroup_show->setExclusive(true);

	connect(btngroup_show, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		Object* op = showeffect;
		on_buttonGroup_everyOpeartions_choice(op, btngroup_show, btn);
		});

	QGridLayout* gird_effect = new QGridLayout;

	QWidget* f1 = createToolBtnItemWidget(tr("亮度调整"), SHOW::LIGHT, "../resource/assert/light.png");
	f1->setStatusTip(tr("图像亮度调整：增强图像的亮度"));
	gird_effect->addWidget(f1, 0, 0);

	QWidget* f2 = createToolBtnItemWidget(tr("γ矫正"),SHOW::GAMMA, "../resource/assert/gamma.png");
	f2->setStatusTip(tr("γ矫正"));
	gird_effect->addWidget(f2, 0, 1);

	gird_effect->setRowStretch(4, 10);
	gird_effect->setColumnStretch(1, 10);

	QWidget* widget_effect= new QWidget;
	widget_effect->setLayout(gird_effect);

	//-----------------创建ToolBox-----------------
	toolbox_side = ToolBoxBuilder()
		.setMinimumWidth(200)
		.setMaximumWidth(200)
		.setFrameShape(QFrame::StyledPanel)
		.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored)
		.addItem(widget_blur, "图像模糊操作")
		.addItem(widget_threshold, "图像阈值操作")
		.addItem(widget_from, "图像形态化操作")
		.addItem(widget_connected, "图像连通分析")
		.addItem(widget_contours, "图像轮廓分析")
		.addItem(widget_effect, "图像效果增强")
		.setCurrentIndex(0)
		.create(this);

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
	statusLab = new QLabel("默认模式");
	statusLab->setObjectName("statusBar_lab");
	statusBar()->addWidget(statusLab);
}

void Widget::create_GUIAvgBlur()
{
	//----------------------------------------------------------
	ls_slider_blur.resize(3);
	std::function<void(int)> funAvgBlur_slider1 = [=](int value) {
		if (ls_slider_blur[1]->value() == ls_slider_blur[1]->minimum() && ls_slider_blur[2]->value() == ls_slider_blur[2]->minimum()) {
			blur->anchorX = blur->anchorY = blur->avg_Ksize / 2;
		}
		blur->onTriggered_slider1_valueChange_avgBlur(value);
	};

	std::function<void(int)> funAvgBlur_slider2 = [=](int value) {
		blur->onTriggered_slider2_valueChange_avgBlur(value);
	};

	std::function<void(int)> funAvgBlur_slider3 = [=](int value) {
		blur->onTriggered_slider3_valueChange_avgBlur(value);
	};

	dlg_avgBlur = new QDialog;
	all_dlgs[0] = dlg_avgBlur;
	dlg_avgBlur->setWindowTitle(tr("均值滤波"));
	dlg_avgBlur->setLayout(
		createDialog_nSlider_GUItemplate<int, Blur*>(
			ls_slider_blur,
			QList<int>() << 1 << 1 << 1,
			QList<int>() << 30 << 30 << 30,
			QList<int>() << 1 << 1 << 1,
			QList<QString>() << "Kernel_slider" << "x_slider" << "y_slider",
			QList<QString>() << "kernel" << "sigmaX" << "sigmaY",
			QList< std::function<void(int)>>() << funAvgBlur_slider1 << funAvgBlur_slider2 << funAvgBlur_slider3,
			true, "\\d+\\s\\d+\\s\\d+", "KSize X Y", &blur)
	);
}

void Widget::create_GUIGaussian()
{
	//----------------------------------------------------------
	//高斯滤波
	ls_slider_gaussian.resize(3);
	std::function<void(int)> funGaussian_slider1 = [=](int value) {
		if (value % 2 == 0) {
			value += 1;
		}
		if (ls_slider_gaussian[1]->value() == ls_slider_gaussian[1]->minimum() && ls_slider_gaussian[2]->value() == ls_slider_gaussian[2]->minimum()) {
			blur->sigmaX = blur->sigmaY = blur->gas_Ksize / 2;
		}
		blur->onTriggered_slider1_valueChange_gaussianBlur(value);
	};

	std::function<void(int)> funGaussian_slider2 = [=](int value) {
		blur->onTriggered_slider2_valueChange_gaussianBlur(value);
	};

	std::function<void(int)> funGaussian_slider3 = [=](int value) {
		blur->onTriggered_slider3_valueChange_gaussianBlur(value);
	};

	dlg_gauss = new QDialog;
	all_dlgs[1] = dlg_gauss;
	dlg_gauss->setWindowTitle(tr("高斯滤波"));
	dlg_gauss->setLayout(
		createDialog_nSlider_GUItemplate<int, Blur*>(
			ls_slider_gaussian,
			QList<int>() << 1 << 1 << 1,
			QList<int>() << 41 << 41 << 40,
			QList<int>() << 1 << 1 << 1,
			QList<QString>() << "Kernel_slider" << "x_slider" << "y_slider",
			QList<QString>() << "kernel" << "sigmaX" << "sigmaY",
			QList< std::function<void(int)>>() << funGaussian_slider1 << funGaussian_slider2 << funGaussian_slider3,
			true, "\\d+\\s\\d+\\s\\d+", "KSize X Y", &blur)
	);
}

void Widget::create_GUIMedian()
{
	//----------------------------------------------------------
	//中值滤波
	ls_slider_median.resize(1);
	std::function<void(int)> funMedian = [=](int value) {
		if (value % 2 == 0)
			value += 1;
		blur->onTriggered_slider_valueChange_medianBlur(value); };

	dlg_median = new QDialog;
	all_dlgs[2] = dlg_median;
	dlg_median->setWindowTitle(tr("中值滤波"));
	dlg_median->setLayout(
		createDialog_nSlider_GUItemplate<int, Blur*>(
			ls_slider_median,
			QList<int>() << 1,
			QList<int>() << MEDIAN_BLUR_MAX,
			QList<int>() << 2,
			QList<QString>() << "median_slider",
			QList<QString>() << "KSize",
			QList< std::function<void(int)>>() << funMedian,
			true, "\\d+", "KSize", &blur)
	);
}

void Widget::create_GUIBilateral()
{
	//----------------------------------------------------------
	//双边滤波
	ls_slider_bilateral.resize(3);
	std::function<void(int)> funBilateral_slider1 = [=](int value) {
		blur->onTriggered_slider1_valueChange_bilateralBlur(value);
	};

	std::function<void(int)> funBilateral_slider2 = [=](int value) {
		blur->onTriggered_slider2_valueChange_bilateralBlur(value);
	};

	std::function<void(int)> funBilateral_slider3 = [=](int value) {
		blur->onTriggered_slider3_valueChange_bilateralBlur(value);
	};
	dlg_Bilateral = new QDialog;
	all_dlgs[3] = dlg_Bilateral;
	dlg_Bilateral->setWindowTitle(tr("双边滤波"));
	dlg_Bilateral->setLayout(
		createDialog_nSlider_GUItemplate<int, Blur*>(
			ls_slider_bilateral,
			QList<int>() << 1 << 0 << 0,
			QList<int>() << 50 << 120 << 150,
			QList<int>() << 1 << 1 << 1,
			QList<QString>() << "bilateral_slider" << "sigma_Color" << "sigma_Space",
			QList<QString>() << "ksize" << "sigmaX" << "sigmaY",
			QList< std::function<void(int)>>() << funBilateral_slider1 << funBilateral_slider2 << funBilateral_slider3,
			true, "\\d+\\s\\d+\\s\\d+", "bin_d sigmaColor sigmaSpace", &blur)
	);
}

void Widget::create_GUIThreshold()
{
	//----------------------------------------------------------
	//阈值化
	ls_slider_threshold.resize(2);
	std::function<void(int)> funThreshold_slider1 = [=](int value) {
		threshold->onTriggered_slider1_valueChanged_thresholdValue(value);
	};

	std::function<void(int)> funThreshold_slider2 = [=](int value) {
		threshold->onTriggered_slider2_valueChanged_maxValue(value);
	};

	dlg_threshold = new QDialog;
	all_dlgs[4] = dlg_threshold;
	dlg_threshold->setWindowTitle(tr("图像的阈值化"));
	dlg_threshold->setLayout(
		createDialog_nSlider_GUItemplate<int, Threshold*>(
			ls_slider_threshold,
			QList<int>() << 0 << 0,
			QList<int>() << 255 << 255,
			QList<int>() << 1 << 1,
			QList<QString>() << "threshold_value" << "maxValue",
			QList<QString>() << "threshold" << "maxval",
			QList< std::function<void(int)>>() << funThreshold_slider1 << funThreshold_slider2,
			true, "\\d+\\s\\d+", "threshold_value maxVal", &threshold)
	);

}

void Widget::create_GUIMorphology()
{
	//----------------------------------------------------------
	//形态学
	ls_slider_morphology.resize(3);
	std::function<void(int)> funMorphology_slider1 = [=](int value) {
		morphology->onTriggered_slider1_valueChanged_kernel(value);
	};

	std::function<void(int)> funMorphology_slider2 = [=](int value) {
		morphology->onTriggered_slider2_valueChanged_anchorX(value);
	};

	std::function<void(int)> funMorphology_slider3 = [=](int value) {
		morphology->onTriggered_slider3_valueChanged_anchorY(value);
	};

	dlg_morphology = new QDialog;
	all_dlgs[5] = dlg_morphology;
	dlg_morphology->setWindowTitle(tr("图像的形态学操作"));
	dlg_morphology->setLayout(
		createDialog_nSlider_GUItemplate<int, Morphology*>(
			ls_slider_morphology,
			QList<int>() << 1 << 1 << 1,
			QList<int>() << 50 << 20 << 20,
			QList<int>() << 1 << 1 << 1,
			QList<QString>() << "Kernal_mor" << "X_mor" << "Y_mor",
			QList<QString>() << "Kernel" << "anchorX" << "anchorY",
			QList< std::function<void(int)>>() << funMorphology_slider1 << funMorphology_slider2 << funMorphology_slider3,
			true, "\\d+\\s\\d+\\s\\d+\\s\\d+", "Kernel X Y iters", &morphology)
	);
}

void Widget::create_GUIConnected()
{
	//----------------------------------------------------------
	//连通性
	ls_combox_connected.resize(2);
	std::function<void(int)> funConnected_combo1 = [=](int index) {
		connected->onTriggered_Comb2_currentTextChanged_ccltype(index);
	};

	std::function<void(int)> funConnected_combo2 = [=](int index) {
		connected->onTriggered_Comb2_currentTextChanged_ccltype(index);
	};

	dlg_connected = new QDialog;
	all_dlgs[6] = dlg_connected;
	dlg_connected->setWindowTitle(tr("图像的连通性操作①"));
	QStringList com1 = { "8" , "4" };
	QStringList com2 = { "default","CC_WU","CCL_GRANA","CCL_BOLELLI","CCL_SAUF","CCL_BBDT","CCL_SPAGHETTI" };
	dlg_connected->setLayout(
		createDialog_nComBox_GUItemplate(
			ls_combox_connected,
			QList<QStringList>() << com1 << com2,
			QList<QString>() << "connectivity" << "ccltype",
			QList<QString>() << "邻域" << "联通算法",
			QList< std::function<void(int)>>() << funConnected_combo1 << funConnected_combo2
		)
	);
}

void Widget::create_GUIContours()
{
	//----------------------------------------------------------
	//轮廓
	ls_combox_contours.resize(5);
	std::function<void(int)> funContours_combo1 = [=](int index) {
		contours->onTriggered_Comb1_currentTextChanged_contoursMode(index);
	};

	std::function<void(int)> funContours_combo2 = [=](int index) {
		contours->onTriggered_Comb2_currentTextChanged_contoursMethod(index);
	};

	std::function<void(int)> funContours_combo3 = [=](int index) {
		contours->onTriggered_Comb3_currentTextChanged_contoursThick(index);
	};

	std::function<void(int)> funContours_combo4 = [=](int index) {
		colorDialog->setMinimumSize(375, 375);
		colorDialog->setMaximumSize(653, 498);
		colorDialog->setGeometry(810, 306, 375, 375);
		colorDialog->show(); //弹出颜色框
	};

	std::function<void(int)> funContours_combo5 = [=](int index) {
		contours->onClicked_btn_convexHull();
	};

	dlg_contours = new QDialog;
	all_dlgs[7] = dlg_contours;
	dlg_contours->setWindowTitle(tr("图像的轮廓检测操作"));
	QStringList contours_com1 = { "RETR_EXTERNAL", "RETR_LIST","RETR_CCOMP","RETR_TREE" };
	QStringList contours_com2 = { "CHAIN_APPROX_NONE", "CHAIN_APPROX_SIMPLE","CHAIN_APPROX_TC89_L1","CHAIN_APPROX_TC89_KCOS" };
	QStringList contours_com3;
	for (int i = 1; i <= 10; i++) {
		contours_com3.append(QString("%1").arg(i));
	}
	QStringList contours_com4 = { "更换颜色" };
	QStringList contours_com5 = { "绘制图像凸包" };
	dlg_contours->setLayout(
		createDialog_nComBox_GUItemplate(
			ls_combox_contours,
			QList<QStringList>() << contours_com1 << contours_com2 << contours_com3 << contours_com4 << contours_com5,
			QList<QString>() << "contours_mode" << "contours_method" << "contours_thick" << "coutours_color" << "coutours_FullHex",
			QList<QString>() << "轮廓检索模式" << "轮廓逼近方法" << "绘制线宽度" << "other" << "other",
			QList< std::function<void(int)>>() << funContours_combo1 << funContours_combo2 << funContours_combo3 << funContours_combo4 << funContours_combo5
		)
	);

}

void Widget::create_GUIbright()
{
	//-----------------------------------------------------
	//亮度调整
	ls_slider_light.resize(2);
	std::function<void(int)> funcLight = [=](int value) {
		ls_slider_light[1]->setValue(ls_slider_light[1]->minimum());
		showeffect->onTriggered_slider_valueChange_brighten(value);
	};
	std::function<void(int)> funcDark = [=](int value) {
		value = -value;
		ls_slider_light[0]->setValue(ls_slider_light[0]->minimum());
		showeffect->onTriggered_slider_valueChange_brighten(value);
	};


	dlg_bright = new QDialog;
	all_dlgs[8] = dlg_bright;
	dlg_bright->setWindowTitle(tr("图像亮度调整"));
	dlg_bright->setLayout(
		createDialog_nSlider_GUItemplate<int, Blur*>(
			ls_slider_light,
			QList<int>() << 1 << 1,
			QList<int>() << 100 << 100,
			QList<int>() << 1 << 1,
			QList<QString>() << "bright_light_value" << "bright_dark_value",
			QList<QString>() << "亮度增加" << "亮度降低",
			QList< std::function<void(int)>>() << funcLight << funcDark)
	);
}

void Widget::create_GUIgamma()
{
	//-----------------------------------------------------
	//gamma矫正
	ls_slider_gamma.resize(1);
	std::function<void(int)> funcGamma = [=](int value) {
		//将int映射为double
		double d_val = value * 1.0 / 10.0;
		showeffect->onTriggered_slider_valueChange_gamma(d_val);
	};

	dlg_gamma = new QDialog;
	all_dlgs[9] = dlg_gamma;
	dlg_gamma->setWindowTitle(tr("图像γ矫正"));
	dlg_gamma->setLayout(
		createDialog_nSlider_GUItemplate<int, Blur*>(
			ls_slider_gamma,
			QList<int>() << 1,
			QList<int>() << 50,
			QList<int>() << 1,
			QList<QString>() << "gamma_slider",
			QList<QString>() << "γ矫正",
			QList< std::function<void(int)>>() << funcGamma,
			true)
	);
}


void Widget::choice_GUI_create(int id)
{
	switch (id)
	{
	case 0:
		create_GUIAvgBlur();
		break;
	case 1:
		create_GUIGaussian();
		break;
	case 2:
		create_GUIMedian();
		break;
	case 3:
		create_GUIBilateral();
		break;
	case 4:
		create_GUIThreshold();
		break;
	case 5:
		create_GUIMorphology();
		break;
	case 6:
		create_GUIConnected();
		break;
	case 7:
		create_GUIContours();
		break;
	case 8:
		create_GUIbright();
		break;
	case 9:
		create_GUIgamma();
		break;
	default:
		break;
	}
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
	else if (belongsToEnum<SHOW>(id)) {
		btngroup_show->addButton(btn, id);	  //效果增强
	}
	

	QGridLayout* grid = new QGridLayout;
	grid->addWidget(btn, 0, 0, Qt::AlignHCenter);
	QLabel* textlab = new QLabel(text);
	textlab->setObjectName("function_tbtn_textLab");
	grid->addWidget(textlab, 1, 0, Qt::AlignHCenter);

	QWidget* wid = new QWidget;
	wid->setLayout(grid);

	return wid;
}

template <typename Type>
QHBoxLayout* Widget::create_Edit_hLayout(const QString& filter, const QString& text, Type* t)
{
	QLineEdit* edit = new QLineEdit;
	edit->setPlaceholderText(text);
	edit->setFixedWidth(edit->fontMetrics().boundingRect(edit->placeholderText()).width()+20);
	//设置验证器
	QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression(filter), edit);
	edit->setValidator(validator);
	connect(edit, &QLineEdit::returnPressed, this, [=]() {
		QList<QString> lStr = edit->text().split(" ");
		(*t)->onReturnPressed_Edit(lStr);
		});
	QPushButton* ok_btn = new QPushButton("确定");
	connect(ok_btn, &QPushButton::clicked, this, [=]() {
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

void Widget::work_cutImage()
{
	if (work_currentIndex >= 0 && work_currentIndex < work_files.count()) {
		reload_Resources_ScrollArea(work_files[work_currentIndex], 1);
	}
}

void Widget::update_Image(double f_scaledDelta, const QPointF& imgPos)
{
	//更新图片显示到完美缩放比例
	QPixmap t_pixmap = QPixmap::fromImage(res->curr_img);
	QPixmap scaledPixmap = t_pixmap.scaled(t_pixmap.size() * f_scaledDelta, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	lab_img->setPixmap(scaledPixmap);

	//检查是否需要启用滚动条
	bool needScrollbars = scaledPixmap.size().width() > scrollArea->size().width() || scaledPixmap.size().height() > scrollArea->size().height();
	scrollArea->setHorizontalScrollBarPolicy(needScrollbars ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(needScrollbars ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
}


template<typename T, typename Type>
QBoxLayout* Widget::createDialog_nSlider_GUItemplate(
	QList<QSlider*>& ls_slider,
	QList<T> low,
	QList<T> high,
	QList<T> step,
	QList< QString> objectName,
	QList< QString> lab_name,
	QList<std::function<void(int)>> slotFunction,
	bool edit,
	const QString& filter,
	const QString& text,
	Type* t
){
	QVBoxLayout* vlayout = new QVBoxLayout;
	for (int i = 0; i < ls_slider.size(); i++) {
		ls_slider[i] = new QSlider(Qt::Horizontal);
		ls_slider[i]->setRange(low[i], high[i]);
		ls_slider[i]->setSingleStep(step[i]);
		ls_slider[i]->setObjectName(objectName[i]);
		ls_slider[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		//传递一个槽函数
		connect(ls_slider[i], &QSlider::sliderMoved,
			this, slotFunction[i]);

		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->addWidget(new QLabel(lab_name[i]));
		hlayout->addWidget(ls_slider[i]);

		vlayout->addLayout(hlayout);
	}	
	//----------------------------
	//具有可输入功能，添加一个输入框
	if (edit) {
		vlayout->addSpacing(10);
		vlayout->addLayout(create_Edit_hLayout(filter, text, t));
	}
	return vlayout;
}

QBoxLayout* Widget::createDialog_nComBox_GUItemplate(
	QList<QComboBox*>& ls_combox,
	QList<QStringList> ls_item,
	QList< QString> objectName,
	QList< QString> lab_name,
	QList<std::function<void(int)>> slotFunction)
{
	QVBoxLayout* vlayout = new QVBoxLayout;
	for (int i = 0; i < ls_combox.size(); i++) {
		ls_combox[i] = new QComboBox;
		ls_combox[i]->setEditable(false);
		for (auto& item : ls_item[i]) {
			ls_combox[i]->addItem(item);
		}
		ls_combox[i]->setObjectName(objectName[i]);
		ls_combox[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		//传递一个槽函数
		connect(ls_combox[i], &QComboBox::activated,
			this, slotFunction[i]);

		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->addWidget(new QLabel(lab_name[i]));
		hlayout->addWidget(ls_combox[i]);

		vlayout->addLayout(hlayout);
	}
	return vlayout;
}

/*
-----------------------------------------------------------
*/
double Widget::init_scaledImageOk() {
	//如果图片的宽度和高度大于的大小，则需要缩小，直到两者都小于滑动区域的大小
	if (!res){
		return 1.0;
	}
	double wDelta = static_cast<double>((double)scrollArea->size().width() / (double)res->curr_img.size().width());
	double hDelta = static_cast<double>((double)scrollArea->size().height() / (double)res->curr_img.size().height());
	auto t_scaledDelta = qMin(wDelta, hDelta);

	update_Image(t_scaledDelta);

	return t_scaledDelta;
}