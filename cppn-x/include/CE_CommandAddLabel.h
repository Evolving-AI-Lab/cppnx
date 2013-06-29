/*
 * CE_CommandAddLabel.h
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDADDLABEL_H_
#define CE_COMMANDADDLABEL_H_

#include <qundostack.h>
#include "CE_Window.h"
#include "CE_LabelWidget.h"

class Window;

class CommandAddLabel: public QUndoCommand {
public:
	CommandAddLabel(Window* window, LabelWidget* label, bool add = true);
	virtual ~CommandAddLabel();

	void undo();
	void redo();
private:
	void addLabel();
	void removeLabel();

	Window* window;
	QList<LabelWidget*> labels;
	bool add;
};



#endif /* CE_COMMANDADDLABEL_H_ */
