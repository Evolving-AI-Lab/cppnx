/*
 * CE_CommandAddLabel.h
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDADDLABEL_H_
#define CE_COMMANDADDLABEL_H_

#include <QUndoCommand>
#include <QGraphicsItem>
//#include "CX_CXUndoCommand.h"
//#include "CE_Window.h"


class LabelWidget;
class Label;

class CommandAddLabel: public QUndoCommand {
public:
	CommandAddLabel(LabelWidget* labelWidget, Label* label, bool add = true);
	CommandAddLabel(LabelWidget* labelWidget, QList<QGraphicsItem*> labels, bool add = true);
	virtual ~CommandAddLabel();

	void undo();
	void redo();
private:
	void addLabel();
	void removeLabel();

	LabelWidget* labelWidget;
	QList<Label*> labels;
	bool add;
};



#endif /* CE_COMMANDADDLABEL_H_ */
