#ifndef WIDGET_H_
#define WIDGET_H_
#define FUNCTION_
#include "assist/config.h"
#include <QMainWindow>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stack>
#include <QDebug>
#include <functional>
#if 1 
class QAbstractButton;
class QButtonGroup;
class QGridLayout;
class QToolButton;
class QToolBox;
class QLabel;	//主图片
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
class CaptureWidget;
class QBoxLayout;
class QSlider;
class QComboBox;
#endif

class Widget :public QMainWindow {
	Q_OBJECT
private:
	static Widget* widget;
	//单例设计模式
	Widget(QWidget* parent = nullptr);
public:	
	//返回实例
	static Widget* getInstance();
	~Widget();
public:
	void init_WidgetInfo();

	//解析json配置文件
	void init_readJson();

	//构造函数中初始化两个Label
	void init_Label();

	//初始化所有opencv操作对象
	void init_OpencvFunctions();

	//设置窗口主布局
	void init_WidgetLayout();

	//更新图片显示到任意的缩放比例，如果为ori_scaledDelta，则为完美比例，否则为自定义缩放比例
	void update_Image(double scaledDelta, const QPointF& pf = QPointF());

	//初始加载时图片必须被完全看见，需要预缩放
	double init_scaledImageOk();
protected:
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
	QWidget* create_GUIAvgBlur();
	QWidget* create_GUIGaussian();		
	QWidget* create_GUIMedian();		
	QWidget* create_GUIBilateral();		
	QWidget* create_GUIThreshold();
	QWidget* create_GUIMorphology();
	QWidget* create_GUIConnected();	
	QWidget* create_GUIContours();		
	QWidget* create_GUIbright();		
	QWidget* create_GUIgamma();		

	QWidget* choice_GUI_create(int id);

public slots:
	//对于所有的opencv操作按钮，封装为此一个函数，此函数可以用于传递指定的op操作，传递对应op操作的按钮组，传递当前按下的按钮：即可完成对于指定行为的响应。
	//调用示例：on_buttonGroup_everyOpeartions_choice(blur,btngroup_blur,btn);
	//用在createToolBox中的槽函数
	void on_buttonGroup_everyOpeartions_choice(Object*& op, QButtonGroup* btn_group, QAbstractButton* btn);

	//右键图片操作
	void on_label_customContextMenuRequested(const QPoint& pos);

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

	//桌面截图
	void on_action_capture_triggered();

	//获取图片信息
	QWidget* on_action_fileInfo_triggered();

	//查看帮助
	void on_actionGroup_help_triggered(QAction* action);

	//工作区：下一页
	void on_pushButton_next_clicked();

	//工作区：上一页
	void on_pushButton_prev_clicked();

	//工作区：自动保存
	void on_checkBox_LeaveAutoSave_clicked();
public:
	//加载/重新加载图片资源并且重置场景
	//mode=0：加载单个图片
	//mode!=0：加载工作区
	void reload_Resources_ScrollArea(const QString& fileName, int mode = 0);

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

	//Dialog: 创建n个滑块和输入框
	template <typename T, typename Type>
	QBoxLayout* createDialog_nSlider_GUItemplate(
		QList<QSlider*>& ls_slider,
		QList<T> low, QList<T> high, QList<T> step,
		QList< QString> objectName,
		QList< QString> lab_name,
		QList<std::function<void(int)>> slotFunction,
		bool edit = false,
		const QString& filter = "",
		const QString& text = "",
		Type* t = nullptr);

	//Dialog: 创建n个选择框
	QBoxLayout* createDialog_nComBox_GUItemplate(
		QList<QComboBox*>& ls_combox,
		QList<QStringList> ls_item,
		QList< QString> objectName,
		QList< QString> lab_name,
		QList<std::function<void(int)>> slotFunction);
public:
	//配置文件
	ExeConfig config;
	//图片资源
	Res* res = nullptr;

	//主图片
	QLabel* lab_img = nullptr;

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
	QAction* action_get_equalizeImg = nullptr;
	QAction* action_fileInfo = nullptr;

	//扩展
	QAction* action_light = nullptr;
	QAction* action_dark = nullptr;
	QAction* action_jie = nullptr;
	QAction* action_capture = nullptr;
	
	//帮助
	QAction* action_help = nullptr;
	QAction* action_aboutme = nullptr;
	QActionGroup* action_help_group = nullptr;


	//----------------滚动页面--------------
	QScrollArea* scrollArea = nullptr;
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
	QButtonGroup* btngroup_show = nullptr;
	QList<QButtonGroup*> btngroups;

	//所有对话框操作
	//0：blur  1: gauss  2: median  3: Bilateral
	//4: threshold  5：morphology 6: connected  7:contours  8:show

	QStackedWidget*  stacked_widgets = nullptr;
	QTabWidget* tab_widgets = nullptr;

	//QDialog* dlg_avgBlur = nullptr;
	//QDialog* dlg_gauss = nullptr;
	//QDialog* dlg_median  = nullptr;
	//QDialog* dlg_Bilateral = nullptr;
	//QDialog* dlg_threshold = nullptr;
	//QDialog* dlg_morphology = nullptr;
	//QDialog* dlg_connected = nullptr;
	//QDialog* dlg_contours = nullptr;
	//QDialog* dlg_bright = nullptr;
	//QDialog* dlg_gamma = nullptr;

	//QList<QDialog*> all_dlgs;
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

	//打开文件夹
	QPushButton* btn_work_next = nullptr;
	QPushButton* btn_work_prev = nullptr;
	QCheckBox* cbx_work_autoSave = nullptr;
	QHBoxLayout* btn_work_layout;
	
	//桌面截图
	CaptureWidget* all_screen = nullptr;

	//gamma
	//需要的时候创建，而不是一开始就创建
	QList<QSlider*> ls_slider_blur;
	QList<QSlider*> ls_slider_gaussian;
	QList<QSlider*> ls_slider_median;
	QList<QSlider*> ls_slider_bilateral;

	QList<QSlider*> ls_slider_threshold;
	QList<QSlider*> ls_slider_morphology;

	QList<QComboBox*> ls_combox_connected;
	QList<QComboBox*> ls_combox_contours;

	QList<QSlider*> ls_slider_light;
	QList<QSlider*> ls_slider_gamma;

public:
	QStringList	work_files; //打开工作区的图片名称组
	int work_currentIndex = 0, work_prevIndex = 0;
};


#endif
