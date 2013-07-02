/*
 * CE_CommandChangeLabelColor.h
 *
 *  Created on: Jun 30, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDCHANGELABELCOLOR_H_
#define CE_COMMANDCHANGELABELCOLOR_H_

#include <qundostack.h>
#include <QColor>

class LabelWidget;

class CommandChangeLabelColor: public QUndoCommand {
public:
	CommandChangeLabelColor(LabelWidget* labelWidget);
	virtual ~CommandChangeLabelColor();

	void undo();
	void redo();

	bool isOk(){
		return ok;
	}

private:
	QColor color;
	QColor previousColor;
	LabelWidget* labelWidget;
	bool ok;
};

#endif /* CE_COMMANDCHANGELABELCOLOR_H_ */
