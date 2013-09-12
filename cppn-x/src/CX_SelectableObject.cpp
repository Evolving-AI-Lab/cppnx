/*
 * CX_SelectableObject.cpp
 *
 *  Created on: Jul 9, 2013
 *      Author: joost
 */

#include "CX_SelectableObject.h"
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

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
