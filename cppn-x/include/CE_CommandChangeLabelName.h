/*
 * CE_CommandChangeLabelName.h
 *
 *  Created on: Jun 30, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDCHANGELABELNAME_H_
#define CE_COMMANDCHANGELABELNAME_H_

#include <QUndoCommand>
//#include "CX_CXUndoCommand.h"

class Label;

class CommandChangeLabelName: public QUndoCommand {
public:
	CommandChangeLabelName(Label* labelWidget);
	virtual ~CommandChangeLabelName();

	void undo();
	void redo();

	bool isOk(){
		return ok;
	}

private:
	Label* labelWidget;
	bool ok;
	QString newName;
	QString oldName;
};

#endif /* CE_COMMANDCHANGELABELNAME_H_ */
