#include "other_functions/eventfilterobject.h"

EventFilterObject::EventFilterObject(QObject* parent)
    :QObject(parent)
{

}
EventFilterObject::~EventFilterObject(){
}

bool EventFilterObject::eventFilter(QObject *obj, QEvent *ev)
{
    switch (ev->type()) {
    case QEvent::Type::MouseButtonPress:
    {
        auto cev=static_cast<QMouseEvent*>(ev);
        pressPos=cev->pos();
        break;
    }
    case QEvent::Type::MouseMove:
    {
        auto pos=static_cast<QMouseEvent*>(ev);
        auto w=static_cast<QWidget*>(obj);
        if (w){
            w->move(pos->globalPosition().toPoint()-pressPos);
        }
        break;
    }
    default:
        break;
    }
    return false;
}
