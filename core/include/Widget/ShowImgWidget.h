#ifndef SHOWIMGWIDGET_H_
#define SHOWIMGWIDGET_H_

#include <QMainWindow>
#include <opencv2/opencv.hpp>
class QAction;
class QMenu;
class QLabel;

/*
只显示一个图片的独立Widget
*/

class ShowImgWidget :public QMainWindow {
	Q_OBJECT
public:
	ShowImgWidget(QWidget* parent = nullptr);
	~ShowImgWidget();

	void createMenuBar();
	void createAction();
	
signals:
	void signal_file_reload();
public:
	QLabel* lab_img = nullptr;

private:
	QAction* action_save = nullptr;
	QAction* action_reload = nullptr;
	QMenu* menu_file = nullptr;
};

#endif // !SHOWIMGWIDGET_H_


