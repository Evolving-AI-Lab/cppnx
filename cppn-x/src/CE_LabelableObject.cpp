/*
 * CE_LabelableObject.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#include "CE_LabelableObject.h"
#include "iostream"
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
//

LabelableObject::LabelableObject(Label* _label, QString note): note(note){
	if(_label == 0){
		label = new Label();
		label->registerObject();
	} else {
		label = _label;
		label->registerObject();
	}
}

LabelableObject::LabelableObject(std::iostream &stream, std::map<std::string, Label*> labelMap){
	std::string labelId;
	std::string noteStr;

	stream >> labelId;
//	stream >> noteStr;
	noteStr = util::readString(stream);

//	std::cout << "Label: " << labelId <<std::endl;
//	std::cout << "Note: " << noteStr <<std::endl;

	note = QString(noteStr.c_str());
	label = labelMap[labelId];
//	std::cout << "Label-pt: " << label << std::endl;
	if(!label) label = new Label();
	label->registerObject();
}

//
LabelableObject::~LabelableObject() {
	label->unregisterObject();
}

Label* LabelableObject::getLabel(){
	return label;
}

void LabelableObject::setLabel(Label* _label){
	label->unregisterObject();
	label->setHighlightOff();
	label = _label;
	label->registerObject();
	label->setHighlightOn();
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

    return SelectableObject::itemChange(change, value);
}
