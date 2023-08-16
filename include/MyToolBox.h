#ifndef MYTOOLBOX_H_
#define MYTOOLBOX_H_

#include <QToolBox>
#include <QDebug>
#include <QEvent>

class ToolBox :public QToolBox {
public:
	using QToolBox::QToolBox;
	int preIndex = -1, curIndex = -1;
protected:
	void mousePressEvent(QMouseEvent* event) override
	{
		preIndex = currentIndex(); // 保存切换前的索引
		QToolBox::mousePressEvent(event); // 调用基类的mousePressEvent函数

		qInfo() << "preIndex: " << preIndex;
	}
	
};


#endif 