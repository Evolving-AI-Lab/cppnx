/*
 * CE_CommandLabelObject.h
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDLABELOBJECT_H_
#define CE_COMMANDLABELOBJECT_H_

#include "CX_ComBase.h"
#include "CE_LabelableObject.h"
#include "CE_Util.h"

class CommandLabelObject: public ComBase {
public:
    CommandLabelObject(QUndoCommand * parent = 0): ComBase(parent), _label(0) {}
	CommandLabelObject(QList<QGraphicsItem*> objects, Label* label);
	virtual ~CommandLabelObject();

	void init(const QList<QGraphicsItem*>& objects, Label* label = 0);
	void init(QGraphicsItem* object, Label* label = 0);

	void undo();
	void redo();

private:
	typedef std::pair<LabelableObject*, Label*> objectLabelPair_t;
	QList<objectLabelPair_t> objectLabelPairs;
	Label* _label;

};

#endif /* CE_COMMANDLABELOBJECT_H_ */
