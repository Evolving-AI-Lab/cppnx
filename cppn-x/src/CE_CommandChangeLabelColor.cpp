/*
 * CE_CommandChangeLabelColor.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: joost
 */



#include "CE_CommandChangeLabelColor.h"
#include "CE_Label.h"

#include <QColorDialog>

CommandChangeLabelColor::CommandChangeLabelColor(Label* labelWidget): labelWidget(labelWidget) {
	previousColor = labelWidget->getColor();
	color = QColorDialog::getColor(Qt::white);
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
