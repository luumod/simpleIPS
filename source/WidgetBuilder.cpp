#include "Widget/WidgetBuilder.h"
#include <QToolBox>
#include <QString>
#include <QLabel>

/*
QLabel
*/

LabelBuilder::LabelBuilder()
{
	lab = new QLabel;
	lab->setMouseTracking(false);
}

LabelBuilder::~LabelBuilder()
{

}

LabelBuilder& LabelBuilder::setMouseTracking(bool open)
{
	lab->setMouseTracking(open);
	return *this;
}

LabelBuilder& LabelBuilder::setObjectName(const QString& objName)
{
	lab->setObjectName(objName);
	return *this;
}

LabelBuilder& LabelBuilder::setContextMenuPolicy(Qt::ContextMenuPolicy policy)
{
	lab->setContextMenuPolicy(policy);
	return *this;
}

LabelBuilder& LabelBuilder::setAlignment(Qt::Alignment align)
{
	lab->setAlignment(align);
	return *this;
}

LabelBuilder& LabelBuilder::setPixmap(const QPixmap& pixmap)
{
	lab->setPixmap(pixmap);
	return *this;
}

QLabel* LabelBuilder::create(QWidget* parent)
{
	lab->setParent(parent);
	return lab;
}

//------------------------------------


ToolBoxBuilder::ToolBoxBuilder()
{
	tool_box = new QToolBox;
}

ToolBoxBuilder::~ToolBoxBuilder()
{
}

ToolBoxBuilder& ToolBoxBuilder::setObjectName(const QString& objName)
{
	tool_box->setObjectName(objName);
	return *this;
}

ToolBoxBuilder& ToolBoxBuilder::setMinimumWidth(int val)
{
	tool_box->setMinimumWidth(val);
	return *this;
}

ToolBoxBuilder& ToolBoxBuilder::setMaximumWidth(int val)
{
	tool_box->setMaximumWidth(val);
	return *this;
}

ToolBoxBuilder& ToolBoxBuilder::setFrameShape(QFrame::Shape shape)
{
	tool_box->setFrameShape(shape);
	return *this;
}

ToolBoxBuilder& ToolBoxBuilder::setSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical)
{
	tool_box->setSizePolicy(horizontal, vertical);
	return *this;
}

ToolBoxBuilder& ToolBoxBuilder::addItem(QWidget* widget,const QString& name)
{
	tool_box->addItem(widget,name);
	return *this;
}

ToolBoxBuilder& ToolBoxBuilder::setCurrentIndex(int index)
{
	// TODO: 在此处插入 return 语句
	tool_box->setCurrentIndex(index);
	return *this;
}

QToolBox* ToolBoxBuilder::create(QWidget* parent)
{
	tool_box->setParent(parent);
	return tool_box;
}
