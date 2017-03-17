/*
 * CE_CommandSetWeight.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#include <CX_ComSetWeight.h>
#include <iostream>

CommandSetWeight::CommandSetWeight(QList<QGraphicsItem*> items){
	qSort(items);
	foreach(QGraphicsItem* item, items){
		Edge* edge = qgraphicsitem_cast<Edge*>(item);
		if(edge){
			if(edge->getWeight() != edge->getOriginalWeight()){
				weightPair_t weightPair(edge->getWeight(), edge->getOriginalWeight());
				triple_t triple(edge, weightPair);
				edgeTriples.push_back(triple);
			}
		}
	}

	setText("reset weight");
	comId = RESET_WEIGHT_ID;
}


CommandSetWeight::CommandSetWeight(Edge* edge, qreal weight) {
	weightPair_t weightPair(edge->getWeight(), weight);
	triple_t triple(edge, weightPair);
	edgeTriples.push_back(triple);

	setText("set weight");
	comId = SET_WEIGHT_ID;
}

CommandSetWeight::~CommandSetWeight() {
	// TODO Auto-generated destructor stub
}

void CommandSetWeight::undo(){
	bool update = edgeTriples.count() <= 1;
	foreach(triple_t triple, edgeTriples){
		triple.first->setWeight(triple.second.first, update);
//		std::cout << "Undo: " << triple.second.first << std::endl;
	}
	if(!update) edgeTriples[0].first->updateAll();
}

void CommandSetWeight::redo(){
	bool update = edgeTriples.count() <= 1;
	foreach(triple_t triple, edgeTriples){
		triple.first->setWeight(triple.second.second, update);
//		std::cout << "Redo: " << triple.second.second << std::endl;
	}
	if(!update) edgeTriples[0].first->updateAll();
}

bool CommandSetWeight::mergeWith(const QUndoCommand *other){
    if (other->id() != id()) return false;
    const CommandSetWeight* otherSetWeight = static_cast<const CommandSetWeight*>(other);
    if(edgeTriples.count() != otherSetWeight->edgeTriples.count()) return false;

    QList<triple_t> newTriples;
    newTriples.reserve(edgeTriples.count());
    for(int i=0; i<edgeTriples.count(); i++){
    	if(edgeTriples[i].first != otherSetWeight->edgeTriples[i].first) return false;
    	qreal thisPreviousWeight = edgeTriples[i].second.first;
    	qreal thisCurrentWeight = edgeTriples[i].second.second;
    	qreal otherPreviousWeight = otherSetWeight->edgeTriples[i].second.first;
    	qreal otherCurrentWeight = otherSetWeight->edgeTriples[i].second.second;
    	weightPair_t weightPair;

    	if(otherPreviousWeight == thisCurrentWeight){
    		weightPair = weightPair_t(thisPreviousWeight, otherCurrentWeight);
    	} else if(thisPreviousWeight == otherCurrentWeight){
    		weightPair = weightPair_t(otherPreviousWeight, thisCurrentWeight);
    	} else{
    		return false;
    	}
    	newTriples.append(triple_t(edgeTriples[i].first, weightPair));
    }

    edgeTriples = newTriples;
    return true;
}
