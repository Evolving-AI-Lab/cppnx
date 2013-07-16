/*
 * CE_CommandAddLabel.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#include "CE_CommandAddLabel.h"
#include "CE_LabelWidget.h"

CommandAddLabel::CommandAddLabel(LabelWidget* labelWidget, Label* label, bool add): labelWidget(labelWidget), add(add) {
	labels.append(label);
	label->registerObject();
	if(add){
		setText("add label");
	}else{
		setText("remove label");
	}
}

CommandAddLabel::CommandAddLabel(LabelWidget* labelWidget, QList<QGraphicsItem*> items, bool add): labelWidget(labelWidget), add(add) {
	foreach(QGraphicsItem* item, items){
		Label* label =  qgraphicsitem_cast<Label*>(item);
		if(label){
			label->registerObject();
			labels.append(label);
		}
	}

	if(add){
		setText("add label");
	}else{
		setText("remove label");
	}
}

CommandAddLabel::~CommandAddLabel() {
	foreach(Label* label, labels){
		label->unregisterObject();
	}
}

void CommandAddLabel::undo(){
//	std::cout << "undo: " << add <<std::endl;
	if(add){
		removeLabel();
	}else {
		addLabel();
	}
}

void CommandAddLabel::redo(){
//	std::cout << "redo: " << add <<std::endl;
	if(add){
		addLabel();
	}else {
		removeLabel();
	}
}

void CommandAddLabel::addLabel(){
	foreach(Label* label, labels){
		labelWidget->addLabel(label);
	}
}
void CommandAddLabel::removeLabel(){
	foreach(Label* label, labels){
		labelWidget->removeLabel(label);
	}
}
