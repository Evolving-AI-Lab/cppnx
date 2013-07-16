/*
 * CE_CommandChangeLabelName.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: joost
 */

#include "CE_CommandChangeLabelName.h"
#include "CE_Label.h"
#include <QInputDialog>

CommandChangeLabelName::CommandChangeLabelName(Label* labelWidget): labelWidget(labelWidget) {
	ok = false;
	oldName = labelWidget->getText();
	newName = QInputDialog::getText(0, QString("Label name"), QString("Label name:"), QLineEdit::Normal, oldName, &ok);
	labelWidget->registerObject();
}

CommandChangeLabelName::~CommandChangeLabelName() {
	labelWidget->unregisterObject();
}

void CommandChangeLabelName::undo(){
	labelWidget->setText(oldName);
//	labelWidget->update();
}

void CommandChangeLabelName::redo(){
	labelWidget->setText(newName);
//	labelWidget->update();
}
