/*
 * CE_CommandLabelObject.h
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDLABELOBJECT_H_
#define CE_COMMANDLABELOBJECT_H_

#include <qundostack.h>
#include "CE_LabelableObject.h"
#include "CE_Util.h"

class CommandLabelObject: public QUndoCommand {
public:
	CommandLabelObject(QList<QGraphicsItem*> objects, LabelWidget* label);
	virtual ~CommandLabelObject();

	void undo();
	void redo();

private:
	typedef std::pair<LabelableObject*, LabelWidget*> objectLabelPair_t;
	QList<objectLabelPair_t> objectLabelPairs;
	LabelWidget* label;

};

#endif /* CE_COMMANDLABELOBJECT_H_ */
