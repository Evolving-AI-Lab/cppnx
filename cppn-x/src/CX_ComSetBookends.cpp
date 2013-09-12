/*
 * CX_ComSetBookends.cpp
 *
 *  Created on: Jul 12, 2013
 *      Author: joost
 */

#include "CX_ComSetBookends.h"
#include "CE_Edge.h"

ComSetBookends::ComSetBookends(Edge* edge, double bookendStart, double bookendEnd, double bookendStep):edge(edge),bookendStart(bookendStart),bookendEnd(bookendEnd), bookendStep(bookendStep) {
	previousBookendStart = edge->getBookendStart();
	previousBookendEnd = edge->getBookendEnd();
	previousBookendStep = edge->getStepsize();
	setText("set bookends");
}

void ComSetBookends::undo(){
	edge->setBookends(previousBookendStart, previousBookendEnd, previousBookendStep);
}

void ComSetBookends::redo(){
	edge->setBookends(bookendStart, bookendEnd, bookendStep);
}

bool ComSetBookends::mergeWith(const QUndoCommand *other){
    if (other->id() != id()) return false;
    const ComSetBookends* otherSetWeight = static_cast<const ComSetBookends*>(other);
    if(edge != otherSetWeight->edge) return false;
    bookendStart = otherSetWeight->bookendStart;
    bookendEnd = otherSetWeight->bookendEnd;
    return true;
}

//ComSetBookends::~ComSetBookends() {
//	// TODO Auto-generated destructor stub
//}

