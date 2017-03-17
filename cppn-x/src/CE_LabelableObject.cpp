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

LabelableObject::LabelableObject(Label* _label, QString note): label(0), note(note){
    makeLabel(_label);
//
//	label = _label;
	init();
}

LabelableObject::LabelableObject(std::iostream &stream, std::map<std::string, Label*> labelMap): label(0){
	std::string labelId;
	std::string noteStr;

	stream >> labelId;
//	stream >> noteStr;
	noteStr = util::readString(stream);

//	std::cout << "Label: " << labelId <<std::endl;
//	std::cout << "In label map: " << labelMap.count(labelId) <<std::endl;

	note = QString(noteStr.c_str());
	makeLabel(labelMap[labelId]);

//	std::cout << "Label-pt: " << label << std::endl;
	init();
}

//
LabelableObject::~LabelableObject() {
    if(label){
        disconnect(label, SIGNAL(onSelected()), this, SLOT(onLabelSelected()));
        label->setHighlightOff();
        label->unregisterObject();
    }
}

void LabelableObject::init(){
//
//	if(!label) label = new Label();
//	label->registerObject();
//	connect(label, SIGNAL(onSelected()), this, SLOT(onLabelSelected()));
}

Label* LabelableObject::getLabel(){
//    if(!label) makeLabel(new Label());
	return label;
}

void LabelableObject::setLabel(Label* _label){
    //Unregister from the old label, if we have one
    if(label){
        disconnect(label, SIGNAL(onSelected()), this, SLOT(onLabelSelected()));
	    label->setHighlightOff();
	    label->unregisterObject();
    }

    //Set the current label to the supplied label
	label = _label;

	//Register the new label, if we have one
	if(label){
	    label->registerObject();
	    if(this->isSelected()) label->setHighlightOn();
	    connect(label, SIGNAL(onSelected()), this, SLOT(onLabelSelected()));
	}
}

bool LabelableObject::hasLabel(){
    if(label){
        return !label->isDeleted();
    } else {
        return false;
    }
}

id_t LabelableObject::getLabelId(){
    if(label){
        return label->getId();
    } else {
        return 0;
    }
}

QVariant LabelableObject::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(label){
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
    }

    return SelectableObject::itemChange(change, value);
}

void LabelableObject::makeLabel(Label* _label){
    if(_label){
        label = _label;
        label->registerObject();
        connect(label, SIGNAL(onSelected()), this, SLOT(onLabelSelected()));
    }
}
