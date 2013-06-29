/*
 * CE_CommandAddLabel.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#include "CE_CommandAddLabel.h"

CommandAddLabel::CommandAddLabel(Window* window, LabelWidget* label, bool add): window(window), add(add) {
	labels.append(label);
	label->registerObject();
	if(add){
		setText("add label");
	}else{
		setText("remove label");
	}
}

CommandAddLabel::~CommandAddLabel() {
	foreach(LabelWidget* label, labels){
		label->unregisterObject();
	}
}

void CommandAddLabel::undo(){
	if(add){
		removeLabel();
	}else {
		addLabel();
	}
}

void CommandAddLabel::redo(){
	if(add){
		addLabel();
	}else {
		removeLabel();
	}
}

void CommandAddLabel::addLabel(){
	foreach(LabelWidget* label, labels){
		window->addLabelWidget(label);
		label->setDeleted(false);
	}
}
void CommandAddLabel::removeLabel(){
	foreach(LabelWidget* label, labels){
		window->removeLabelWidget(label);
	}
}
