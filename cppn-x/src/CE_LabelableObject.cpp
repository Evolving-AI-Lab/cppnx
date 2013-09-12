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
	label = _label;
	init();
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
	init();
}

//
LabelableObject::~LabelableObject() {
	label->unregisterObject();
}

void LabelableObject::init(){
	if(!label) label = new Label();
	label->registerObject();
	connect(label, SIGNAL(onSelected()), this, SLOT(onLabelSelected()));
}

Label* LabelableObject::getLabel(){
	return label;
}

void LabelableObject::setLabel(Label* _label){
	disconnect(label, SIGNAL(onSelected()), this, SLOT(onLabelSelected()));
	label->unregisterObject();
	label->setHighlightOff();
	label = _label;
	label->registerObject();
	label->setHighlightOn();
	connect(label, SIGNAL(onSelected()), this, SLOT(onLabelSelected()));
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
