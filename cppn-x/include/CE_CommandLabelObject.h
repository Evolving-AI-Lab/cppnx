/*
 * CE_CommandLabelObject.h
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDLABELOBJECT_H_
#define CE_COMMANDLABELOBJECT_H_

#include <QUndoCommand>
#include "CE_LabelableObject.h"
#include "CE_Util.h"
//#include "CX_CXUndoCommand.h"

class CommandLabelObject: public QUndoCommand {
public:
	CommandLabelObject(QList<QGraphicsItem*> objects, Label* label);
	virtual ~CommandLabelObject();

	void undo();
	void redo();

private:
	typedef std::pair<LabelableObject*, Label*> objectLabelPair_t;
	QList<objectLabelPair_t> objectLabelPairs;
	Label* label;

};

#endif /* CE_COMMANDLABELOBJECT_H_ */
