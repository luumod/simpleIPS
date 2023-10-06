#ifndef WIDGET_H_
#define WIDGET_H_
#define FUNCTION_
#include <QMainWindow>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stack>
#include <QDebug>
#if 1 
class QAbstractButton;
class QButtonGroup;
class QGridLayout;
class QToolButton;
class QToolBox;
class Main_Label;	//主图片
class Sub_widget;	//预览图片
class QLabel;
class QAction;
class QStackedWidget;
class QVBoxLayout;
class Object;
class Blur;
class Threshold;
class Morphology;
class Connected;
class Contours;
class QToolBar;
class QFileDialog;
class QColorDialog;
class CvtColor;
class QActionGroup;
class QHBoxLayout;
class DrawWidget; 
class BaseOperate;
class QMouseEvent;
class Showeffect;
class QScrollArea;
class QPushButton;
class QCheckBox;
class QWheelEvent;
class LookWidget;
class Res;
#endif

struct ExeConfig {
	QString win_title;
	int win_location_x, win_location_y;
	QString win_theme;
	friend QDebug operator<<(QDebug debug, const ExeConfig& oth) {
		QDebugStateSaver saver(debug);
		debug << "win_title: " << oth.win_title << '\n';
		debug << "win_location_x: " << oth.win_location_x << '\n';
		debug << "win_location_y: " << oth.win_location_y << '\n';
		debug << "win_theme: " << oth.win_theme << '\n';
		return debug;
	}
};



class Widget :public QMainWindow {
	Q_OBJECT
private:
	static Widget* widget;
	//单例设计模式
	Widget(QMainWindow* parent = nullptr);
public:	
	//返回实例
	static Widget* getInstance();
	~Widget();
public:
	void init_WidgetInfo();

	//解析json配置文件
	void init_readJson();

	//构造函数中初始化两个Label，注意：这个函数只是为了封装，只会调用一次
	void init_Label();

	//设置每个操作的具体选项数值
	void init_Optsdialog();

	//初始化所有opencv操作对象
	void init_OpencvFunctions();

	//设置窗口主布局
	void init_WidgetLayout();

	//更新图片显示到任意的缩放比例，如果为ori_scaledDelta，则为完美比例，否则为自定义缩放比例
	void update_Image(double scaledDelta);

	//初始加载时图片必须被完全看见，需要预缩放
	double init_scaledImageOk();
protected:
	//鼠标点击时自动关闭对话框Dialog
	void mousePressEvent(QMouseEvent* ev)override;

	//移动窗口获取当前左上角坐标
	void moveEvent(QMoveEvent* ev)override;

	//Ctrl+滑轮 缩放图片
	void wheelEvent(QWheelEvent* ev)override;
public:
	void createAction();
	void createMenu();
	void createToolBar();
	void createToolBox();
	void createStatusBar();

	//GUI创建
	QHBoxLayout* create_GUIAvgBlur();		//均值滤波
	QHBoxLayout* create_GUIGaussianBlur();	//高斯滤波
	QHBoxLayout* create_GUIMedianBlur();	//中值滤波
	QHBoxLayout* create_GUIBilateralBlur(); //双边滤波
	QHBoxLayout* create_GUIThreshold();		//阈值化
	QHBoxLayout* create_GUIMorphology();	//形态学
	QHBoxLayout* create_GUIConnected();		//连通块分析
	QHBoxLayout* create_GUIContours();		//轮廓检测
	QVBoxLayout* create_GUIShow();			//效果增强
public slots:
	//右键图片操作
	void on_label_customContextMenuRequested(const QPoint& pos);

	//模糊操作
	void on_bttuonGroup_blur_clicked(QAbstractButton* btn);

	//阈值操作
	void on_bttuonGroup_threshold_clicked(QAbstractButton* btn);
	
	//形态学操作
	void on_bttuonGroup_morphology_clicked(QAbstractButton* btn);

	//连通性分析
	void on_bttuonGroup_connected_clicked(QAbstractButton* btn);

	//轮廓检测
	void on_bttuonGroup_contours_clicked(QAbstractButton* btn);

	//图像显示操作
	void on_bttuonGroup_show_clicked(QAbstractButton* btn);

	//关闭窗口
	void on_action_exit_triggered();

	//打开新图片
	void on_action_openFile_triggered();

	//打开文件夹
	void on_action_openWorks_triggered();

	//保存当前图片到文件夹中: lab_img
	void on_action_saveFile_triggered();

	//重置所有操作至原始状态
	void on_action_allRestore_triggered();

	//确定加载预览图片到主图片
	void on_action_previewToNormal_triggered();

	//简单绘图板
	void on_action_drawBoard_triggered();

	//开启图片加工模式
	void on_action_changeMode_triggered();

	//撤销此次操作
	void on_action_undo_triggered();

	//选择颜色
	void on_colorDialog_choice_triggered(const QColor& color);

	//切换图片的格式：LAB  HSV 等格式
	void on_actionGroup_cvtColor_triggered(QAction* action);

	//图片的旋转
	void on_actionGroup_rotate_triggered(QAction* action);

	//图片的翻转
	void on_actionGroup_flip_triggered(QAction* action);
	
	//主题切换
	void on_action_theme_triggered(int type);

	//图片截取
	void on_action_jie_triggered();

	//获取图片信息
	void on_action_fileInfo_triggered();

	//查看帮助
	void on_actionGroup_help_triggered(QAction* action);

	//工作区：下一页
	void on_pushButton_next_clicked();

	//工作区：上一页
	void on_pushButton_prev_clicked();

	//工作区：自动保存
	void on_checkBox_LeaveAutoSave_clicked();
public:
	//打开文件夹保存图片信息
	bool loadImagesFormFloder(const QString& floderPath);

	//当点击每个操作的时候，判断是否处于加工状态
	void choice_buttonGroupsBtns();

	//非加工模式下：切换不同的操作时，清除原来的操作
	void restore_cutOperation();

	//保存图片，设置保存点
	void savePoint();

	//读取保存点
	void returnPoint();

	//当点击一个Dialog后，隐藏其他的QDialog，只会显示一个
	void hideAllDialog(QDialog* currDialog);

	//依照inter_mt更新所有图片数据（不包括对图像格式的改变与旋转等基础操作造成的改变）
	void updateFromIntermediate(); 

	//依照root_mt更新所有图片数据：相当于清除所有操作
	void updateFromRoot();

	//切换到工作区的布局（多张图片）
	void layout_changeToWork();

	//切换到普通的布局
	void layout_changeToNormal();
private FUNCTION_: //辅助函数
	//获取所有的指定的QSlider 控件并且清除值
	void setIndexPageWidgetValue(int index = -1);

	//清除所有页面对话框数据
	void clearAllWidgetValue();
	
	//创建ToolBox中的每个ToolButton
	QWidget* createToolBtnItemWidget(const QString& text, int id, const QString& fileName = "");

	//创建每种对话框种的的输入数值方式
	template <typename Type>
	QHBoxLayout* create_Edit_hLayout(const QString& filter, const QString& text, Type* t);

	//工作区：切换图片
	void work_cutImage();
public:
	//配置文件
	ExeConfig config;
	//图片资源
	Res* res = nullptr;

	//主图片
	Main_Label* lab_img = nullptr;

	//图像截取的预览图片
	LookWidget* look = nullptr;

	//混合加工模式
	bool mode = false;

	//鼠标滑轮控制
	double ori_scaledDelta = 1.0; //原始完美缩放比例
	double scaledDelta = 1.0;
private:
	//撤销栈
	std::stack<cv::Mat> undo_sta;

	DrawWidget* widget_draw = nullptr; //一个简单的绘图板

	QAction* action_exit = nullptr;
	QAction* action_open = nullptr;
	QAction* action_works = nullptr;
	QAction* action_save = nullptr;
	QAction* action_restore = nullptr;
	QAction* action_begin = nullptr;
	QAction* action_return = nullptr;
	QAction* action_previewOk = nullptr;
public:

	QAction* action_ori = nullptr;
	QAction* action_hls = nullptr;
	QAction* action_rgb = nullptr;
	QAction* action_hsv = nullptr;
	QAction* action_lab = nullptr;
	QActionGroup* action_cvtColor_group = nullptr;

	QAction* action_draw = nullptr;
	//翻转
	QAction* action_flip0 = nullptr;
	QAction* action_flip1 = nullptr;
	QAction* action_flip_1 = nullptr;
	QActionGroup* action_flip_group = nullptr;

	//旋转
	QAction* action_right90 = nullptr;
	QAction* action_right180 = nullptr;
	QAction* action_right270 = nullptr;
	QActionGroup* action_rotate_group = nullptr;

	//掩膜
	QAction* action_mark = nullptr;

	//功能
	QAction* action_hist = nullptr;
	QAction* action_get_equ = nullptr;
	QAction* action_fileInfo = nullptr;

	//扩展
	QAction* action_light = nullptr;
	QAction* action_dark = nullptr;
	QAction* action_jie = nullptr;
	
	//帮助
	QAction* action_help = nullptr;
	QAction* action_aboutme = nullptr;
	QActionGroup* action_help_group = nullptr;

private:
	//上下文菜单
	QMenu* context_menu = nullptr;
	//主菜单
	QMenu* menu_file = nullptr;
	QMenu* menu_edit = nullptr;
	QMenu* menu_convert = nullptr;
	QMenu* menu_reverse = nullptr;
	QMenu* menu_flip = nullptr;
	QMenu* menu_mark = nullptr;
	QMenu* menu_func = nullptr;
	QMenu* menu_tools = nullptr;
	QMenu* menu_help = nullptr;
	

	QToolBar* toolbar1 = nullptr;

	QToolBox* toolbox_side = nullptr;
	int preToolBoxIndex = 0, curToolBoxIndex = 0; //切换toolbox页面时清除选择状态

	QLabel* statusLab = nullptr;
	

	QButtonGroup* btngroup_blur = nullptr;
	QButtonGroup* btngroup_threshold = nullptr;
	QButtonGroup* btngroup_form = nullptr;
	QButtonGroup* btngroup_connected = nullptr;
	QButtonGroup* btngroup_contours = nullptr;
	QButtonGroup* btngroup_cvtColor = nullptr;
	QButtonGroup* btngroup_show = nullptr;
	QList<QButtonGroup*> btngroups;

	//所有对话框操作
	//0：blur  1: gauss  2: median  3: Bilateral
	//4: threshold  5：morphology 6: connected  7:contours  8:show
	QList<QDialog*> all_dlg;


	QFileDialog* fileDialog = nullptr;
	QColorDialog* colorDialog = nullptr;

	//---------------功能实现-----------------
	Blur* blur = nullptr; //模糊
	Threshold* threshold = nullptr;
	Morphology* morphology = nullptr;
	Connected* connected = nullptr;
	Contours* contours = nullptr;
	Showeffect* showeffect = nullptr;
	BaseOperate* img_base = nullptr; //图像基础操作
	QList<Object*> Opts;


	//----------------滚动页面--------------
	QScrollArea* scrollArea = nullptr;

	//打开文件夹
	QList<cv::Mat*> wid_stacked;
	QPushButton* btn_work_next = nullptr;
	QPushButton* btn_work_prev = nullptr;
	QCheckBox* cbx_work_autoSave = nullptr;
	QHBoxLayout* btn_work_layout;
public:
	QStringList	work_files;
	int work_currentIndex = 0, work_prevIndex = 0;
};


#endif
