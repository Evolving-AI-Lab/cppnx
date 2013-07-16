/*
 * CX_ComSetBookends.h
 *
 *  Created on: Jul 12, 2013
 *      Author: joost
 */

#ifndef CX_COMSETBOOKENDS_H_
#define CX_COMSETBOOKENDS_H_

#include <QUndoCommand>
#include "CE_Defines.h"

class Edge;

class ComSetBookends: public QUndoCommand {
public:
	ComSetBookends(Edge* edge, double bookendStart, double bookendEnd);
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
	double previousBookendStart;
	double previousBookendEnd;
};

#endif /* CX_COMSETBOOKENDS_H_ */
