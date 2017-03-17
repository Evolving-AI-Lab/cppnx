/*
 * CX_DragAndDropObject.cpp
 *
 *  Created on: Jul 9, 2013
 *      Author: joost
 */

#include "CX_DragAndDropObject.h"
#include <QGraphicsSceneMouseEvent>
#include <QDrag>
#include <QColor>
#include <QLineF>
#include <QApplication>
#include <QMimeData>
#include <QPainter>
#include <QBitmap>

#include <iostream>

#include <QWidget>
#include <QObject>


DragAndDropObject::DragAndDropObject() {
	setAcceptDrops(true);
	dragOver = false;
	dragImage = 0;
	alternateDragImage = 0;
	index = 0;
	compatibillityId = 0;
}

DragAndDropObject::~DragAndDropObject() {
	// TODO Auto-generated destructor stub
}

void DragAndDropObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
        return;
    }

    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);

    mime->setProperty("index", QVariant(index));
    mime->setProperty("compatId", QVariant(compatibillityId));

    QPixmap pixmap;

    if(dragImage){
    	pixmap = *dragImage;
    }else if(alternateDragImage){
    	pixmap = QPixmap::fromImage(*alternateDragImage);
    } else{
    	pixmap = QPixmap(iconSize);
    	pixmap.fill(Qt::white);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(iconTranslate);
        paint(&painter, 0, 0);
        painter.end();
	}

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(15, 20));
    drag->exec();

    setCursor(Qt::OpenHandCursor);
}

void DragAndDropObject::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
//	std::cout << "Source type: " << event->mimeData()->property("compatId").toInt() << " Target type: " << compatibillityId <<std::endl;
    if (event->mimeData()->property("compatId").toUInt() == compatibillityId) {
        event->setAccepted(true);
        dragOver = true;
        update();
    } else {
        event->setAccepted(false);
    }
}

void DragAndDropObject::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    dragOver = false;
    update();
}
//! [2]

//! [3]
void DragAndDropObject::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	emit requestMove(event->mimeData()->property("index").toUInt(), index);

//	std::cout << " Accept " <<std::endl;
    dragOver = false;

    update();
}
