/*
 * CE_CommandSetPos.cpp
 *
 *  Created on: Jun 29, 2013
 *      Author: joost
 */

#include "CE_CommandSetPos.h"
#include <istream>

CommandSetPos::CommandSetPos(QList<QGraphicsItem*> items) {
	qSort(items);
	foreach(QGraphicsItem* item, items){
		Node* node = qgraphicsitem_cast<Node*>(item);
		if(node){
//			std::cout << "Prev: " << node->getPrevPos().x() << " " << node->getPrevPos().y() << " ";
//			std::cout << "New: " << node->pos().x() << " " << node->pos().y() <<std::endl;
			nodePosTriples.append(triple_t(node, node->getPrevPos(), node->pos()));
		}
	}
	if(items.count() > 1){
		setText("move nodes");
	} else{
		setText("move node");
	}
}

CommandSetPos::~CommandSetPos() {
	// TODO Auto-generated destructor stub
}

void CommandSetPos::undo(){
	foreach(triple_t triple, nodePosTriples){
		triple.first->setPos(triple.second);
		triple.first->updatePosition();
	}
}

void CommandSetPos::redo(){
	foreach(triple_t triple, nodePosTriples){
		triple.first->setPos(triple.third);
		triple.first->updatePosition();
	}
}

bool CommandSetPos::mergeWith(const QUndoCommand *other){
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
