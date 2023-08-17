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
class Blur;
class QStackedWidget;
class QVBoxLayout;
class Threshold;
class Form;
class QToolBar;
class QFileDialog;
class Connected;

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

	QWidget* create_GUIAvgBlur();      //均值滤波
	QWidget* create_GUIGaussianBlur(); //高斯滤波
	QWidget* create_GUIMedianBlur(); //中值滤波
	QWidget* create_GUIBilateralBlur(); //双边滤波
	QWidget* create_GUIThreshoild();	//阈值化
	QWidget* create_GUIMorphology();	//形态学
	QWidget* create_GUIConnected();	//连通块分析
public slots:
	void onClicked_buttonGroup_blur(QAbstractButton* btn);
	void onClicked_buttonGroup_threshold(QAbstractButton* btn);
	void onClicked_buttonGroup_form(QAbstractButton* btn);
	void onClicked_buttonGroup_connected(QAbstractButton* btn);

	void onClicked_action_openFile();
	void onTriggered_action_saveFile();

	void onTriggered_action_allRestore();
	void onTriggered_action_blur_restore();
	void onTriggered_action_threshold_restore();
	void onTriggered_action_morphology_restore();
	void onTriggered_action_connected_restore();

	void onTriggered_action_process();
	void onTriggered_action_undo();
signals:
	void sg_beginCreation();
private:
	void setIndexPageSliderValue(int index = -1);
	void clearAllSliderValue();

	//int whereAmI();
private:
	cv::Mat ori_mt; //保存原始的加载的图片
	cv::Mat temp_mt;//如果是创作者模式，则需要保存每次更新后的图片
	QImage ori_img;
	QLabel* lab_img = nullptr;

	QAction* action_exit = nullptr;
	QAction* action_open = nullptr;
	QAction* action_save = nullptr;
	QAction* action_restore = nullptr;
	QAction* action_begin = nullptr;
	QAction* action_return = nullptr;

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
	Form* morphology = nullptr;
	Connected* connected = nullptr;
};


#endif
