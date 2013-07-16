/*
 * CX_ContextMenuGraphicsView.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: joost
 */

#include "CX_ContextMenuGraphicsView.h"

void ContextMenuGraphicsView::ContextMenuEvent(SelectableObject* object, bool begin){
//	std::cout << object << " " << " begin: " << begin << " selected: " << object->isSelected() << std::endl;

	if(object->isSelected()){
		foreach(QGraphicsItem* item, scene()->selectedItems()){
			SelectableObject* currentObject = dynamic_cast <SelectableObject*>(item);

//			std::cout << currentObject << " type: " << item->type() - QGraphicsItem::UserType << std::endl;
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
