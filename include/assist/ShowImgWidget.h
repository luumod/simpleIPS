#ifndef SHOWIMGWIDGET_H_
#define SHOWIMGWIDGET_H_

#include <QMainWindow>
class QAction;
class QMenu;
class QLabel;

/*
只显示一个Label的独立窗口
*/

class ShowImgWidget :public QMainWindow {
	Q_OBJECT
public:
	ShowImgWidget(QWidget* parent = nullptr);
	~ShowImgWidget();

	void createMenuBar();
	void createAction();
	
public:
	QLabel* lab_img = nullptr;

private:
	QAction* action_save = nullptr;
	QMenu* menu_file = nullptr;
};

#endif // !SHOWIMGWIDGET_H_


