#ifndef WIDGET_H_
#define WIDGET_H_
#define FUNCTION_
#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <stack>
class QAbstractButton;
class QButtonGroup;
class QGridLayout;
class QToolButton;
class QToolBox;
class Main_Label;	//主图片
class Sub_Label;	//预览图片
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

class Widget :public QMainWindow {
	Q_OBJECT
private:
	//单例设计模式
	Widget(QMainWindow* parent = nullptr);
public:	
	//返回实例
	static Widget* getInstance();
	~Widget();
protected:
	void mousePressEvent(QMouseEvent* ev)override;
public:
	//操作对象
	void initFunction();

	void createAction();
	void createMenu();
	void createToolBar();
	void createToolBox();
	void createStatusBar();
	
	QWidget* createToolBtnItemWidget(const QString& text, int id, const QString& fileName = "");

	//创建每个操作的输入数值方式
	template <typename Type>
	QHBoxLayout* create_Edit_hLayout(const QString& filter, const QString& text, Type* t);

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

signals:
	void modeChanged(); //模式改变 默认->加工->默认
	
public slots:
	void onClicked_buttonGroup_blur(QAbstractButton* btn);
	void onClicked_buttonGroup_threshold(QAbstractButton* btn);
	void onClicked_buttonGroup_morphology(QAbstractButton* btn);
	void onClicked_buttonGroup_connected(QAbstractButton* btn);
	void onClicked_buttonGroup_contours(QAbstractButton* btn);
	void onClicked_buttonGroup_show(QAbstractButton* btn);

	//打开新图片
	void onTriggered_action_openFile();

	//保存当前图片到文件夹中: lab_img
	void onTriggered_action_saveToFile();

	//重置所有操作至原始状态
	void onTriggered_action_allRestore();

	//确定加载预览图片到主图片
	void onTriggered_action_previewToNormal();

	//非加工模式下：切换不同的操作时，清除原来的操作
	void restore_cutOperation();

	//开启图片加工模式
	void onTriggered_action_process();

	//撤销此次操作
	void onTriggered_action_undo();

	//保存图片，设置保存点
	void savePoint();

	//读取保存点
	void returnPoint();

	//隐藏其他的QDialog
	void hideAllDialog(QDialog* currDialog);

	//依照inter_mt更新所有图片数据（不包括对图像格式的改变与旋转等基础操作造成的改变）
	void updateFromIntermediate(); 

	//依照root_mt更新所有图片数据：相当于清除所有操作
	void updateFromRoot();
private: //辅助函数
	//清除某一页参数的数值
	void setIndexPageWidgetValue(int index = -1);
	void clearAllWidgetValue();

public FUNCTION_: //功能性函数
	//选择颜色
	void onTriggered_ColorDialog_choice(const QColor& color);

public:
	cv::Mat	root_mt; //root根层：保存原始的加载的图片

	cv::Mat inter_mt; //intermediate中间层：普通模式下对此图片进行操作

	cv::Mat preview_mt; //preview预览层：加工模式下对此图片进行预操作

	cv::Mat curr_mt; //current当前层：lab_img的实际显示图片
	QImage  curr_img;

	//撤销功能
	std::stack<cv::Mat> undo_sta;

	Main_Label* lab_img = nullptr;
	Sub_Label* sub_lab_img = nullptr; //自定义预览图片类
	static Widget* widget;
	bool mode = false;

	int now_operation = -1; //记录当前操作位置，根据button的id。此变量用于在blur中选择
	int now_dialog = 0;	    //记录当前的操作对话框 默认在第一个页面
private:

	DrawWidget* widget_draw = nullptr; //一个简单的绘图板


	QAction* action_exit = nullptr;
	QAction* action_open = nullptr;
	QAction* action_save = nullptr;
	QAction* action_restore = nullptr;
	QAction* action_begin = nullptr;
	QAction* action_return = nullptr;
	QAction* action_preview = nullptr;

public:

	QAction* action_ori = nullptr;
	QAction* action_hls = nullptr;
	QAction* action_rgb = nullptr;
	QAction* action_hsv = nullptr;
	QAction* action_lab = nullptr;
	QAction* action_draw = nullptr;
	QActionGroup* action_cvtColor_group = nullptr;

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

	//模糊操作对话框

	QList<QDialog*> ls_dlg_avg;
	QDialog* dlg_threshold = nullptr;
	QDialog* dlg_morphology = nullptr;
	QDialog* dlg_connected = nullptr;
	QDialog* dlg_contours = nullptr;
	QDialog* dlg_showeffect = nullptr;
	//汇总
	QList<QDialog*> all_dlg;

	//-----------------------------
	QList<QButtonGroup*> btngroups;

	QFileDialog* fileDialog = nullptr;
	QColorDialog* colorDialog = nullptr;

	//窗口布局
	QVBoxLayout* vlayout_right = nullptr;

	//---------------功能实现-----------------
	Blur* blur = nullptr; //模糊
	Threshold* threshold = nullptr;
	Morphology* morphology = nullptr;
	Connected* connected = nullptr;
	Contours* contours = nullptr;
	Showeffect* showeffect = nullptr;
	BaseOperate* img_base = nullptr; //图像基础操作
	QList<Object*> ls;
};


#endif
