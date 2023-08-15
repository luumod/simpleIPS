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
	QWidget* create_GUIThreshoild();		//阈值化
	QWidget* create_GUIMorphology();	//形态学
public slots:
	void onClicked_buttonGroup_blur(QAbstractButton* btn);
	void onClicked_buttonGroup_threshold(QAbstractButton* btn);
	void onClicked_buttonGroup_form(QAbstractButton* btn);
	void onClicked_action_openFile();
	void onTriggered_action_saveFile();
	void onTriggered_action_restore();
	void onTriggered_action_begin();
signals:
	void sg_beginCreation();
private:
	void clearCurrentPageSliderValue();
private:
	cv::Mat mt;
	QImage ori_img;
	QLabel* lab_img = nullptr;

	QAction* action_exit = nullptr;
	QAction* action_open = nullptr;
	QAction* action_save = nullptr;
	QAction* action_restore = nullptr;
	QAction* action_begin = nullptr;

	QMenu* context_menu = nullptr;
	QMenu* menu_file = nullptr;
	QMenu* menu_edit = nullptr;

	QToolBar* toolbar1 = nullptr;

	QToolBox* toolbox_side = nullptr;

	QButtonGroup* btngroup_blur = nullptr;
	QButtonGroup* btngroup_threshold = nullptr;
	QButtonGroup* btngroup_form = nullptr;

	//blur调整框
	//QWidget* adj_avgBlur = nullptr;
	//QWidget* adj_gasBlur = nullptr;
	//QWidget* adj_meanBlur = nullptr;
	//QWidget* adj_bilateralBlur = nullptr;
	//QWidget* adj_

	//窗口布局
	QVBoxLayout* vlayout_right = nullptr;
	QStackedWidget* stack_tools = nullptr;

	//---------------功能实现-----------------
	Blur* blur = nullptr; //模糊
	Threshold* threshold = nullptr;
	Form* morphology = nullptr;

	bool diy = false;
};


#endif
