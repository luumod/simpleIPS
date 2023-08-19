#include "testObject.h"
#include "testWidget.h"
#include <QDebug>

Object::Object()
{
	
}

Object::~Object()
{
}

Widget* Object::get()
{
	return Widget::getInstance();
}

