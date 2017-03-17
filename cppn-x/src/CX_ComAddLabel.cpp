/*
 * CE_CommandAddLabel.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#include <CX_ComAddLabel.h>
#include "CE_LabelWidget.h"

//CommandAddLabel::CommandAddLabel(QUndoCommand* parent = 0):
//    ComBase(parent), _labelWidget(0), _add(true)
//{
//
//}

CommandAddLabel::CommandAddLabel(LabelWidget* labelWidget, Label* label, bool add){
    init(labelWidget, label, add);
}

CommandAddLabel::CommandAddLabel(LabelWidget* labelWidget, QList<QGraphicsItem*> items, bool add){
    init(labelWidget, items, add);
}

CommandAddLabel::~CommandAddLabel() {
	foreach(Label* label, _labels){
		label->unregisterObject();
	}
}

void CommandAddLabel::init(LabelWidget* labelWidget, Label* label, bool add){
    QList<Label*> labels;
    labels.append(label);
    init(labelWidget, labels, add);
}

void CommandAddLabel::init(LabelWidget* labelWidget, QList<QGraphicsItem*> items, bool add){
    QList<Label*> labels;
    foreach(QGraphicsItem* item, items){
        Label* label =  qgraphicsitem_cast<Label*>(item);
        if(label) labels.append(label);
    }
    init(labelWidget, labels, add);
}

void CommandAddLabel::init(LabelWidget* labelWidget, QList<Label*> labels, bool add){
    _labelWidget = labelWidget;
    _add = add;
    foreach(Label* label, labels){
        if(label){
            label->registerObject();
            _labels.append(label);
        }
    }

    if(add){
        setText("add label");
    }else{
        setText("remove label");
    }
}

void CommandAddLabel::undo(){
    QListIterator<Label*> it(_labels);
    it.toBack();
	if(_add){
        while(it.hasPrevious()){
            _labelWidget->removeLabel(it.previous());
        }
	}else {
        while(it.hasPrevious()){
            _labelWidget->addLabel(it.previous());
        }
	}
}

void CommandAddLabel::redo(){
    QListIterator<Label*> it(_labels);
	if(_add){
	    while(it.hasNext()){
	        _labelWidget->addLabel(it.next());
	    }
	}else {
        while(it.hasNext()){
            _labelWidget->removeLabel(it.next());
        }
	}
}

void CommandAddLabel::addLabel(){
    foreach(Label* label, _labels){
        dbg::out(dbg::info, "command") << "Removing label: " << label->getText().toStdString() << std::endl;
        _labelWidget->addLabel(label);
    }
}

void CommandAddLabel::removeLabel(){
	foreach(Label* label, _labels){
        dbg::out(dbg::info, "command") << "Removing label: " << label->getText().toStdString() << std::endl;
		_labelWidget->removeLabel(label);
	}
}
