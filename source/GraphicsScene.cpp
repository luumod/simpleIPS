#include "other_functions/GraphicsScene.h"
#include "assist/Enums.h"
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsRectItem>

GraphicsScene::GraphicsScene(QObject* parent) :QGraphicsScene(parent) {}

GraphicsScene::~GraphicsScene()
{
	for (auto& x : lsItems) {
		delete x;
		x = nullptr;
	}
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* ev) {
	init();
	ok = true;
	if (type == SHAPE::Line) {
		start = ev->scenePos();
		addItem(line);
	}
	else if (type == SHAPE::Rect) {
		start = ev->scenePos();
		addItem(rectItem);
	}
	else if (type == SHAPE::Eillipse) {
		start = ev->scenePos();
		addItem(eill);
	}
	else if (type == SHAPE::Path) {
		path.moveTo(ev->scenePos());
		addItem(pathItem);
	}
}
void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* ev) {
	if (ok) {
		if (type == SHAPE::Path) {
			path.lineTo(ev->scenePos());
		}
		else {
			end = ev->scenePos();
		}
		draw(SHAPE(type));
	}
}
void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* ev) {
	if (type == SHAPE::Path) {
		path.lineTo(ev->scenePos());
	}
	else {
		end = ev->scenePos();
	}
	draw(SHAPE(type));
	ok = false;
}

void GraphicsScene::init() {
	if (type == SHAPE::Line) {
		lsItems.push_back(line = new QGraphicsLineItem);
		line->setPen(QPen(Qt::red, 2));
	}
	else if (type == SHAPE::Rect) {
		lsItems.push_back(rectItem = new QGraphicsRectItem);
		rectItem->setPen(QPen(Qt::yellow, 2));
	}
	else if (type == SHAPE::Eillipse) {
		lsItems.push_back(eill = new QGraphicsEllipseItem);
		eill->setPen(QPen(Qt::blue, 2));
	}
	else if (type == SHAPE::Path) {
		lsItems.push_back(pathItem = new QGraphicsPathItem);
		pathItem->setPen(QPen(Qt::green, 2));
	}

}
void GraphicsScene::draw(SHAPE type) {
	if (type == SHAPE::Line) {
		choice(start, end);
	}
	else if (type == SHAPE::Rect) {
		choice(start, end);
	}
	else if (type == SHAPE::Eillipse) {
		choice(start, end);
	}
	else if (type == SHAPE::Path) {
		choice(path);
	}
}
void GraphicsScene::choice(QPointF start, QPointF end) {
	if (type == SHAPE::Line) {
		line->setLine(QLineF(start, end));
	}
	else if (type == SHAPE::Eillipse) {
		eill->setRect(QRectF(start, end));
	}
	else if (type == SHAPE::Rect) {
		rectItem->setRect(QRectF(start, end).normalized());
	}
}

void GraphicsScene::choice(QPainterPath path) {
	if (type == SHAPE::Path) {
		pathItem->setPath(path);
	}
}
