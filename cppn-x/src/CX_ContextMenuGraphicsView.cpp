/*
 * CX_ContextMenuGraphicsView.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: joost
 */

#include "CX_ContextMenuGraphicsView.h"

void ContextMenuGraphicsView::ContextMenuEvent(SelectableObject* object, bool begin){
	partOfContextMenuEvent = begin;
	if(object->isSelected()){
		foreach(QGraphicsItem* item, scene()->selectedItems()){
			SelectableObject* currentObject = dynamic_cast <SelectableObject*>(item);
			currentObject->setPartOfContextMenuEvent(begin);
			currentObject->update();
		}
	} else {
		if(begin){
			foreach(QGraphicsItem* item, scene()->selectedItems()){
				item->setSelected(false);
			}
			object->setSelected(true);
		}

		object->setPartOfContextMenuEvent(begin);
		object->update();
	}
}

void ContextMenuGraphicsView::updateAll(){
	foreach(QGraphicsItem* item, scene()->items()){
		item->update();
	}
}

QAction* ContextMenuGraphicsView::createAction(QString name, QString statusBarTip, const char *member){
    QAction* action = new QAction(name, this);
    action->setStatusTip(statusBarTip);
    connect(action, SIGNAL(triggered()), this, member);
    addAction(action);
    return action;
}
