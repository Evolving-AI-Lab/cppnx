/*
 * CE_CommandLabelObject.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#include "CX_ComLabelObject.h"
#include "CE_Edge.h"
#include "CE_Node.h"
#include "CX_Debug.hpp"

CommandLabelObject::CommandLabelObject(QList<QGraphicsItem*> objects, Label* label){
    init(objects, label);
}

CommandLabelObject::~CommandLabelObject() {
    if(_label){
        _label->unregisterObject();
    }
	foreach(objectLabelPair_t pair, objectLabelPairs){
	    if(pair.second){
	        pair.second->unregisterObject();
	    }
	}
}

void CommandLabelObject::init(const QList<QGraphicsItem*>& objects, Label* label){
    dbg::out(dbg::info, "command") << "Labeling " << objects.size() << " objects with label: " << label << std::endl;
    _label = label;
    if(_label) _label->registerObject();
    foreach(QGraphicsItem* item, objects){
        LabelableObject* object = util::multiCast<LabelableObject*, Edge*, Node*>(item);
        if(object){
            if(object->hasLabel()){
                object->getLabel()->registerObject();
            }
            objectLabelPairs.push_back(std::pair<LabelableObject*, Label*>(object, object->getLabel()) );
        }
    }

    setText("label object");
}

void CommandLabelObject::init(QGraphicsItem* object, Label* label){
    QList<QGraphicsItem*> objects;
    objects.append(object);
    init(objects, label);
}

void CommandLabelObject::undo(){
	foreach(objectLabelPair_t pair, objectLabelPairs){
	    dbg::out(dbg::info, "command") <<
	            "Undo. Applying label: " << (pair.second ? pair.second->getText().toStdString() : "0") <<
	            " to: " << pair.first << std::endl;
		pair.first->setLabel(pair.second);
		pair.first->update();
	}
}

void CommandLabelObject::redo(){
	foreach(objectLabelPair_t pair, objectLabelPairs){
	    dbg::out(dbg::info, "command") <<
	            "Redo. Applying label: " << (_label ? _label->getText().toStdString() : "0") <<
	            " to: " << pair.first << std::endl;
		pair.first->setLabel(_label);
		pair.first->update();
	}
}



