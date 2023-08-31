#ifndef WIDGET_H_
#define WIDGET_H_

#include <QMainWindow>
#include <opencv2/opencv.hpp>
class QAbstractButton;
class QButtonGroup;
class QGridLayout;
class QToolButton;
class QToolBox;
class Label;
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

class Widget :public QMainWindow {
	Q_OBJECT
private:
	//单例设计模式
	Widget(QMainWindow* parent = nullptr);
public:	
	//返回实例
	static Widget* getInstance();
	~Widget();
public:
	//操作对象
	void initFunction();

	void createAction();
	void createMenu();
	void createToolBar();
	void createToolBox();
	void createStatusBar();

	QWidget* createToolBtnItemWidget(const QString& text, int id, const QString& fileName = "");

	//GUI创建
	QWidget* create_GUIAvgBlur();		//均值滤波
	QWidget* create_GUIGaussianBlur();	//高斯滤波
	QWidget* create_GUIMedianBlur();	//中值滤波
	QWidget* create_GUIBilateralBlur(); //双边滤波
	QWidget* create_GUIThreshoild();	//阈值化
	QWidget* create_GUIMorphology();	//形态学
	QWidget* create_GUIConnected();		//连通块分析
	QWidget* create_GUIContours();		//轮廓检测
	QWidget* create_GUIChangeImg();		//颜色模型转换
public slots:
	void onClicked_buttonGroup_blur(QAbstractButton* btn);
	void onClicked_buttonGroup_threshold(QAbstractButton* btn);
	void onClicked_buttonGroup_morphology(QAbstractButton* btn);
	void onClicked_buttonGroup_connected(QAbstractButton* btn);
	void onClicked_buttonGroup_contours(QAbstractButton* btn);

	void onTriggered_actionGroup(QAction* action);

	//打开新图片
	void onTriggered_action_openFile();

	//保存当前图片: lab_img
	void onTriggered_action_saveFile();

	//重置所有操作至原始状态
	void onTriggered_action_allRestore();

	//确定加载预览图片到主图片
	void onTriggered_action_previewToNormal();

	//选择颜色
	void onTriggered_ColorDialog_choice(const QColor& color);

	//非加工模式下：切换不同的操作时，清除原来的操作
	void restore_cutOperation();

	//开启图片加工模式
	void onTriggered_action_process();

	//撤销此次操作
	void onTriggered_action_undo();

	//图片数据还原原始状态: ori_mt
	void dataClear();

	//保存图片，设置保存点
	void savePoint();

	//读取保存点
	void returnPoint();

	void choiceToolBtnFalse();

private: //辅助函数
	//清除某一页参数的数值
	void setIndexPageWidgetValue(int index = -1);
	void clearAllWidgetValue();
	void loadANewPicture(const cv::Mat& new_mt);

public:
	cv::Mat ori_mt; //保存原始的加载的图片
	QImage ori_img;
	//cv::Mat temp_mt;//如果是创作者模式，则需要保存每次更新后的图片


	cv::Mat savePoint_mt;
	cv::Mat mt;

	cv::Mat origin_convert; //转换前的原图

	QImage img;

	QLabel* lab_img = nullptr;
	Label* sub_lab_img = nullptr; //预览图片
	static Widget* widget;
	bool mode = false;
private:

	QAction* action_exit = nullptr;
	QAction* action_open = nullptr;
	QAction* action_save = nullptr;
	QAction* action_restore = nullptr;
	QAction* action_begin = nullptr;
	QAction* action_return = nullptr;
	QAction* action_preview = nullptr;

	QAction* action_hls = nullptr;
	QAction* action_rgb = nullptr;
	QAction* action_hsv = nullptr;
	QAction* action_lab = nullptr;
	QActionGroup* action_group = nullptr;

	//上下文菜单
	QMenu* context_menu = nullptr;
	//主菜单
	QMenu* menu_file = nullptr;
	QMenu* menu_edit = nullptr;
	QMenu* menu_convert = nullptr;

	QToolBar* toolbar1 = nullptr;

	QToolBox* toolbox_side = nullptr;
	int preToolBoxIndex = 0, curToolBoxIndex = 0;

	int now = -1; //全局定位点，定位在哪个功能位置

	QLabel* statusLab = nullptr;
	

	QButtonGroup* btngroup_blur = nullptr;
	QButtonGroup* btngroup_threshold = nullptr;
	QButtonGroup* btngroup_form = nullptr;
	QButtonGroup* btngroup_connected = nullptr;
	QButtonGroup* btngroup_contours = nullptr;
	QButtonGroup* btngroup_cvtColor = nullptr;

	QList<QButtonGroup*> btngroups;

	QFileDialog* fileDialog = nullptr;
	QColorDialog* colorDialog = nullptr;

	//窗口布局
	QVBoxLayout* vlayout_right = nullptr;
	QStackedWidget* stack_tools = nullptr;

	//---------------功能实现-----------------
	Blur* blur = nullptr; //模糊
	Threshold* threshold = nullptr;
	Morphology* morphology = nullptr;
	Connected* connected = nullptr;
	Contours* contours = nullptr;
	QList<Object*> ls;
};


#endif
