/*
 * CE_LabelableObject.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#include "CE_LabelableObject.h"
#include "iostream"
//

LabelableObject::LabelableObject(Window* window, LabelWidget* _label, QString note): note(note){
	if(_label == 0){
		label = new LabelWidget(window);
		label->registerObject();
	} else {
		label = _label;
		label->registerObject();
	}
}

//
LabelableObject::~LabelableObject() {
	label->unregisterObject();
}

LabelWidget* LabelableObject::getLabel(){
	return label;
}

void LabelableObject::setLabel(LabelWidget* _label){
	label->unregisterObject();
	label = _label;
	label->registerObject();
}

QVariant LabelableObject::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemSelectedChange:
        if(isSelected()){
        	label->setHighlightOff();
        }
    	break;
    case QGraphicsItem::ItemSelectedHasChanged:
        if(isSelected()){
        	label->setHighlightOn();
        }
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}
