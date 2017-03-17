/*
 * CE_CommandAddLabel.h
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDADDLABEL_H_
#define CE_COMMANDADDLABEL_H_

#include <QGraphicsItem>
#include <QList>

#include "CX_ComBase.h"
#include "CX_Debug.hpp"

class LabelWidget;
class Label;

class CommandAddLabel: public ComBase {
public:
    CommandAddLabel(QUndoCommand* parent = 0): ComBase(parent), _labelWidget(0), _add(true){}
	CommandAddLabel(LabelWidget* labelWidget, Label* label, bool add = true);
	CommandAddLabel(LabelWidget* labelWidget, QList<QGraphicsItem*> labels, bool add = true);
	virtual ~CommandAddLabel();

	void init(LabelWidget* labelWidget, Label* label, bool add = true);
	void init(LabelWidget* labelWidget, QList<QGraphicsItem*> items, bool add = true);
	void init(LabelWidget* labelWidget, QList<Label*> labels, bool add = true);

	void undo();
	void redo();
private:
	void addLabel();
	void removeLabel();

	LabelWidget* _labelWidget;
	QList<Label*> _labels;
	bool _add;
};



#endif /* CE_COMMANDADDLABEL_H_ */
