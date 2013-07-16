/*
 * CE_CommandLabelObject.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#include "CE_CommandLabelObject.h"
#include "CE_Edge.h"
#include "CE_Node.h"

CommandLabelObject::CommandLabelObject(QList<QGraphicsItem*> objects, Label* label): label(label) {
	label->registerObject();
	foreach(QGraphicsItem* item, objects){
		LabelableObject* object = util::multiCast<LabelableObject*, Edge*, Node*>(item);
		if(object){
			object->getLabel()->registerObject();
			objectLabelPairs.push_back(std::pair<LabelableObject*, Label*>(object, object->getLabel()) );
		}
	}

	setText("label object");
}

CommandLabelObject::~CommandLabelObject() {
	label->unregisterObject();
	foreach(objectLabelPair_t pair, objectLabelPairs){
		pair.second->unregisterObject();
	}
}

void CommandLabelObject::undo(){
	foreach(objectLabelPair_t pair, objectLabelPairs){
		pair.first->setLabel(pair.second);
		pair.first->update();
	}
}

void CommandLabelObject::redo(){
	foreach(objectLabelPair_t pair, objectLabelPairs){
		pair.first->setLabel(label);
		pair.first->update();
	}
}



