#include "../include/LabelImg.h"
#include "../include/widget.h"

void Label::enterEvent(QEnterEvent* ev)
{
	if (img) {
		delete img;
		img = nullptr;
	}
	img = new QPixmap(Widget::getInstance()->lab_img->pixmap());
	Widget::getInstance()->lab_img->setPixmap(Widget::getInstance()->sub_lab_img->pixmap());
}

void Label::leaveEvent(QEvent* ev)
{
	if (img) {
		Widget::getInstance()->lab_img->setPixmap(*img);
	}
}

void Label::mousePressEvent(QMouseEvent* ev)
{
	Widget::getInstance()->onTriggered_action_previewToNormal();
}