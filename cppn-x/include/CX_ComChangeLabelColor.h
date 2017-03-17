/*
 * CE_CommandChangeLabelColor.h
 *
 *  Created on: Jun 30, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDCHANGELABELCOLOR_H_
#define CE_COMMANDCHANGELABELCOLOR_H_

#include <QColor>
#include <iostream>
#include "CX_ComBase.h"

class Label;

class CommandChangeLabelColor: public ComBase {
public:
	CommandChangeLabelColor(Label* labelWidget);
	virtual ~CommandChangeLabelColor();

	void undo();
	void redo();

	bool isOk() const{return ok;}

private:
	QColor color;
	QColor previousColor;
	Label* labelWidget;
	bool ok;
};

#endif /* CE_COMMANDCHANGELABELCOLOR_H_ */
