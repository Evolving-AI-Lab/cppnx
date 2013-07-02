/*
 * CE_CommandChangeLabelName.h
 *
 *  Created on: Jun 30, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDCHANGELABELNAME_H_
#define CE_COMMANDCHANGELABELNAME_H_

#include <qundostack.h>

class LabelWidget;

class CommandChangeLabelName: public QUndoCommand {
public:
	CommandChangeLabelName(LabelWidget* labelWidget);
	virtual ~CommandChangeLabelName();

	void undo();
	void redo();

	bool isOk(){
		return ok;
	}

private:
	LabelWidget* labelWidget;
	bool ok;
	QString newName;
	QString oldName;
};

#endif /* CE_COMMANDCHANGELABELNAME_H_ */
