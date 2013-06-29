/*
 * CE_LabelableObject.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#include "CE_LabelableObject.h"
//

LabelableObject::LabelableObject(LabelWidget* _label, QString note): note(note){
	if(_label == 0){
		label = new LabelWidget();
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
