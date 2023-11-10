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
    init_readJson();		        //读取配置文件
    init_WidgetInfo();		        //设置主窗口信息
    init_CustomAction();			//创建行为
    init_CustomMenu();			    //创建菜单
    init_ToolBoxSide();		        //创建左侧操作区域与GUI界面
    init_CustomStatusBar();		    //创建状态栏
    init_Label();			        //预处理图片显示
    init_specialConnect();          //处理特殊的信号与槽的链接

    QFile qssFile;
    if (config.win_theme == "light"){
        qssFile.setFileName("resource/qss/light.css");
    }
    else if (config.win_theme == "dark"){
        qssFile.setFileName("resource/qss/dark.css");
    }
    if (qssFile.open(QFile::OpenModeFlag::ReadOnly)) {
        this->setStyleSheet(qssFile.readAll());
    }
}

/**
 * @brief Destroy the Widget:: Widget object
 */
Widget::~Widget()
{
	delete img_base;
	img_base = nullptr;

    for (auto& x:ls_opts){
        delete x;
        x = nullptr;
    }

	while (!undo_sta.empty()) {
		undo_sta.pop();
	}
	delete res;
	res = nullptr;

    delete ui;
    ui=nullptr;
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
    //设置显式弹出菜单
    ui->lab_img->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->lab_img_ori->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->lab_img->setPixmap(QPixmap::fromImage(res->curr_img));
    ui->lab_img_ori->setPixmap(QPixmap::fromImage(res->curr_img));

    ui->scrollArea->setMinimumSize(SCROLLAREA_WIDTH,SCROLLAREA_HEIGHT);
    ui->scrollArea_ori->setMinimumSize(SCROLLAREA_WIDTH,SCROLLAREA_HEIGHT);

}
void Widget::init_specialConnect()
{
    //便于管理
    ls_opts.push_back(blur = new Blur);
    ls_opts.push_back(threshold = new Threshold);
    ls_opts.push_back(morphology = new Morphology);
    ls_opts.push_back(connected = new Connected);
    ls_opts.push_back(contours = new Contours);
    ls_opts.push_back(showeffect = new Showeffect);

    for (int i =0;i<14;i++){
        choice_GUI_create(i);
    }
    ui->adj_stackedWidget->setCurrentWidget(ui->adjPage_first);
    connect(this, &Widget::signal_choiceToolButton, this, [=](const QString& name,int id) {
        ui->groupBox_adj->setTitle(name);
        clearAllAdjPageWidgetValue(); //清除所有页面的操作
        ui->adj_stackedWidget->setCurrentIndex(id); //切换当前页面
    });

    //更新图片的时候，图片信息修改
    connect(res, &Res::signal_updateImage, this, [=]() {
        ui->edit_name->setText(res->fileInfo.fileName());
        ui->edit_path->setText(res->fileInfo.absolutePath() + "/");
        ui->edit_rect->setText(QString::number(res->root_mt.cols) + "像素 * " + QString::number(res->root_mt.rows) + "像素");
        auto B = res->fileInfo.size() / 1024;
        ui->edit_size->setText(QString::number(B) + " Bytes");
        ui->edit_mode->setText(imageFormatToString(res->curr_img.format()));
        ui->edit_geshi->setText(QString(res->fileInfo.suffix().toLower()));
        ui->edit_channels->setText(QString::number(res->root_mt.channels()) + "通道图片");
    });
    emit res->signal_updateImage();
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


void Widget::on_buttonGroup_everyOpeartions_choice(Object* op,QButtonGroup* btn_group,QAbstractButton* btn){
    if (op == blur){
        blur->current_choice = btn_group->id(btn);
    }
    else if (op == threshold){
        threshold->current_choice = btn_group->id(btn);
    }
    else if (op == morphology){
        morphology->current_choice = btn_group->id(btn);
    }
    else if (op == connected){
        connected->current_choice = btn_group->id(btn);
    }
    else if (op == contours){
        contours->current_choice = btn_group->id(btn);
    }
    else if (op == showeffect){
        showeffect->current_choice = btn_group->id(btn);
    }

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
	emit signal_choiceToolButton(btn->text(),id);
}

void Widget::on_action_exit_triggered()
{
	this->close();
}

void Widget::on_action_open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "选择文件", ".",	"图像文件(*.png *.jpg *.jpeg *.bmp *.webp)");
	if (!fileName.isEmpty()) {
        work_files.clear();
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

    clear_allButtonClicked();
}

void Widget::on_action_draw_triggered()
{
    auto widget_draw = DrawWidget::get(this);
    widget_draw->reset();
    widget_draw->show();
}

void Widget::on_colorDialog_triggered(const QColor& color)
{
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

void Widget::on_action_jie_triggered()
{
    //图片截取Widget
    look = LookWidget::get(this);
	look->reset(QPixmap::fromImage(res->curr_img));
	look->show();
}

void Widget::on_action_capture_triggered()
{
	QScreen* screen = QApplication::primaryScreen();
	QPixmap screen_lab = screen->grabWindow(0);

	//获取缩放比例
	config.win_screen_scale = screen->devicePixelRatio();

    auto all_screen = CaptureWidget::get(this);
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

void Widget::on_right_arrow_clicked(bool clicked)
{
    ui->lab_img_ori->setPixmap(ui->lab_img->pixmap());
}

void Widget::on_tbtn_allScreenHist_clicked(bool clicked)
{
    QMainWindow* w = new QMainWindow;
    QLabel* lab = new QLabel;
    lab->setPixmap(QPixmap::fromImage(img_base->showGrayHist_AdjArea("灰度直方图",w->height(),w->width())));
    w->setCentralWidget(lab);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->resize(640,480);
    w->show();
}

void Widget::on_tbtn_updateHist_clicked(bool clicked)
{
    ui->lab_hist->setPixmap(QPixmap::fromImage(img_base->showGrayHist_AdjArea("灰度直方图",ui->lab_hist->height(),ui->lab_hist->width())));
}

void Widget::on_action_equColor_triggered()
{
    img_base->showEqualizedBGRImage();
}

void Widget::on_action_equGray_triggered()
{
    img_base->showEqualizedGrayImage();
}

void Widget::on_toolbox_side_currentChanged(int value)
{
    ui->adj_stackedWidget->setCurrentWidget(ui->adjPage_first);

    preToolBoxIndex = curToolBoxIndex;
    curToolBoxIndex = value;

    //	清除之前页面上的按钮点击状态
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
}

void Widget::on_action_mark_triggered()
{
    img_base->onTriggered_picture_mask();
}

void Widget::on_action_hist_triggered()
{
    img_base->drawGrayHist("image histogram");
}

void Widget::on_action_light_triggered()
{
    config.win_theme = "light";
     QMessageBox::information(nullptr,"提示","切换亮色主题成功，请重启程序");
}

void Widget::on_action_dark_triggered()
{
    config.win_theme = "dark";
     QMessageBox::information(nullptr,"提示","切换暗色主题成功，请重启程序");
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

void Widget::on_btnWork_next_clicked(bool clicked)
{
	work_prevIndex = work_currentIndex;
	work_currentIndex++;
	if (work_currentIndex >= work_files.count()) {
		work_currentIndex = 0;
		work_prevIndex = work_files.count() - 1;
	}
    on_cbx_work_autoSave();
	work_cutImage();
}

void Widget::on_btnWork_prev_clicked(bool clicked)
{
	work_prevIndex = work_currentIndex;
	work_currentIndex--;
	if (work_currentIndex < 0) {
		work_prevIndex = 0;
		work_currentIndex = work_files.count() - 1;
	}
    on_cbx_work_autoSave();
    work_cutImage();
}

void Widget::on_lab_img_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    context_menu->exec(QCursor::pos());
}

void Widget::on_lab_img_ori_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    context_menu__->exec(QCursor::pos());
}

void Widget::clearAllAdjPageWidgetValue()
{
    for (int i=0;i<ui->adj_stackedWidget->count();i++){
        QWidget* widget = ui->adj_stackedWidget->widget(i);
        for (auto child : widget->findChildren<QSlider*>()) {
            child->setValue(0);
        }
        for (auto child : widget->findChildren<QComboBox*>()) {
            child->setCurrentIndex(0);
        }
        for (auto child : widget->findChildren<QLineEdit*>()) {
            child->clear();
        }
    }
}

void Widget::on_AvgBlur_slider1_sliderMoved(int value)
{
    if (ui->AvgBlur_slider2->value() == ui->AvgBlur_slider2->minimum() && ui->AvgBlur_slider3->value() == ui->AvgBlur_slider3->minimum()) {
        blur->anchorX = blur->anchorY = blur->avg_Ksize / 2;
    }
    blur->onTriggered_slider1_valueChange_avgBlur(value);
}

void Widget::on_AvgBlur_slider2_sliderMoved(int value)
{
    blur->onTriggered_slider2_valueChange_avgBlur(value);
}

void Widget::on_AvgBlur_slider3_sliderMoved(int value)
{
    blur->onTriggered_slider3_valueChange_avgBlur(value);
}

void Widget::on_AvgBlur_edit_returnPressed()
{
    QList<QString> lStr = ui->AvgBlur_edit->text().split(" ");
    blur->onReturnPressed_Edit(lStr);
}

void Widget::on_AvgBlur_btn_clicked(bool clicked)
{
    QString str = ui->AvgBlur_edit->text();
    int pos = 0;
    auto state = ui->AvgBlur_edit->validator()->validate(str, pos);
    //必须首先合法
    if (state == QValidator::Acceptable) {
        on_AvgBlur_edit_returnPressed();
    }
}

void Widget::on_GaussBlur_slider1_sliderMoved(int value)
{
    if (value % 2 == 0) {
        value += 1;
    }
    if (ui->GaussBlur_slider2->value() == ui->GaussBlur_slider2->minimum() && ui->GaussBlur_slider3->value() == ui->GaussBlur_slider3->minimum()) {
        blur->sigmaX = blur->sigmaY = blur->gas_Ksize / 2;
    }
    blur->onTriggered_slider1_valueChange_gaussianBlur(value);
}

void Widget::on_GaussBlur_slider2_sliderMoved(int value)
{
    blur->onTriggered_slider2_valueChange_gaussianBlur(value);
}

void Widget::on_GaussBlur_slider3_sliderMoved(int value)
{
    blur->onTriggered_slider3_valueChange_gaussianBlur(value);
}

void Widget::on_GaussBlur_edit_returnPressed()
{
    QList<QString> lStr = ui->GaussBlur_edit->text().split(" ");
    blur->onReturnPressed_Edit(lStr);
}

void Widget::on_GaussBlur_btn_clicked(bool clicked)
{
    QString str = ui->GaussBlur_edit->text();
    int pos = 0;
    auto state = ui->GaussBlur_edit->validator()->validate(str, pos);
    //必须首先合法
    if (state == QValidator::Acceptable) {
        on_GaussBlur_edit_returnPressed();
    }
}

void Widget::on_MedianBlur_slider1_sliderMoved(int value)
{
    if (value % 2 == 0)
        value += 1;
    blur->onTriggered_slider_valueChange_medianBlur(value);
}

void Widget::on_MedianBlur_edit_returnPressed()
{
    QList<QString> lStr = ui->MedianBlur_edit->text().split(" ");
    blur->onReturnPressed_Edit(lStr);
}

void Widget::on_MedianBlur_btn_clicked(bool clicked)
{
    QString str = ui->MedianBlur_edit->text();
    int pos = 0;
    auto state = ui->MedianBlur_edit->validator()->validate(str, pos);
    //必须首先合法
    if (state == QValidator::Acceptable) {
        on_MedianBlur_edit_returnPressed();
    }
}

void Widget::on_BiltBlur_slider1_sliderMoved(int value)
{
    blur->onTriggered_slider1_valueChange_bilateralBlur(value);
}

void Widget::on_BiltBlur_slider2_sliderMoved(int value)
{
    blur->onTriggered_slider2_valueChange_bilateralBlur(value);
}

void Widget::on_BiltBlur_slider3_sliderMoved(int value)
{
    blur->onTriggered_slider3_valueChange_bilateralBlur(value);
}

void Widget::on_BiltBlur_edit_returnPressed()
{
    QList<QString> lStr = ui->BiltBlur_edit->text().split(" ");
    blur->onReturnPressed_Edit(lStr);
}

void Widget::on_BiltBlur_btn_clicked(bool clicked)
{
    QString str = ui->BiltBlur_edit->text();
    int pos = 0;
    auto state = ui->BiltBlur_edit->validator()->validate(str, pos);
    //必须首先合法
    if (state == QValidator::Acceptable) {
        on_BiltBlur_edit_returnPressed();
    }
}

void Widget::on_Threshold_slider1_sliderMoved(int value)
{
    qInfo()<<"yes 1" << value;
    threshold->onTriggered_slider1_valueChanged_thresholdValue(value);
}

void Widget::on_Threshold_slider2_sliderMoved(int value)
{
    qInfo()<<"yes 2" << value;
    threshold->onTriggered_slider2_valueChanged_maxValue(value);
}

void Widget::on_Threshold_edit_returnPressed()
{
    QList<QString> lStr = ui->Threshold_edit->text().split(" ");
    threshold->onReturnPressed_Edit(lStr);
}

void Widget::on_Threshold_btn_clicked(bool clicked)
{
    QString str = ui->Threshold_edit->text();
    int pos = 0;
    auto state = ui->Threshold_edit->validator()->validate(str, pos);
    //必须首先合法
    if (state == QValidator::Acceptable) {
        on_Threshold_edit_returnPressed();
    }
}

void Widget::on_Morphology_slider1_sliderMoved(int value)
{
    morphology->onTriggered_slider1_valueChanged_kernel(value);
}

void Widget::on_Morphology_slider2_sliderMoved(int value)
{
    morphology->onTriggered_slider2_valueChanged_anchorX(value);
}

void Widget::on_Morphology_slider3_sliderMoved(int value)
{
    morphology->onTriggered_slider3_valueChanged_anchorY(value);
}

void Widget::on_Morphology_edit_returnPressed()
{
    QList<QString> lStr = ui->Morphology_edit->text().split(" ");
    morphology->onReturnPressed_Edit(lStr);
}

void Widget::on_Morphology_btn_clicked(bool clicked)
{
    QString str = ui->Morphology_edit->text();
    int pos = 0;
    auto state = ui->Morphology_edit->validator()->validate(str, pos);
    //必须首先合法
    if (state == QValidator::Acceptable) {
        on_Morphology_edit_returnPressed();
    }
}

void Widget::on_Connected_cbx1_activated(int index)
{
    connected->onTriggered_Comb2_currentTextChanged_ccltype(index);
}

void Widget::on_Connected_cbx2_activated(int index)
{
    connected->onTriggered_Comb2_currentTextChanged_ccltype(index);
}

void Widget::on_Contours_cbx1_activated(int index)
{
    contours->onTriggered_Comb1_currentTextChanged_contoursMode(index);
}

void Widget::on_Contours_cbx2_activated(int index)
{
    contours->onTriggered_Comb2_currentTextChanged_contoursMethod(index);
}

void Widget::on_Contours_cbx3_activated(int index)
{
    contours->onTriggered_Comb3_currentTextChanged_contoursThick(index);
}

void Widget::on_Bright_slider1_sliderMoved(int value)
{
    ui->Bright_slider2->setValue(ui->Bright_slider2->minimum());
    showeffect->onTriggered_slider_valueChange_brighten(value);
}

void Widget::on_Bright_slider2_sliderMoved(int value)
{
    value = -value;
    ui->Bright_slider1->setValue(ui->Bright_slider1->minimum());
    showeffect->onTriggered_slider_valueChange_brighten(value);
}

void Widget::on_Bright_edit_returnPressed()
{
    QList<QString> lStr = ui->Bright_edit->text().split(" ");
    showeffect->onReturnPressed_Edit(lStr);
}

void Widget::on_gamma_slider1_sliderMoved(int value)
{
    //将int映射为double
    double d_val = value * 1.0 / 10.0;
    showeffect->onTriggered_slider_valueChange_gamma(d_val);
}

void Widget::on_gamma_edit_returnPressed()
{
    QList<QString> lStr = ui->gamma_edit->text().split(" ");
    showeffect->onReturnPressed_Edit(lStr);
}

void Widget::on_Contrast_slider1_sliderMoved(int value)
{
    showeffect->onTriggered_slider_valueChange_linearg1(value);
}

void Widget::on_Contrast_slider2_sliderMoved(int value)
{
    showeffect->onTriggered_slider_valueChange_linearg2(value);
}

void Widget::on_Contrast_edit_returnPressed()
{
    QList<QString> lStr = ui->Contrast_edit->text().split(" ");
    showeffect->onReturnPressed_Edit(lStr);
}

void Widget::on_Contrast_rbtn1_clicked(bool clicked)
{
    showeffect->linear_mode = 0; //灰度图
}

void Widget::on_Contrast_rbtn2_clicked(bool clicked)
{
    showeffect->linear_mode = 1;
}

void Widget::on_GrayWindow_slider1_sliderMoved(int value)
{
    showeffect->onTriggered_slider_valueChange_GaryWindowF1(value);
}

void Widget::on_GrayWindow_slider2_sliderMoved(int value)
{
    showeffect->onTriggered_slider_valueChange_GaryWindowF2(value);
}

void Widget::on_GrayWindow_edit_returnPressed()
{
    QList<QString> lStr = ui->GrayWindow_edit->text().split(" ");
    showeffect->onReturnPressed_Edit(lStr);
}

void Widget::on_DpLinear_slider1_sliderMoved(int value)
{
    showeffect->onTriggered_slider_valueChange_DynamicA(value);
}

void Widget::on_DpLinear_slider2_sliderMoved(int value)
{
    showeffect->onTriggered_slider_valueChange_DynamicB(value);
}

void Widget::on_DpLinear_edit_returnPressed()
{
    QList<QString> lStr = ui->DpLinear_edit->text().split(" ");
    showeffect->onReturnPressed_Edit(lStr);
}

void Widget::on_DpLinear_rbtn1_clicked(bool clicked)
{
    showeffect->DpLinear_mode = 0; //灰度图
}

void Widget::on_DpLinear_rbtn2_clicked(bool clicked)
{
    showeffect->DpLinear_mode = 1;
}

void Widget::on_NdpLinear_slider1_sliderMoved(int value)
{
    showeffect->onTriggered_slider_valueChange_NoneDynamicC(value);
}

void Widget::on_NdpLinear_edit_returnPressed()
{
    QList<QString> lStr = ui->NdpLinear_edit->text().split(" ");
    showeffect->onReturnPressed_Edit(lStr);
}

void Widget::on_NdpLinear_rbtn1_clicked(bool clicked)
{
    showeffect->NoneDpLinear_mode = 0; //灰度图
}

void Widget::on_NdpLinear_rbtn2_clicked(bool clicked)
{
    showeffect->NoneDpLinear_mode = 1;
}

void Widget::clear_allButtonClicked()
{
    for (auto& btnGps : btngroups) {
        btnGps->setExclusive(false);
        for (auto& x : btnGps->buttons()) {
            x->setChecked(false);
        }
        btnGps->setExclusive(true);
    }
}

void Widget::reload_Resources_ScrollArea(const QString& fileName, int mode)
{
    Q_UNUSED(mode);
	res->reset(fileName.toLocal8Bit().data());

	//操作图片尺寸更新
	scaledDelta = ori_scaledDelta = init_scaledImageOk();
	//原始图片尺寸更新
	update_Image_1(ori_scaledDelta);

    clear_allButtonClicked();
	//清除之前的保存的撤销图片
	while (!undo_sta.empty()) {
		undo_sta.pop();
	}

    //打开新图片更新灰度直方图
    ui->lab_hist->setPixmap(QPixmap::fromImage(img_base->showGrayHist_AdjArea(QString("灰度直方图"),ui->lab_hist->height(),ui->lab_hist->width())));
}

void Widget::on_cbx_work_autoSave()
{
    //在切换页面的时候弹出保存提示
    if (ui->cbxWork_autoSave->isChecked()) {
        on_action_save_triggered();
	}
}

bool Widget::loadImagesFormFloder(const QString& floderPath)
{
	QDir dir(floderPath);
    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.webp"; // 支持的图片格式
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
	if (mode) {
        //仅仅在创作者模式下生效：触发时会将图片置回到未操作前的位置
		returnPoint();
		update_Image(ori_scaledDelta);
        clear_allButtonClicked();
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

void Widget::init_CustomAction()
{
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
}

void Widget::init_CustomMenu()
{
	//图片上下文菜单
	//此处跟随滑动区域，滑动区域在整个窗口中作为主窗口不会消失。
    context_menu = new QMenu(this);
    context_menu->addAction(ui->action_disp);
    context_menu->addAction(ui->action_save);
    context_menu->addAction(ui->action_exit);
    context_menu->addAction(ui->action_restore);

	context_menu__ = new QMenu(this);
    context_menu__->addAction(ui->action_hide);
    context_menu__->addAction(ui->action_exit);
}


void Widget::init_ToolBoxSide()
{
	//-----------------模糊操作-----------------------------
	btngroups.push_back(btngroup_blur = new QButtonGroup(this));
	btngroup_blur->setExclusive(true);
	//连接信号
	connect(btngroup_blur, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
        //点击按钮创建抽象操作类，运行时动态加载
        on_buttonGroup_everyOpeartions_choice(blur, btngroup_blur, btn);
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
        on_buttonGroup_everyOpeartions_choice(threshold, btngroup_threshold, btn);
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
        on_buttonGroup_everyOpeartions_choice(morphology, btngroup_form, btn);
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
        on_buttonGroup_everyOpeartions_choice(connected, btngroup_connected, btn);
		});

    btngroup_connected->addButton(ui->one_connected_btn,CONNECTED::CONNECTED_TYPE1);
    btngroup_connected->addButton(ui->two_connected_btn,CONNECTED::CONNECTED_TYPE2);

	//-----------------轮廓绘制操作-----------------------------
	btngroups.push_back(btngroup_contours = new QButtonGroup(this));
	btngroup_contours->setExclusive(true);

    connect(btngroup_contours, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
        on_buttonGroup_everyOpeartions_choice(contours, btngroup_contours, btn);
		});

    btngroup_contours->addButton(ui->draw_contours_btn,CONTOURS::CONTOURS_TYPE1);

	//-----------------图像效果增强------------------------------
	btngroups.push_back(btngroup_show = new QButtonGroup(this));
	btngroup_show->setExclusive(true);

	connect(btngroup_show, &QButtonGroup::buttonClicked, this, [=](QAbstractButton* btn) {
        on_buttonGroup_everyOpeartions_choice(showeffect, btngroup_show, btn);
		});

    btngroup_show->addButton(ui->bright_effect_btn,SHOW::LIGHT);
    btngroup_show->addButton(ui->gamma_effect_btn,SHOW::GAMMA);
    btngroup_show->addButton(ui->linear_effect_btn,SHOW::LINEAR);
    btngroup_show->addButton(ui->gray_effect_btn,SHOW::GRAYWINDOW);
    btngroup_show->addButton(ui->DpLinear_effect_btn,SHOW::DPLINEAR);
    btngroup_show->addButton(ui->nonDpLinear_effect_btn,SHOW::NON_DPLINEAR);
}

void Widget::init_CustomStatusBar()
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
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+\\s\\d+\\s\\d+"), ui->AvgBlur_edit);
    ui->AvgBlur_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIGaussian()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+\\s\\d+\\s\\d+"), ui->GaussBlur_edit);
    ui->GaussBlur_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIMedian()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+"), ui->MedianBlur_edit);
    ui->MedianBlur_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIBilateral()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+\\s\\d+\\s\\d+"), ui->BiltBlur_edit);
    ui->BiltBlur_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIThreshold()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+\\s\\d+"), ui->Threshold_edit);
    ui->Threshold_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIMorphology()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+\\s\\d+\\s\\d+\\s\\d+"), ui->Morphology_edit);
    ui->Morphology_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIConnected()
{
    return 	nullptr;
}

QWidget* Widget::create_GUIContours()
{
    //"更换颜色" "绘制图像凸包"

    return nullptr;
}

QWidget* Widget::create_GUIbright()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("-?\\d+"), ui->Bright_edit);
    ui->Bright_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIgamma()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+"), ui->gamma_edit);
    ui->gamma_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIContrast()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+\\s\\d+"), ui->Contrast_edit);
    ui->Contrast_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIGrayWindow()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+\\s\\d+"), ui->GrayWindow_edit);
    ui->GrayWindow_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUIDPLinear()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+\\s\\d+"), ui->DpLinear_edit);
    ui->DpLinear_edit->setValidator(validator);

    return nullptr;
}

QWidget* Widget::create_GUINoneDpLinear()
{
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+"), ui->NdpLinear_edit);
    ui->NdpLinear_edit->setValidator(validator);

    return nullptr;
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
    double wDelta = static_cast<double>((double)ui->scrollArea->size().width() / (double)res->curr_img.size().width());
    double hDelta = static_cast<double>((double)ui->scrollArea->size().height() / (double)res->curr_img.size().height());
	auto t_scaledDelta = qMin(wDelta, hDelta);

	update_Image(t_scaledDelta);

	return t_scaledDelta;
}
