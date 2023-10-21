#include "Widget/widget_include_files.h"
#include "opencv2/core/utils/logger.hpp"
#include "ui_mainwindow.h"
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
/**
 * @brief Construct a new Widget:: Widget object
 * @param  parent           My Param doc
 */
Widget::Widget(QWidget* parent)
	:QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,res(new Res("resource/Images/113.png", this))
{
    ui->setupUi(this);
    init_readJson();		//读取配置文件
    init_WidgetInfo();		//设置主窗口信息
    createAction();			//创建行为
    createToolBar();		//创建工具栏
    createMenu();			//创建菜单
    createToolBox();		//创建左侧操作区域与GUI界面
    createStatusBar();		//创建状态栏
    init_Label();			//预处理图片显示
    init_GroupBoxGUIAdjust();

//    //加载主题
    if (config.win_theme == "light") {
        on_action_theme_triggered(0);
    }
    else if (config.win_theme == "dark") {
        on_action_theme_triggered(1);
    }
}

/**
 * @brief Destroy the Widget:: Widget object
 */
Widget::~Widget()
{
	delete img_base;
	img_base = nullptr;
	if (op) {
		delete op;
		op = nullptr;
	}

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
    this->move(config.win_location_x, config.win_location_y);
}

void Widget::init_Label()
{
    ui->lab_img->setPixmap(QPixmap::fromImage(res->curr_img));
	//图片上下文菜单
    connect(ui->lab_img, &QLabel::customContextMenuRequested, this, &Widget::on_label_customContextMenuRequested);

    ui->lab_img_ori->setPixmap(QPixmap::fromImage(res->curr_img));
    //图片上下文菜单
    connect(ui->lab_img_ori, &QLabel::customContextMenuRequested, this, &Widget::on_label_customContextMenuRequested__);

    ui->scrollArea->setMinimumSize(SCROLLAREA_WIDTH,SCROLLAREA_HEIGHT);
    ui->scrollArea_ori->setMinimumSize(SCROLLAREA_WIDTH,SCROLLAREA_HEIGHT);

}

void Widget::init_GroupBoxCutImage()
{
    ui->groupBox_cut->setVisible(false);
    connect(this, &Widget::signal_changeTo_FileOrWork, this, [=]() {
        if (work_files.empty()) {
            //为空，则是file
            ui->groupBox_cut->setVisible(false);
        }
        else {
            //否则，是Work
            ui->groupBox_cut->setVisible(true);
        }
    });

    connect(ui->btn_work_next, &QPushButton::clicked, this, &Widget::on_btn_work_next_clicked);
    connect(ui->btn_work_prev, &QPushButton::clicked, this, &Widget::on_btn_work_prev_clicked);
}

void Widget::init_GroupBoxGUIAdjust()
{
    connect(this, &Widget::signal_choiceToolButton, this, [=](const QString& name) {
        ui->groupBox_adj->setTitle("参数调整: " + name);
        });

    QHBoxLayout* lay_adj = new QHBoxLayout;
    lay_adj->addWidget(choice_GUI_create(0)); //默认第一个窗口AvgBlur
    ui->groupBox_adj->setLayout(lay_adj);
    connect(this, &Widget::signal_choiceToolButton, this, [=](const QString& name,int id) {
        Q_UNUSED(name);
        QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(ui->groupBox_adj->layout());//QHBoxLayout
        QLayoutItem* item;
        while ((item = hLayout->takeAt(0)) != nullptr) {
            QWidget* wid = item->widget();
            if (wid) {
                delete wid;
                wid = nullptr;
            }
        }
        delete hLayout;
        hLayout = nullptr;

        //根据id来创建新的GUI
        QHBoxLayout* lay_adj = new QHBoxLayout;
        lay_adj->addWidget(choice_GUI_create(id));
        ui->groupBox_adj->setLayout(lay_adj);
        ui->groupBox_adj->setVisible(true);
    });
}

void Widget::moveEvent(QMoveEvent* ev)
{
    Q_UNUSED(ev);
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

void Widget::resizeEvent(QResizeEvent* ev)
{
    Q_UNUSED(ev);
    scaledDelta = ori_scaledDelta = init_scaledImageOk();
    update_Image_1(scaledDelta);
}

void Widget::on_label_customContextMenuRequested(const QPoint& pos) {
    Q_UNUSED(pos);
	context_menu->exec(QCursor::pos());
}

void Widget::on_label_customContextMenuRequested__(const QPoint& pos) {
    Q_UNUSED(pos);
	context_menu__->exec(QCursor::pos());
}


void Widget::on_buttonGroup_everyOpeartions_choice(Object* op,QButtonGroup* btn_group,QAbstractButton* btn){
	op->current_choice = btn_group->id(btn);

	/*
	* 当点击具体操作的时候，判断是否处于加工状态：
	* false：说明当前操作不会影响下一次操作的图片，因此直接重置图片
	* true：则下一次操作时的原始图片会在当前操作后的图片上进行，不会重置，但是会进行入栈操作，以起到撤销的效果
	* */
	if (mode) {
		//先恢复再保存
		savePoint();
	}
	else {
		restore_cutOperation();
	}

	int id = switch_Dialog_id(op->current_choice);
    qInfo()<<id;
	emit signal_choiceToolButton(btn->text(),id);
}

void Widget::on_action_exit_triggered()
{
	this->close();
}

void Widget::on_action_open_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(nullptr, "选择文件", ".",	"图像文件(*.png *.jpg)");
	if (!fileName.isEmpty()) {
		work_files.clear();
		emit signal_changeTo_FileOrWork();//发送改变信号
		reload_Resources_ScrollArea(fileName);		
	}
}

void Widget::on_action_works_triggered()
{
	//清除原始内容
	work_files.clear();
    QString FloderPath = QFileDialog::getExistingDirectory(this, "选择文件夹", "resource/Images");
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
            on_action_works_triggered();
			return;
		}
		else if (result == QMessageBox::RejectRole) {
			return; //取消选择
		}
	}
	// 即时加载图片
	reload_Resources_ScrollArea(work_files[work_currentIndex], 1);
}

void Widget::on_action_save_triggered()
{
	//保存加工后的图片
	QString FileName = QFileDialog::getSaveFileName(nullptr, "save image", ".", "Images(*.png *.bmp *.jpg)");
	if (!FileName.isEmpty()) {
		res->curr_img.save(FileName);
	}
}

void Widget::on_action_restore_triggered()
{
	/*
	重置所有操作至原始图片
	*/
	updateFromRoot();
}

void Widget::on_action_draw_triggered()
{
	widget_draw = new DrawWidget;
	widget_draw->show();
	widget_draw->setAttribute(Qt::WA_DeleteOnClose);
}

void Widget::on_colorDialog_triggered(const QColor& color)
{
	Contours* contours = dynamic_cast<Contours*>(op);
	//选择颜色
	contours->onTriggered_Color_currentTextChanged_contoursColor(color);
}

void Widget::on_action_cvtColor_group_triggered(QAction* action)
{
	img_base->cvtColor(action);
}

void Widget::on_action_rotate_group_triggered(QAction* action)
{
    if (action == ui->action_right90) {
		img_base->onTriggered_picture_rotate90();
	}
    else if (action == ui->action_right180) {
		img_base->onTriggered_picture_rotate180();
	}
    else if (action == ui->action_right270) {
		img_base->onTriggered_picture_rotate270();
	}
}

void Widget::on_action_flip_group_triggered(QAction* action)
{
    if (action == ui->action_flip0) {
		img_base->onTriggered_picture_flip0();
	}
    else if (action == ui->action_flip1) {
		img_base->onTriggered_picture_flip1();
	}
    else if (action == ui->action_flip_1) {
		img_base->onTriggered_picture_flip_1();
	}
}

void Widget::on_action_theme_triggered(int type)
{
	QFile qssFile;
	if (type == 0) {
		qssFile.setFileName("resource/qss/light.css");
	}
	else if ( type == 1){
		qssFile.setFileName("resource/qss/dark.css");
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

void Widget::on_actionGroupHelp_triggered(QAction* action)
{
    if (action == ui->action_help) {
		//打开帮助文档
		QUrl help_url("https://github.com/luumod/myPhotoshopApp");
		if (!QDesktopServices::openUrl(help_url)) {
			qWarning() << "打开帮助文档失败!";
		}
	}
    else if (action == ui->action_aboutme) {
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

void Widget::on_action_hide_triggered()
{
    ui->action_hide->setEnabled(false);
    ui->action_disp->setEnabled(true);
    //单图片
    ui->widget_img_ori->setVisible(false);
    ui->right_arrow->setVisible(false);

    QLayoutItem* item;
    QVBoxLayout* ver_AdjArea_layout = new QVBoxLayout;
    auto hor_AdjArea_layout = ui->adjArea->layout();

    while ((item = hor_AdjArea_layout->takeAt(0))) {
        ver_AdjArea_layout->addItem(item);
    }
    delete hor_AdjArea_layout;
    hor_AdjArea_layout = nullptr;

    ui->adjArea->setLayout(ver_AdjArea_layout);
    ver_AdjArea_layout->setStretch(0,2);
    ver_AdjArea_layout->setStretch(1,1);
    ver_AdjArea_layout->setStretch(2,4);

    item = nullptr;
    QHBoxLayout* hor_central_layout = new QHBoxLayout;
    auto ver_central_layout = ui->widget_central->layout();
    while ((item = ver_central_layout->takeAt(0))){
        hor_central_layout->addItem(item);
    }
    delete ver_central_layout;
    ver_central_layout = nullptr;

    ui->widget_central->setLayout(hor_central_layout);
    hor_central_layout->setStretch(0,6);
    hor_central_layout->setStretch(1,2);

    ui->scrollArea->setMinimumSize(600,600);
    ui->scrollArea->resize(ui->scrollArea->minimumSize());
    scaledDelta = ori_scaledDelta = init_scaledImageOk();
}

void Widget::on_action_disp_triggered()
{
    ui->action_hide->setEnabled(true);
    ui->action_disp->setEnabled(false);

    ui->widget_img_ori->setVisible(true);
    ui->right_arrow->setVisible(true);


    QLayoutItem* item;
    QHBoxLayout* hor_AdjArea_layout = new QHBoxLayout;
    auto ver_AdjArea_layout = ui->adjArea->layout();
    while ((item = ver_AdjArea_layout->takeAt(0))) {
        hor_AdjArea_layout->addItem(item);
    }
    delete ver_AdjArea_layout;
    ver_AdjArea_layout = nullptr;

    ui->adjArea->setLayout(hor_AdjArea_layout);
    hor_AdjArea_layout->setStretch(0,2);
    hor_AdjArea_layout->setStretch(1,1);
    hor_AdjArea_layout->setStretch(2,4);

    item = nullptr;
    QVBoxLayout* ver_central_layout = new QVBoxLayout;
    auto hor_central_layout = ui->widget_central->layout();
    while ((item = hor_central_layout->takeAt(0))){
        ver_central_layout->addItem(item);
    }
    delete hor_central_layout;
    hor_central_layout = nullptr;

    ui->widget_central->setLayout(ver_central_layout);
    ver_central_layout->setStretch(0,6);
    ver_central_layout->setStretch(1,2);


    ui->scrollArea->setMinimumSize(SCROLLAREA_WIDTH,SCROLLAREA_HEIGHT);
    ui->scrollArea_ori->setMinimumSize(SCROLLAREA_WIDTH,SCROLLAREA_HEIGHT);
    ui->scrollArea->resize(ui->scrollArea->minimumSize());
    ui->scrollArea_ori->resize(ui->scrollArea_ori->minimumSize());
    scaledDelta = ori_scaledDelta = init_scaledImageOk();
    update_Image_1(scaledDelta);
}

void Widget::on_btn_work_next_clicked()
{
	work_prevIndex = work_currentIndex;
	work_currentIndex++;
	if (work_currentIndex >= work_files.count()) {
		work_currentIndex = 0;
		work_prevIndex = work_files.count() - 1;
	}
    on_cbx_work_autoSave_clicked();
	work_cutImage();
}

void Widget::on_btn_work_prev_clicked()
{
	work_prevIndex = work_currentIndex;
	work_currentIndex--;
	if (work_currentIndex < 0) {
		work_prevIndex = 0;
		work_currentIndex = work_files.count() - 1;
	}
    on_cbx_work_autoSave_clicked();
	work_cutImage();
}

void Widget::reload_Resources_ScrollArea(const QString& fileName, int mode)
{
    Q_UNUSED(mode);
	res->reset(fileName.toLocal8Bit().data());

	//对于操作图片窗口的更新
    ui->scrollArea->takeWidget();
    ui->scrollArea->setWidget(ui->lab_img);
	//对于原始图片窗口的更新
    ui->scrollArea_ori->takeWidget();
    ui->scrollArea_ori->setWidget(ui->lab_img_ori);

	//操作图片尺寸更新
	scaledDelta = ori_scaledDelta = init_scaledImageOk();
	//原始图片尺寸更新
	update_Image_1(ori_scaledDelta);

	//所有按钮置为未选中状态
	for (auto& btnGps : btngroups) {
		btnGps->setExclusive(false);
		for (auto& x : btnGps->buttons()) {
			x->setChecked(false);
		}
		btnGps->setExclusive(true);
	}
	//清除之前的保存的撤销图片
	while (!undo_sta.empty()) {
		undo_sta.pop();
	}
}

void Widget::on_cbx_work_autoSave_clicked()
{
    //在切换页面的时候弹出保存提示
    if (ui->cbx_work_autoSave->isChecked()) {
        on_action_save_triggered();
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
	emit signal_changeTo_FileOrWork();//发送改变为Work信号
	work_currentIndex = work_prevIndex = 0;
	return true;
}

void Widget::restore_cutOperation()
{
	updateFromIntermediate();
}

void Widget::on_action_begin_triggered(){
	//点击此开始创作模式
	//图片清除，重新开始，所有滑块归零
	if (!mode) {
		emit signal_changeMode(true);
	}
	else {
		emit signal_changeMode(false);
    }
	//数据清空
	updateFromIntermediate();
}

void Widget::on_action_return_triggered()
{
	//仅仅在创作者模式下生效：触发时会将图片置回到未操作前的位置
	if (mode) {
		returnPoint();
		update_Image(ori_scaledDelta);
	}
	else {
        on_action_restore_triggered();
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

void Widget::createAction()
{
    connect(res, &Res::signal_updateImage, this, [=]() {
        ui->edit_name->setText(res->fileInfo.fileName());
        ui->edit_path->setText(res->fileInfo.absolutePath() + "/");
        auto B = res->fileInfo.size() / 1024;
        ui->edit_size->setText(QString::number(B) + " Bytes");
        ui->edit_rect->setText(QString::number(res->root_mt.cols) + " * " + QString::number(res->root_mt.rows));
        ui->edit_mode->setText(QVariant(res->curr_img.format()).toString());
        ui->edit_channels->setText(QString::number(res->root_mt.channels()));
        ui->edit_geshi->setText(QString(res->fileInfo.suffix().toLower()));
    });
    emit res->signal_updateImage();

    action_theme = new QAction(this);

    //关闭窗口
    ui->action_exit->setStatusTip(tr("退出程序"));
    ui->action_exit->setShortcut(tr("Ctrl+E"));

    //打开文件
    ui->action_open->setStatusTip(tr("选择并且打开一个图片"));
    ui->action_open->setShortcut(tr("Ctrl+O"));
    //打开文件夹
    ui->action_works->setStatusTip(tr("选择并且打开文件夹作为当前工作区域"));
    ui->action_works->setShortcut(tr("Ctrl+F"));

    //保存图片
    ui->action_save->setStatusTip(tr("保存此图片"));
    ui->action_save->setShortcut(tr("Ctrl+S"));

    //重置图片
    ui->action_restore->setStatusTip(tr("重置此图片，取消所有加工"));
    ui->action_restore->setShortcut(tr("Ctrl+Shift+Z"));

    //开始制作模式
    ui->action_begin->setStatusTip(tr("图片加工模式，可以混合修改图片"));
    ui->action_begin->setCheckable(true);

    //撤销
    ui->action_return->setShortcut(tr("Ctrl+Z"));
    ui->action_return->setStatusTip(tr("撤销此操作"));

    //打开简单绘图板
    ui->action_draw->setStatusTip(tr("绘图操作"));

	//轮廓操作时 选择颜色 自动更新
	colorDialog = new QColorDialog(this);
    connect(colorDialog, &QColorDialog::currentColorChanged,
            this, [=](const QColor &color){on_colorDialog_triggered(color);});

	//颜色转换
	img_base = new BaseOperate();
	action_cvtColor_group = new QActionGroup(this);
    action_cvtColor_group->addAction(ui->action_ori);
    action_cvtColor_group->addAction(ui->action_hls);
    action_cvtColor_group->addAction(ui->action_hsv);
    action_cvtColor_group->addAction(ui->action_rgb);
    action_cvtColor_group->addAction(ui->action_lab);
    connect(action_cvtColor_group, &QActionGroup::triggered, this,[=](QAction* action){on_action_cvtColor_group_triggered(action);});

	//图片旋转菜单
	action_rotate_group = new QActionGroup(this);
    action_rotate_group->addAction(ui->action_right90);
    action_rotate_group->addAction(ui->action_right180);
    action_rotate_group->addAction(ui->action_right270);
    connect(action_rotate_group, &QActionGroup::triggered, this,[=](QAction* action){on_action_rotate_group_triggered(action);});


	//图片翻转菜单
	action_flip_group = new QActionGroup(this);
    action_flip_group->addAction(ui->action_flip0);
    action_flip_group->addAction(ui->action_flip1);
    action_flip_group->addAction(ui->action_flip_1);
    connect(action_flip_group, &QActionGroup::triggered, this,[=](QAction* action){on_action_flip_group_triggered(action);});

    //帮助菜单
    actionGroupHelp = new QActionGroup(this);
    actionGroupHelp->addAction(ui->action_help);
    actionGroupHelp->addAction(ui->action_aboutme);
    connect(actionGroupHelp, &QActionGroup::triggered, this, [=](QAction* action){on_actionGroupHelp_triggered(action);});

    //图片对比度提高
    connect(ui->action_mark, &QAction::triggered, this, [=]() {
		img_base->onTriggered_picture_mask();
		});

    //灰度直方图
    connect(ui->action_hist, &QAction::triggered, this, [=]() {
		img_base->drawGrayHist("image histogram");
		});

    //扩展
    connect(ui->action_light, &QAction::triggered, this, [=]() {
		config.win_theme = "light";
		QMessageBox::information(this, tr("提示"), tr("切换主题成功！请重启程序"));
		});

    connect(ui->action_dark, &QAction::triggered, this, [=]() {
		config.win_theme = "dark";
		QMessageBox::information(this, tr("提示"), tr("切换主题成功！请重启程序"));
		});


    connect(ui->action_jie, &QAction::triggered, this, &Widget::on_action_jie_triggered);

    ui->action_capture->setShortcut(tr("Ctrl+Alt+A"));
    connect(ui->action_capture, &QAction::triggered, this, &Widget::on_action_capture_triggered);



}

void Widget::createMenu()
{
	//图片上下文菜单
	//此处跟随滑动区域，滑动区域在整个窗口中作为主窗口不会消失。
    context_menu = new QMenu(this);
    context_menu->addAction(ui->action_hide);
    context_menu->addAction(ui->action_save);
    context_menu->addAction(ui->action_exit);
    context_menu->addAction(ui->action_restore);

	context_menu__ = new QMenu(this);
    context_menu__->addAction(ui->action_hide);
    context_menu__->addAction(ui->action_exit);
	
	QMenu* menu_equalize = new QMenu("均衡化图像",this);
	menu_equalize->addAction("灰度图", this, [=]() {img_base->showEqualizedGrayImage(); });
	menu_equalize->addAction("彩色图", this, [=]() {img_base->showEqualizedBGRImage(); });
    ui->menu_func->addMenu(menu_equalize);
    ui->menu_func->addSeparator();
}

void Widget::createToolBar()
{
    ui->toolBar = addToolBar(tr("file"));
    ui->toolBar->addAction(ui->action_begin);
    ui->toolBar->addAction(ui->action_restore);
    ui->toolBar->addAction(ui->action_return);
    ui->toolBar->addAction(ui->action_draw);
}

void Widget::createToolBox()
{
	//-----------------模糊操作-----------------------------
	btngroups.push_back(btngroup_blur = new QButtonGroup(this));
	btngroup_blur->setExclusive(true);
	//连接信号
	connect(btngroup_blur, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		//点击按钮创建抽象操作类，运行时动态加载
		if (!op) {
			op = new Blur();
		}
		on_buttonGroup_everyOpeartions_choice(op, btngroup_blur, btn);
		});

    btngroup_blur->addButton(ui->avg_blur_btn,BLUR::Average);
    btngroup_blur->addButton(ui->gas_blur_btn,BLUR::Gaussian);
    btngroup_blur->addButton(ui->median_blur_btn,BLUR::Median);
    btngroup_blur->addButton(ui->bilt_blur_btn,BLUR::Bilateral);

	//-----------------阈值化操作-----------------------------
	btngroups.push_back(btngroup_threshold = new QButtonGroup(this));
	btngroup_threshold->setExclusive(true);
	//连接信号
	connect(btngroup_threshold, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		if (!op) {
			op = new Threshold();
		}
		on_buttonGroup_everyOpeartions_choice(op, btngroup_threshold, btn);
		});

    btngroup_threshold->addButton(ui->two_threshold_btn,THRESHOLD::Binary);
    btngroup_threshold->addButton(ui->invtwo_threshold_btn,THRESHOLD::Binary_inv);
    btngroup_threshold->addButton(ui->jieduan_threshold_btn,THRESHOLD::Trunc);
    btngroup_threshold->addButton(ui->yuzhi_threshold_btn,THRESHOLD::Tozero);
    btngroup_threshold->addButton(ui->invyuzhi_threshold_btn,THRESHOLD::Tozero_inv);


	//-----------------形态化操作-----------------------------
	btngroups.push_back(btngroup_form = new QButtonGroup(this));
	btngroup_form->setExclusive(true);
	//连接信号
	connect(btngroup_form, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		if (!op) {
			op = new Morphology();
		}
		on_buttonGroup_everyOpeartions_choice(op, btngroup_form, btn);
        });

    btngroup_form->addButton(ui->pengzhang_mor_btn,FORM::Erode);
    btngroup_form->addButton(ui->fushi_mor_btn,FORM::Dilate);
    btngroup_form->addButton(ui->kai_mor_btn,FORM::Open);
    btngroup_form->addButton(ui->bi_mor_btn,FORM::Close);
    btngroup_form->addButton(ui->tidu_mor__btn,FORM::Gradient);
    btngroup_form->addButton(ui->dingmao_mor_btn,FORM::Tophat);
    btngroup_form->addButton(ui->hei_mor_btn,FORM::Blackhat);

	//-----------------连通区域分析操作-----------------------------
	btngroups.push_back(btngroup_connected = new QButtonGroup(this));
	btngroup_connected->setExclusive(true);//互斥

	connect(btngroup_connected, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		if (!op) {
			op = new Connected();
		}
		on_buttonGroup_everyOpeartions_choice(op, btngroup_connected, btn);
		});

    btngroup_connected->addButton(ui->one_connected_btn,CONNECTED::CONNECTED_TYPE1);
    btngroup_connected->addButton(ui->two_connected_btn,CONNECTED::CONNECTED_TYPE2);

	//-----------------轮廓绘制操作-----------------------------
	btngroups.push_back(btngroup_contours = new QButtonGroup(this));
	btngroup_contours->setExclusive(true);

	connect(btngroup_contours, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		if (!op) {
			op = new Contours();
		}
		on_buttonGroup_everyOpeartions_choice(op, btngroup_contours, btn);
		});

    btngroup_contours->addButton(ui->draw_contours_btn,CONTOURS::CONTOURS_TYPE1);

	//-----------------图像效果增强------------------------------
	btngroups.push_back(btngroup_show = new QButtonGroup(this));
	btngroup_show->setExclusive(true);

	connect(btngroup_show, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
		if (!op) {
			op = new Showeffect();
		}
		on_buttonGroup_everyOpeartions_choice(op, btngroup_show, btn);
		});

    btngroup_show->addButton(ui->bright_effect_btn,SHOW::LIGHT);
    btngroup_show->addButton(ui->gamma_effect_btn,SHOW::GAMMA);
    btngroup_show->addButton(ui->linear_effect_btn,SHOW::LINEAR);
    btngroup_show->addButton(ui->gray_effect_btn,SHOW::GRAYWINDOW);
    btngroup_show->addButton(ui->DpLinear_effect_btn,SHOW::DPLINEAR);
    btngroup_show->addButton(ui->nonDpLinear_effect_btn,SHOW::NON_DPLINEAR);

    ////获取切换后的toolbox索引
    connect(ui->toolbox_side, &QToolBox::currentChanged, this, [=](int value) {
		preToolBoxIndex = curToolBoxIndex;
        curToolBoxIndex = value;

		if (op) {
			delete op;
			op = nullptr;
		}
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
    statusLab = new QLabel("默认模式");
	statusLab->setObjectName("statusBar_lab");
	statusBar()->addWidget(statusLab);

	connect(this, &Widget::signal_changeMode, this, [=](bool st) {
		if (st) {
			mode = true;
			statusLab->setText(tr("混合加工模式"));

			// 获取应用程序的状态栏
			QStatusBar* statusBar_warn = statusBar();

			// 创建一个 QLabel 来显示警告信息
			QLabel* warningLabel = new QLabel("警告！混合操作具有不确定性，若操作失败产生中断本人概不负责！");
			warningLabel->setStyleSheet("color: red"); // 设置文本颜色为红色

			// 使用 addWidget 将 QLabel 放置在状态栏的右侧
			statusBar_warn->addPermanentWidget(warningLabel);

			// 如果需要显示一段时间后自动隐藏，您可以使用 QTimer 来实现
			QTimer::singleShot(5000, [=]() {
				warningLabel->setVisible(false);
			statusBar_warn->removeWidget(warningLabel);
			delete warningLabel;
				});
		}
		else {
			mode = false;
			statusLab->setText(tr("默认模式"));
		}
		});
	

}

QWidget* Widget::create_GUIAvgBlur()
{
	//----------------------------------------------------------
	Blur* blur = dynamic_cast<Blur*>(op);
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

	auto w = new QWidget;
	w->setLayout(
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
	return w;
}

QWidget* Widget::create_GUIGaussian()
{
	Blur* blur = dynamic_cast<Blur*>(op);
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


	auto w = new QWidget;
	w->setLayout(
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
	return w;
}

QWidget* Widget::create_GUIMedian()
{
	Blur* blur = dynamic_cast<Blur*>(op);
	//----------------------------------------------------------
	//中值滤波
	ls_slider_median.resize(1);
	std::function<void(int)> funMedian = [=](int value) {
		if (value % 2 == 0)
			value += 1;
		blur->onTriggered_slider_valueChange_medianBlur(value); };

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Blur*>(
		ls_slider_median,
		QList<int>() << 1,
		QList<int>() << MEDIAN_BLUR_MAX,
		QList<int>() << 2,
		QList<QString>() << "median_slider",
		QList<QString>() << "KSize",
		QList< std::function<void(int)>>() << funMedian,
		true, "\\d+", "KSize", &blur)
	);
	return w;
}

QWidget* Widget::create_GUIBilateral()
{
	Blur* blur = dynamic_cast<Blur*>(op);
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

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Blur*>(
		ls_slider_bilateral,
		QList<int>() << 1 << 0 << 0,
		QList<int>() << 50 << 120 << 150,
		QList<int>() << 1 << 1 << 1,
		QList<QString>() << "bilateral_slider" << "sigma_Color" << "sigma_Space",
		QList<QString>() << "ksize" << "sigmaX" << "sigmaY",
		QList< std::function<void(int)>>() << funBilateral_slider1 << funBilateral_slider2 << funBilateral_slider3,
		true, "\\d+\\s\\d+\\s\\d+", "bin_d sigmaColor sigmaSpace", &blur)
	);
	return w;
}

QWidget* Widget::create_GUIThreshold()
{
	Threshold* threshold = dynamic_cast<Threshold*>(op);
	//----------------------------------------------------------
	//阈值化
	ls_slider_threshold.resize(2);
	std::function<void(int)> funThreshold_slider1 = [=](int value) {
		threshold->onTriggered_slider1_valueChanged_thresholdValue(value);
	};

	std::function<void(int)> funThreshold_slider2 = [=](int value) {
		threshold->onTriggered_slider2_valueChanged_maxValue(value);
	};

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Threshold*>(
		ls_slider_threshold,
		QList<int>() << 0 << 0,
		QList<int>() << 255 << 255,
		QList<int>() << 1 << 1,
		QList<QString>() << "threshold_value" << "maxValue",
		QList<QString>() << "threshold" << "maxval",
		QList< std::function<void(int)>>() << funThreshold_slider1 << funThreshold_slider2,
		true, "\\d+\\s\\d+", "threshold_value maxVal", &threshold)
	);
	return w;
}

QWidget* Widget::create_GUIMorphology()
{
	Morphology* morphology = dynamic_cast<Morphology*>(op);
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

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Morphology*>(
		ls_slider_morphology,
		QList<int>() << 1 << 1 << 1,
		QList<int>() << 50 << 20 << 20,
		QList<int>() << 1 << 1 << 1,
		QList<QString>() << "Kernal_mor" << "X_mor" << "Y_mor",
		QList<QString>() << "Kernel" << "anchorX" << "anchorY",
		QList< std::function<void(int)>>() << funMorphology_slider1 << funMorphology_slider2 << funMorphology_slider3,
		true, "\\d+\\s\\d+\\s\\d+\\s\\d+", "Kernel X Y iters", &morphology)
	);
	return w;
}

QWidget* Widget::create_GUIConnected()
{
	Connected* connected = dynamic_cast<Connected*>(op);
	//----------------------------------------------------------
	//连通性
	ls_combox_connected.resize(2);
	std::function<void(int)> funConnected_combo1 = [=](int index) {
		connected->onTriggered_Comb2_currentTextChanged_ccltype(index);
	};

	std::function<void(int)> funConnected_combo2 = [=](int index) {
		connected->onTriggered_Comb2_currentTextChanged_ccltype(index);
	};

	QStringList com1 = { "8" , "4" };
	QStringList com2 = { "default","CC_WU","CCL_GRANA","CCL_BOLELLI","CCL_SAUF","CCL_BBDT","CCL_SPAGHETTI" };

	auto w = new QWidget;
	w->setLayout(createDialog_nComBox_GUItemplate(
		ls_combox_connected,
		QList<QStringList>() << com1 << com2,
		QList<QString>() << "connectivity" << "ccltype",
		QList<QString>() << "邻域" << "联通算法",
		QList< std::function<void(int)>>() << funConnected_combo1 << funConnected_combo2)
	);
	return 	w;
}

QWidget* Widget::create_GUIContours()
{
	Contours* contours = dynamic_cast<Contours*>(op);
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
        Q_UNUSED(index);
		colorDialog->setMinimumSize(375, 375);
		colorDialog->setMaximumSize(653, 498);
		colorDialog->setGeometry(810, 306, 375, 375);
		colorDialog->show(); //弹出颜色框
	};

	std::function<void(int)> funContours_combo5 = [=](int index) {
        Q_UNUSED(index);
		contours->onClicked_btn_convexHull();
	};

	QStringList contours_com1 = { "RETR_EXTERNAL", "RETR_LIST","RETR_CCOMP","RETR_TREE" };
	QStringList contours_com2 = { "CHAIN_APPROX_NONE", "CHAIN_APPROX_SIMPLE","CHAIN_APPROX_TC89_L1","CHAIN_APPROX_TC89_KCOS" };
	QStringList contours_com3;
	for (int i = 1; i <= 10; i++) {
		contours_com3.append(QString("%1").arg(i));
	}
	QStringList contours_com4 = { "更换颜色" };
	QStringList contours_com5 = { "绘制图像凸包" };

	auto w = new QWidget;
	w->setLayout(createDialog_nComBox_GUItemplate(
		ls_combox_contours,
		QList<QStringList>() << contours_com1 << contours_com2 << contours_com3 << contours_com4 << contours_com5,
		QList<QString>() << "contours_mode" << "contours_method" << "contours_thick" << "coutours_color" << "coutours_FullHex",
		QList<QString>() << "轮廓检索模式" << "轮廓逼近方法" << "绘制线宽度" << "other" << "other",
		QList< std::function<void(int)>>() << funContours_combo1 << funContours_combo2 << funContours_combo3 << funContours_combo4 << funContours_combo5)
	);
	return w;
}

QWidget* Widget::create_GUIbright()
{
	Showeffect* showeffect = dynamic_cast<Showeffect*>(op);
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

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Showeffect*>(
		ls_slider_light,
		QList<int>() << 1 << 1,
		QList<int>() << 100 << 100,
		QList<int>() << 1 << 1,
		QList<QString>() << "bright_light_value" << "bright_dark_value",
		QList<QString>() << "亮度增加" << "亮度降低",
		QList< std::function<void(int)>>() << funcLight << funcDark,
		true, "-?\\d+", "亮度值(负值表示降低)", &showeffect)
	);
	return w;
}

QWidget* Widget::create_GUIgamma()
{
	Showeffect* showeffect = dynamic_cast<Showeffect*>(op);
	//-----------------------------------------------------
	//gamma矫正
	ls_slider_gamma.resize(1);
	std::function<void(int)> funcGamma = [=](int value) {
		//将int映射为double
		double d_val = value * 1.0 / 10.0;
		showeffect->onTriggered_slider_valueChange_gamma(d_val);
	};

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Showeffect*>(
		ls_slider_gamma,
		QList<int>() << 1,
		QList<int>() << 50,
		QList<int>() << 1,
		QList<QString>() << "gamma_slider",
		QList<QString>() << "γ矫正",
		QList< std::function<void(int)>>() << funcGamma,
		true,"\\d+","γ值",&showeffect)
	);
	return w;
}

QWidget* Widget::create_GUIContrast()
{
	Showeffect* showeffect = dynamic_cast<Showeffect*>(op);
	ls_slider_linear.resize(2);
	std::function<void(int)> funcLinearG1 = [=](int value) {
		//将int映射为double
		showeffect->onTriggered_slider_valueChange_linearg1(value);
	};
	std::function<void(int)> funcLinearG2 = [=](int value) {
		//将int映射为double
		showeffect->onTriggered_slider_valueChange_linearg2(value);
	};

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Showeffect*>(
		ls_slider_linear,
		QList<int>() << 0 << 0,
		QList<int>() << 255 << 255,
		QList<int>() << 1 << 1,
		QList<QString>() << "linearG1_slider" << "linearG2_slider",
		QList<QString>() << "仿射下限" <<"仿射上限",
		QList< std::function<void(int)>>() << funcLinearG1 << funcLinearG2,
		true, "\\d+\\s\\d+","仿射下限 仿射上限",&showeffect)
	);
	// 获取QWidget的布局
	//额外添加控件
	QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(w->layout());

	if (layout) {
		QHBoxLayout* hlayout = new QHBoxLayout;
		QRadioButton* r_btn1 = new QRadioButton("灰度图");
		r_btn1->setChecked(true);
		QRadioButton* r_btn2 = new QRadioButton("彩色图");
		QButtonGroup* group = new QButtonGroup(this);
		group->addButton(r_btn1);
		group->addButton(r_btn2);
		connect(group, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
			if (btn == r_btn1) {
				showeffect->linear_mode = 0; //灰度图
			}
			else {
				showeffect->linear_mode = 1;
			}
			});
		hlayout->addWidget(r_btn1,0,Qt::AlignHCenter);
		hlayout->addWidget(r_btn2,0, Qt::AlignHCenter);

		layout->insertLayout(0, hlayout);
	}

	return w;
}

QWidget* Widget::create_GUIGrayWindow()
{
	Showeffect* showeffect = dynamic_cast<Showeffect*>(op);
	ls_slider_grayWindow.resize(2);
	std::function<void(int)> funcGrayG1 = [=](int value) {
		//将int映射为double
		showeffect->onTriggered_slider_valueChange_GaryWindowF1(value);
	};
	std::function<void(int)> funcGrayG2 = [=](int value) {
		//将int映射为double
		showeffect->onTriggered_slider_valueChange_GaryWindowF2(value);
	};

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Showeffect*>(
		ls_slider_grayWindow,
		QList<int>() << 0 << 0,
		QList<int>() << 255 << 255,
		QList<int>() << 1 << 1,
		QList<QString>() << "GreayF1_slider" << "GrayF2_slider",
		QList<QString>() << "选择下限" << "选择上限",
		QList< std::function<void(int)>>() << funcGrayG1 << funcGrayG2,
		true, "\\d+\\s\\d+", "选择下限 选择上限", &showeffect)
	);
	return w;
}

QWidget* Widget::create_GUIDPLinear()
{
	Showeffect* showeffect = dynamic_cast<Showeffect*>(op);
	ls_slider_dpLinear.resize(2);
	std::function<void(int)> funcdpA = [=](int value) {
		//将int映射为double
		showeffect->onTriggered_slider_valueChange_DynamicA(value);
	};
	std::function<void(int)> funcdpB = [=](int value) {
		//将int映射为double
		showeffect->onTriggered_slider_valueChange_DynamicB(value);
	};

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Showeffect*>(
		ls_slider_dpLinear,
		QList<int>() << 0 << 0,
		QList<int>() << 255 << 255,
		QList<int>() << 1 << 1,
		QList<QString>() << "DPLinearA_slider" << "DPLinearB_slider",
		QList<QString>() << "区域下限" << "区域上限",
		QList< std::function<void(int)>>() << funcdpA << funcdpB,
		true,"\\d+\\s\\d+","区域下限 区域上限",&showeffect)
	);
	// 获取QWidget的布局
	//额外添加控件
	QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(w->layout());

	if (layout) {
		QHBoxLayout* hlayout = new QHBoxLayout;
		QRadioButton* r_btn1 = new QRadioButton("灰度图");
		r_btn1->setChecked(true);
		QRadioButton* r_btn2 = new QRadioButton("彩色图");
		QButtonGroup* group = new QButtonGroup(this);
		group->addButton(r_btn1);
		group->addButton(r_btn2);
		connect(group, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
			if (btn == r_btn1) {
				showeffect->DpLinear_mode = 0; //灰度图
			}
			else {
				showeffect->DpLinear_mode = 1;
			}
			});
		hlayout->addWidget(r_btn1, 0, Qt::AlignHCenter);
		hlayout->addWidget(r_btn2, 0, Qt::AlignHCenter);

		layout->insertLayout(0, hlayout);
	}
	return w;
}

QWidget* Widget::create_GUINoneDpLinear()
{
	Showeffect* showeffect = dynamic_cast<Showeffect*>(op);
	ls_slider_NoneDpLinear.resize(1);
	std::function<void(int)> funcNoneDpC = [=](int value) {
		//将int映射为double
		showeffect->onTriggered_slider_valueChange_NoneDynamicC(value);
	};

	auto w = new QWidget;
	w->setLayout(createDialog_nSlider_GUItemplate<int, Showeffect*>(
		ls_slider_NoneDpLinear,
		QList<int>() << 0,
		QList<int>() << 80,
		QList<int>() << 1,
		QList<QString>() << "NoneDpLinearC_slider",
		QList<QString>() << "增益常数",
		QList< std::function<void(int)>>() << funcNoneDpC,
		true, "\\d+", "增益常数",&showeffect)
	);
	// 获取QWidget的布局
	//额外添加控件
	QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(w->layout());

	if (layout) {
		QHBoxLayout* hlayout = new QHBoxLayout;
		QRadioButton* r_btn1 = new QRadioButton("灰度图");
		r_btn1->setChecked(true);
		QRadioButton* r_btn2 = new QRadioButton("彩色图");
		QButtonGroup* group = new QButtonGroup(this);
		group->addButton(r_btn1);
		group->addButton(r_btn2);
		connect(group, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
			if (btn == r_btn1) {
				showeffect->NoneDpLinear_mode = 0; //灰度图
			}
			else {
				showeffect->NoneDpLinear_mode = 1;
			}
			});
		hlayout->addWidget(r_btn1, 0, Qt::AlignHCenter);
		hlayout->addWidget(r_btn2, 0, Qt::AlignHCenter);
		layout->insertLayout(0, hlayout);
	}
	return w;
}

QWidget* Widget::choice_GUI_create(int id)
{
	switch (id)
	{
	case 0:
		return create_GUIAvgBlur();
		break;
	case 1:
		return create_GUIGaussian();
		break;
	case 2:
		return create_GUIMedian();
		break;
	case 3:
		return create_GUIBilateral();
		break;
	case 4:
		return create_GUIThreshold();
		break;
	case 5:
		return create_GUIMorphology();
		break;
	case 6:
		return create_GUIConnected();
		break;
	case 7:
		return create_GUIContours();
		break;
	case 8:
		return create_GUIbright();
		break;
	case 9:
		return create_GUIgamma();
		break;
	case 10:
		return create_GUIContrast();
		break;
	case 11:
		return create_GUIGrayWindow();
		break;
	case 12:
		return create_GUIDPLinear();
		break;
	case 13:
		return create_GUINoneDpLinear();
		break;
	default:
		break;
	}
    return 0;
}

QWidget* Widget::createToolBtnItemWidget(const QString& text, int id, const QString& fileName)
{
	QToolButton* btn = new QToolButton;
	btn->setObjectName("function_tbtn");
	btn->setText(text);
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
	textlab->setWordWrap(true);  // 启用自动换行
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


void Widget::update_Image_1(double f_scaledDelta)
{
	//更新图片显示到完美缩放比例
    QImage img = Mat2QImage(res->root_mt);
    QPixmap t_pixmap = QPixmap::fromImage(img);
	QPixmap scaledPixmap = t_pixmap.scaled(t_pixmap.size() * f_scaledDelta, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->lab_img_ori->setPixmap(scaledPixmap);

	//检查是否需要启用滚动条
    bool needScrollbars = scaledPixmap.size().width() > ui->scrollArea->size().width() || scaledPixmap.size().height() > ui->scrollArea->size().height();
    ui->scrollArea_ori->setHorizontalScrollBarPolicy(needScrollbars ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
    ui->scrollArea_ori->setVerticalScrollBarPolicy(needScrollbars ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
}

void Widget::update_Image(double f_scaledDelta)
{
	//更新图片显示到完美缩放比例
	QPixmap t_pixmap = QPixmap::fromImage(res->curr_img);
	QPixmap scaledPixmap = t_pixmap.scaled(t_pixmap.size() * f_scaledDelta, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->lab_img->setPixmap(scaledPixmap);

	//检查是否需要启用滚动条
    bool needScrollbars = scaledPixmap.size().width() > ui->scrollArea->size().width() || scaledPixmap.size().height() > ui->scrollArea->size().height();
    ui->scrollArea->setHorizontalScrollBarPolicy(needScrollbars ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(needScrollbars ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
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
    qInfo()<<ui->scrollArea->size() << res->curr_img.size();
    double wDelta = static_cast<double>((double)ui->scrollArea->size().width() / (double)res->curr_img.size().width());
    double hDelta = static_cast<double>((double)ui->scrollArea->size().height() / (double)res->curr_img.size().height());
	auto t_scaledDelta = qMin(wDelta, hDelta);

	update_Image(t_scaledDelta);

	return t_scaledDelta;
}
