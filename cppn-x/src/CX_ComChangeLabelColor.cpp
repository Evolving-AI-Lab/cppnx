/*
 * CE_CommandChangeLabelColor.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: joost
 */



#include <CX_ComChangeLabelColor.h>
#include "CE_Label.h"

#include <QColorDialog>

CommandChangeLabelColor::CommandChangeLabelColor(Label* labelWidget): labelWidget(labelWidget) {
	previousColor = labelWidget->getColor();
	color = QColorDialog::getColor(previousColor);
    ok = color.isValid();
    labelWidget->registerObject();
}

CommandChangeLabelColor::~CommandChangeLabelColor() {
	labelWidget->unregisterObject();
}

void CommandChangeLabelColor::undo(){
	labelWidget->setColor(previousColor);
}

void CommandChangeLabelColor::redo(){
	labelWidget->setColor(color);
}
