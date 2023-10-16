#ifndef CUSTOMWIDGET_H_
#define CUSTOMWIDGET_H_

#include <QWidget>
#include <QFrame>
#include <QMouseEvent>
#include <QLabel>
class QToolBox;
class QLabel;


class Temp_Label : public QLabel
{
	Q_OBJECT

public:
	Temp_Label(const QString& text, QWidget* parent = nullptr)
		: QLabel(text, parent)
	{
		setMouseTracking(true); // 启用鼠标跟踪，这样可以捕获鼠标左键点击事件
	}
signals:
	void LeftButtonClicked();
protected:
	void mousePressEvent(QMouseEvent* event) override
	{
		if (event->button() == Qt::LeftButton)
		{
			emit LeftButtonClicked();
		}
	}
};


//专为图片设计的 QLabel
class LabelBuilder:public QLabel {
public:
	LabelBuilder();
	~LabelBuilder();

	LabelBuilder& setMouseTracking(bool open);
	LabelBuilder& setObjectName(const QString& objName);
	LabelBuilder& setContextMenuPolicy(Qt::ContextMenuPolicy policy);
	LabelBuilder& setAlignment(Qt::Alignment align);
	LabelBuilder& setPixmap(const QPixmap& pixmap);

	QLabel* create(QWidget* parent = nullptr);
	void ClickableLabel(const QString& text, QWidget* parent = nullptr);


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
