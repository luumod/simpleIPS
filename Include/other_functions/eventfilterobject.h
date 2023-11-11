#ifndef EVENTFILTEROBJECT_H
#define EVENTFILTEROBJECT_H

#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QDebug>
#include <QMouseEvent>
#include <QPoint>

class EventFilterObject:public QObject
{
public:
    EventFilterObject(QObject* parent=nullptr);
    ~EventFilterObject();
public:
    bool eventFilter(QObject* obj,QEvent* ev)override;
private:
    QPoint pressPos;
};

#endif // EVENTFILTEROBJECT_H
