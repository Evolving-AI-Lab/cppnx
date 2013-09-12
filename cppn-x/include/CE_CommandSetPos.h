/*
 * CE_CommandSetPos.h
 *
 *  Created on: Jun 29, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDSETPOS_H_
#define CE_COMMANDSETPOS_H_

#include <QGraphicsItem>

#include "CE_Util.h"
#include "CE_Node.h"
#include "CE_Defines.h"
#include "CX_ComBase.h"

class CommandSetPos: public ComBase {
public:
	CommandSetPos(QList<QGraphicsItem*> items);
	virtual ~CommandSetPos();

	void undo();
	void redo();

	int id() const{
		return SET_POS_ID;
	}
	bool mergeWith(const QUndoCommand *other);

private:
	typedef util::Triple<Node*, QPointF, QPointF> triple_t;
	QList<triple_t> nodePosTriples;
};

#endif /* CE_COMMANDSETPOS_H_ */
