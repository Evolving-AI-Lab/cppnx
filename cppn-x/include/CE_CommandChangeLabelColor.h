/*
 * CE_CommandChangeLabelColor.h
 *
 *  Created on: Jun 30, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDCHANGELABELCOLOR_H_
#define CE_COMMANDCHANGELABELCOLOR_H_

#include <QUndoCommand>
#include <QColor>
//#include "CX_CXUndoCommand.h"

class Label;

class CommandChangeLabelColor: public QUndoCommand {
public:
	CommandChangeLabelColor(Label* labelWidget);
	virtual ~CommandChangeLabelColor();

	void undo();
	void redo();

	bool isOk(){
		return ok;
	}

private:
	QColor color;
	QColor previousColor;
	Label* labelWidget;
	bool ok;
};

#endif /* CE_COMMANDCHANGELABELCOLOR_H_ */
