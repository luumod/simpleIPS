#include "../include/other_functions/LabelImg.h"
#include "../include/widget.h"

Label::~Label() {
	if (curr_img) {
		delete curr_img;
		curr_img = nullptr;
	}
}

void Label::enterEvent(QEnterEvent* ev)
{
	if (curr_img) {
		delete curr_img;
		curr_img = nullptr;
	}
	curr_img = new QPixmap(Widget::getInstance()->lab_img->pixmap());
	Widget::getInstance()->lab_img->setPixmap(Widget::getInstance()->sub_lab_img->pixmap());
}

void Label::leaveEvent(QEvent* ev)
{
	if (curr_img) {
		Widget::getInstance()->lab_img->setPixmap(*curr_img);
	}
}

void Label::mousePressEvent(QMouseEvent* ev)
{
	Widget::getInstance()->onTriggered_action_previewToNormal();
}