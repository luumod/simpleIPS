#ifndef WIDGET_H_
#define WIDGET_H_

#include <QMainWindow>
#include <opencv2/opencv.hpp>
class QAbstractButton;
class QButtonGroup;
class QGridLayout;
class QToolButton;
class QToolBox;
class QLabel;
class QAction;
class QStackedWidget;
class QVBoxLayout;

class Object;
class Blur;
class Threshold;
class Morphology;
class Connected;

class QToolBar;
class QFileDialog;

class Widget :public QMainWindow {
	Q_OBJECT
public:
	Widget(QWidget* parent = nullptr);
	~Widget();
public:
	void initFunction();
	void createAction();
	void createMenu();
	void createToolBar();
	void createToolBox();

	QWidget* createToolBtnItemWidget(const QString& text, int id);

	//GUI创建
	QWidget* create_GUIAvgBlur();		//均值滤波
	QWidget* create_GUIGaussianBlur();	//高斯滤波
	QWidget* create_GUIMedianBlur();	//中值滤波
	QWidget* create_GUIBilateralBlur(); //双边滤波
	QWidget* create_GUIThreshoild();	//阈值化
	QWidget* create_GUIMorphology();	//形态学
	QWidget* create_GUIConnected();		//连通块分析
public slots:
	void onClicked_buttonGroup_blur(QAbstractButton* btn);
	void onClicked_buttonGroup_threshold(QAbstractButton* btn);
	void onClicked_buttonGroup_morphology(QAbstractButton* btn);
	void onClicked_buttonGroup_connected(QAbstractButton* btn);

	void onTriggered_action_openFile();
	void onTriggered_action_saveFile();
	void onTriggered_action_allRestore();
	void onTriggered_action_previewToNormal();

	//非加工模式下：切换不同的操作时，清除原来的操作
	void restore_cutOperation(Object* operation);
	
	void setPixmap_differentOpera(Object* operation);

	//开启图片加工模式
	void onTriggered_action_process();

	//撤销此次操作
	void onTriggered_action_undo();

private: //辅助函数
	//清除某一页参数的数值
	void setIndexPageWidgetValue(int index = -1);
	void clearAllWidgetValue();

private:
	cv::Mat ori_mt; //保存原始的加载的图片
	cv::Mat temp_mt;//如果是创作者模式，则需要保存每次更新后的图片
	QImage ori_img;
	QLabel* lab_img = nullptr;
	QLabel* sub_lab_img = nullptr; //预览图片

	QAction* action_exit = nullptr;
	QAction* action_open = nullptr;
	QAction* action_save = nullptr;
	QAction* action_restore = nullptr;
	QAction* action_begin = nullptr;
	QAction* action_return = nullptr;
	QAction* action_preview = nullptr;

	QMenu* context_menu = nullptr;
	QMenu* menu_file = nullptr;
	QMenu* menu_edit = nullptr;

	QToolBar* toolbar1 = nullptr;

	QToolBox* toolbox_side = nullptr;
	int preToolBoxIndex = 0, curToolBoxIndex = 0;

	int now = -1; //全局定位点，定位在哪个功能位置
	bool mode = false;

	QButtonGroup* btngroup_blur = nullptr;
	QButtonGroup* btngroup_threshold = nullptr;
	QButtonGroup* btngroup_form = nullptr;
	QButtonGroup* btngroup_connected = nullptr;

	QFileDialog* fileDialog = nullptr;

	//窗口布局
	QVBoxLayout* vlayout_right = nullptr;
	QStackedWidget* stack_tools = nullptr;

	//---------------功能实现-----------------
	Blur* blur = nullptr; //模糊
	Threshold* threshold = nullptr;
	Morphology* morphology = nullptr;
	Connected* connected = nullptr;
	QList<Object*> ls;
};


#endif
