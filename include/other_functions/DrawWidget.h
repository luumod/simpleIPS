#ifndef DRAWWIDGET_H_
#define DRAWWIDGET_H_

#include <QMainWindow>
#include "../opencv_functions/Object.h"
class GraphicsScene;
class QLabel;
class QButtonGroup;
class QAbstractButton;
class QMenu;
class QAction;
class QScrollArea;

class DrawWidget :public QMainWindow,public Object {
	Q_OBJECT
public:
	DrawWidget(QWidget* parent = nullptr);
	~DrawWidget();
	QWidget* createToolBtnItemWidget(const QString& text, int id, const QString& fileName);
	void initialize()override;

	void createAction();
	void createMenuBar();
	QWidget* createChoice();
	QWidget* createTools();
public slots:
	void onClicked_choiceShape(QAbstractButton* btn);
private:
	GraphicsScene* scene = nullptr;
	QButtonGroup* group = nullptr;
	
	QMenu* menu_file = nullptr;
	QAction* action_save = nullptr;
};

#endif