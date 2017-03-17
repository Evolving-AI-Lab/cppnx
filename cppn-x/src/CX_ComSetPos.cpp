/*
 * CE_CommandSetPos.cpp
 *
 *  Created on: Jun 29, 2013
 *      Author: Joost Huizinga
 */

//Local includes
#include "CX_ComSetPos.h"

CommandSetPos::CommandSetPos(QList<QGraphicsItem*> items) {
    dbg::trace trace("commandsetpos", DBG_HERE);
    dbg::out(dbg::info, "commandsetpos") << "Items passed: " << items.size() << std::endl;
    QList<MovableObject*> nodes;
	foreach(QGraphicsItem* item, items){
	    if(MovableObject::movableTypes.contains(item->type())){
	        MovableObject* node = (MovableObject*) item;
	        if(node){
	            nodes.append(node);
	        }
	    }

	}
	init(nodes);
}

CommandSetPos::CommandSetPos(QList<Node*> nodes) {
    dbg::trace trace("commandsetpos", DBG_HERE);
    dbg::out(dbg::info, "commandsetpos") << "Nodes passed: " << nodes.size() << std::endl;
    QList<MovableObject*> objects;
    foreach(MovableObject* object, nodes){
        dbg::out(dbg::info, "commandsetpos") << "Attempting to append object: " << object << std::endl;
        if(object){
            objects.append(object);
        }
    }
    init(objects);
}

CommandSetPos::CommandSetPos(QList<MovableObject*> objects){
    dbg::trace trace("commandsetpos", DBG_HERE);
    dbg::out(dbg::info, "commandsetpos") << "Objects passed: " << objects.size() << std::endl;
    init(objects);
}

CommandSetPos::~CommandSetPos() {
    dbg::trace trace("commandsetpos", DBG_HERE);
}

void CommandSetPos::init(QList<MovableObject*>& nodes){
    dbg::trace trace("commandsetpos", DBG_HERE);
    foreach(MovableObject* node, nodes){
        if(node){
            nodePosTriples.append(triple_t(node, node->getPrevPos(), node->pos()));
        }
    }
    dbg::out(dbg::info, "commandsetpos") << "Nodes moved: " << nodePosTriples.size() << std::endl;

    if(nodes.count() > 1){
        setText("move nodes");
    } else{
        setText("move node");
    }
}

void CommandSetPos::undo(){
    dbg::trace trace("commandsetpos", DBG_HERE);
	foreach(triple_t triple, nodePosTriples){
		triple.first->setPos(triple.second);
		triple.first->updatePosition();
	}
}

void CommandSetPos::redo(){
    dbg::trace trace("commandsetpos", DBG_HERE);
	foreach(triple_t triple, nodePosTriples){
		triple.first->setPos(triple.third);
		triple.first->updatePosition();
	}
}

bool CommandSetPos::mergeWith(const QUndoCommand *other){
    dbg::trace trace("commandsetpos", DBG_HERE);
    if (other->id() != id()) return false;
    const CommandSetPos* otherSetWeight = static_cast<const CommandSetPos*>(other);
    if(nodePosTriples.count() != otherSetWeight->nodePosTriples.count()) return false;

    QList<triple_t> newTriples;
    newTriples.reserve(nodePosTriples.count());
    for(int i=0; i<nodePosTriples.count(); i++){
    	if(nodePosTriples[i].first != otherSetWeight->nodePosTriples[i].first) return false;
    	QPointF thisPreviousPos = nodePosTriples[i].second;
    	QPointF thisCurrentPos = nodePosTriples[i].third;
    	QPointF otherPreviousPos = otherSetWeight->nodePosTriples[i].second;
    	QPointF otherCurrentPos = otherSetWeight->nodePosTriples[i].third;

    	if(thisPreviousPos == otherCurrentPos){
    		newTriples.append(triple_t(nodePosTriples[i].first, otherPreviousPos, thisCurrentPos));
    	} else if(thisCurrentPos == otherPreviousPos){
    		newTriples.append(triple_t(nodePosTriples[i].first, thisPreviousPos, otherCurrentPos));
    	} else{
    		return false;
    	}
    }

    nodePosTriples = newTriples;
    return true;
}
