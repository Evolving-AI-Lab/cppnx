/*
 * CX_SelectableObject.cpp
 *
 *  Created on: Jul 9, 2013
 *      Author: joost
 */

//QT includes
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

//Local includes
#include "CX_SelectableObject.h"

SelectableObject::SelectableObject(QGraphicsItem * parent):
QGraphicsWidget(parent), contextMenu(0), partOfContextMenuEvent(false), parentHasFocus(0){
    setFlag(ItemIsSelectable);
}

SelectableObject::SelectableObject():QGraphicsWidget(), contextMenu(0), partOfContextMenuEvent(false), parentHasFocus(0){
    setFlag(ItemIsSelectable);

}

SelectableObject::~SelectableObject() {
	// TODO Auto-generated destructor stub
}

void SelectableObject::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	emit contextMenuEvent(this, true);
    if(contextMenu) contextMenu->exec(event->screenPos());
    emit contextMenuEvent(this, false);
}


QVariant SelectableObject::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemSelectedHasChanged:
    	emit selectedHasChanged();
        if(!isSelected()){
        	partOfContextMenuEvent = false;
        } else {
        	emit onSelected();
        }
        break;
    default:
        break;
    };

    return QGraphicsWidget::itemChange(change, value);
}

QPen SelectableObject::getSelectedPen(const QColor& defaultColor){
    if (partOfContextMenuEvent){
        return QPen(CONTEXT_EVENT_COLOR, 2);
    }else if (this->isSelected()){
        if (*parentHasFocus){
            return QPen(SELECTED_COLOR, 2);
        }else{
            return QPen(NO_FOCUS_SELECTED_COLOR, 2);
        }
    }else{
        return QPen(defaultColor, 0);
    }
}
