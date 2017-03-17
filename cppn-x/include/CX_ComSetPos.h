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
#include "CX_MovableObject.hpp"
#include "CE_Defines.h"
#include "CX_ComBase.h"
#include "CX_Debug.hpp"

class CommandSetPos: public ComBase {
public:
	CommandSetPos(QList<QGraphicsItem*> items);
    CommandSetPos(QList<Node*> items);
    CommandSetPos(QList<MovableObject*> items);
	virtual ~CommandSetPos();

	void init(QList<MovableObject*>& items);
	void undo();
	void redo();

	int id() const{return SET_POS_ID;}
	bool mergeWith(const QUndoCommand *other);

private:
	typedef util::Triple<MovableObject*, QPointF, QPointF> triple_t;
	QList<triple_t> nodePosTriples;
};

#endif /* CE_COMMANDSETPOS_H_ */
