﻿#ifndef TESTOBJECT_H_
#define TESTOBJECT_H_

#include <QGraphicsScene>
#include <QPainterPath>
#include "Enums.h"

class QPointF;
class QGraphicsLineItem;
class QGraphicsEllipseItem;
class QGraphicsPathItem;
class QGraphicsRectItem;

class GraphicsScene :public QGraphicsScene {
public:
	GraphicsScene(QObject* parent = nullptr);
	~GraphicsScene();
	void mousePressEvent(QGraphicsSceneMouseEvent* ev)override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* ev)override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* ev)override;
	int type = 1;
	QGraphicsLineItem* line = nullptr;
	QGraphicsEllipseItem* eill = nullptr;
	QGraphicsPathItem* pathItem = nullptr;
	QGraphicsRectItem* rectItem = nullptr;
private:
	QList<QGraphicsItem*> lsItems;
	QPointF start, end;
	QPainterPath path;
	bool ok = false;
	

private:
	void init();
	void draw(SHAPE type);
	void choice(QPointF start, QPointF end);
	void choice(QPainterPath path);
};

#endif