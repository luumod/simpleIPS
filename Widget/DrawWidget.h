#ifndef DRAWWIDGET_H_
#define DRAWWIDGET_H_

#include <QMainWindow>
#include "opencv_functions/Object.h"
class GraphicsScene;
class QLabel;
class QButtonGroup;
class QAbstractButton;
class QMenu;
class QAction;
class QScrollArea;
class QGraphicsPixmapItem;
class QGraphicsView;

/*
绘图板Widget：static
*/

class DrawWidget :public QMainWindow {
    Q_OBJECT

private:
	DrawWidget(QWidget* parent = nullptr);
    static DrawWidget* draw;
public:
    static DrawWidget* get(QWidget* parent = nullptr);
	~DrawWidget();

    void reset();
	QWidget* createToolBtnItemWidget(const QString& text, int id, const QString& fileName);

	void createAction();
	void createMenuBar();
    QWidget* createChoice();

    QPixmap pixmap;

public slots:
	void onClicked_choiceShape(QAbstractButton* btn);
private:
	GraphicsScene* scene = nullptr;
    QButtonGroup* group = nullptr;

	QMenu* menu_file = nullptr;
	QAction* action_save = nullptr;

    QGraphicsPixmapItem* pixItem = nullptr;
    QGraphicsView* view = nullptr;
};

#endif
