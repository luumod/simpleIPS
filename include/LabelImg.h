#ifndef LABELIMG_H_
#define LABELIMG_H_

#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QPixmap>

/*
实现对于预览图片的操作
*/

class Label :public QLabel {
	Q_OBJECT
public:
	using QLabel::QLabel;
	~Label();
protected:
	void enterEvent(QEnterEvent* ev)override;
	void leaveEvent(QEvent* ev)override;
	void mousePressEvent(QMouseEvent* ev)override;
private:
	QPixmap* img = nullptr;
};

#endif // !LABELIMG_H_
