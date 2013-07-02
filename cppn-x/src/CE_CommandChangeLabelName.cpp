/*
 * CE_CommandChangeLabelName.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: joost
 */

#include "CE_CommandChangeLabelName.h"
#include "CE_LabelWidget.h"
#include <QInputDialog>

CommandChangeLabelName::CommandChangeLabelName(LabelWidget* labelWidget): labelWidget(labelWidget) {
	ok = false;
	oldName = labelWidget->getText();
	newName = QInputDialog::getText(labelWidget, QString("Label name"), QString("Label name:"), QLineEdit::Normal, oldName, &ok);
	labelWidget->registerObject();
}

CommandChangeLabelName::~CommandChangeLabelName() {
	labelWidget->unregisterObject();
}

void CommandChangeLabelName::undo(){
	labelWidget->setText(oldName);
}

void CommandChangeLabelName::redo(){
	labelWidget->setText(newName);
}
