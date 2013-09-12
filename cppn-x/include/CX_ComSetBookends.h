/*
 * CX_ComSetBookends.h
 *
 *  Created on: Jul 12, 2013
 *      Author: joost
 */

#ifndef CX_COMSETBOOKENDS_H_
#define CX_COMSETBOOKENDS_H_

#include "CX_ComBase.h"
#include "CE_Defines.h"

class Edge;

class ComSetBookends: public ComBase {
public:
	ComSetBookends(Edge* edge, double bookendStart, double bookendEnd, double bookendStep);
//	virtual ~ComSetBookends();

	void undo();
	void redo();

	int id() const{
		return SET_BOOKEND_ID;
	}

	bool mergeWith(const QUndoCommand *other);

private:
	Edge* edge;
	double bookendStart;
	double bookendEnd;
	double bookendStep;
	double previousBookendStart;
	double previousBookendEnd;
	double previousBookendStep;
};

#endif /* CX_COMSETBOOKENDS_H_ */
