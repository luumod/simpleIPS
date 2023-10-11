#ifndef CUSTOMWIDGET_H_
#define CUSTOMWIDGET_H_

#include <QWidget>
#include <QFrame>
class QToolBox;
class QLabel;

//专为图片设计的 QLabel
class LabelBuilder {
public:
	LabelBuilder();
	~LabelBuilder();

	LabelBuilder& setObjectName(const QString& objName);
	LabelBuilder& setContextMenuPolicy(Qt::ContextMenuPolicy policy);
	LabelBuilder& setAlignment(Qt::Alignment align);
	LabelBuilder& setPixmap(const QPixmap& pixmap);

	QLabel* create(QWidget* parent = nullptr);
protected:
	/**/
private:
	QLabel* lab;
};

//侧边栏
class ToolBoxBuilder {
public:
	ToolBoxBuilder();
	~ToolBoxBuilder();

	ToolBoxBuilder& setObjectName(const QString& objName);
	ToolBoxBuilder& setMinimumWidth(int val);
	ToolBoxBuilder& setMaximumWidth(int val);
	ToolBoxBuilder& setFrameShape(QFrame::Shape);
	ToolBoxBuilder& setSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical);
	ToolBoxBuilder& addItem(QWidget* widget, const QString& name);
	ToolBoxBuilder& setCurrentIndex(int index);
	
	QToolBox* create(QWidget* parent = nullptr);
protected:
	/**/
private:
	QToolBox* tool_box;
};


#endif // !CAPTUREWIDGET_H_
