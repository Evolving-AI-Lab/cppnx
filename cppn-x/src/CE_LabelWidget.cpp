/*
 * CE_Label.cpp
 *
 *  Created on: Jun 3, 2013
 *      Author: joost
 */

#include "CE_LabelWidget.h"
#include "CE_CommandChangeLabelColor.h"
#include "CE_CommandChangeLabelName.h"

#include <QMenu>
#include <QContextMenuEvent>


LabelWidget::LabelWidget(Window* window, QString text, QColor color, bool isDeleted, QWidget *parent):
QFrame(parent), window(window), color(color){
	highlight=false;
	id=0;
	registerdObjects=0;
	deleted = isDeleted;
	//Create actions
	colorAction = new QAction(text, this);
//	deleteAction = new QAction(tr("Delete ") + text, this);
	deleteAction = new QAction(tr("Delete"), this);
	changeColorAction = new QAction(tr("Change color..."), this);
	renameAction = new QAction(tr("Rename..."), this);


	//Create label
	label = new QLabel(text);

	//Create colored button
	colorButton = new QPushButton();
    QString qss = QString("background-color: %1").arg(color.name());
    colorButton->setStyleSheet(qss);
    colorButton->setFixedSize(30, 30);

    //Create delete button
    QPixmap pixmap(10, 10);
    QColor trans = QColor(0, 0, 0, 0);
    pixmap.fill(trans);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.drawLine (2,2, 8,8);
    painter.drawLine (2,8, 8,2);
    deleteButton = new QPushButton();
    deleteButton->setIcon(pixmap);
    deleteButton->setFixedSize(10, 10);

    //Add buttons to this widget
    colorButtonLayout = new QHBoxLayout;
    colorButtonLayout->addWidget(label);
    colorButtonLayout->addWidget(colorButton);
//    colorButtonLayout->addWidget(deleteButton);
    setLayout(colorButtonLayout);

    connect(colorButton, SIGNAL(clicked()), colorAction, SLOT(trigger()));
    connect(deleteButton, SIGNAL(clicked()), deleteAction, SLOT(trigger()));

    connect(changeColorAction, SIGNAL(triggered()), this, SLOT(changeLabelColor()));
    connect(renameAction, SIGNAL(triggered()), this, SLOT(changeLabelName()));

    setAutoFillBackground(true);
    setFrameStyle(QFrame::Panel);

	baseSs = QString("LabelWidget {background: %1; border: 1px solid %2}").arg(palette().color(QPalette::Window).name()).arg(palette().color(QPalette::Window).name());;
	selectedSs = QString("LabelWidget {background: %1; border: 1px solid %2}").arg(palette().color(QPalette::Highlight).name()).arg(palette().color(QPalette::Window).name());;
	highlightSs = QString("LabelWidget {background: %1; border: 1px solid %2}").arg(palette().color(QPalette::Window).name()).arg(QColor(Qt::black).name());;
	selectedHighlightSs = QString("LabelWidget {background: %1; border: 1px solid %2}").arg(palette().color(QPalette::Highlight).name()).arg(QColor(Qt::black).name());;

    setStyleSheet(baseSs);
//    setFocusPolicy(Qt::ClickFocus );

}


LabelWidget::~LabelWidget() {
	delete(label);
	delete(colorButton);
	delete(deleteButton);

}

QString LabelWidget::getText() const{
	return label->text();
}

//std::string LabelWidget::getText() const{
//	return label->text().toStdString();
//}


void LabelWidget::setColor(QColor _color){
	color = _color;
    QString qss = QString("background-color: %1").arg(color.name());
    colorButton->setStyleSheet(qss);
    window->getGraphWidget()->updateAll();
}

void LabelWidget::changeLabelColor(){
	CommandChangeLabelColor* command = new CommandChangeLabelColor(this);
	if(command->isOk()) window->undoStack.push(command);
}

void LabelWidget::changeLabelName(){
	CommandChangeLabelName* command = new CommandChangeLabelName(this);
	if(command->isOk()) window->undoStack.push(command);
}

void LabelWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    menu.addAction(deleteAction);
    menu.addAction(renameAction);
    menu.addAction(changeColorAction);
    menu.exec(event->globalPos());
}

void LabelWidget::focusInEvent(QFocusEvent* event ){
	Q_UNUSED(event);
    if(highlight){
    	setStyleSheet(selectedHighlightSs);
    } else {
    	setStyleSheet(selectedSs);
    }


//	setBackgroundRole(QPalette::Highlight);
}

void LabelWidget::focusOutEvent(QFocusEvent* event ){
	Q_UNUSED(event);
    if(highlight){
    	setStyleSheet(highlightSs);
    } else {
    	setStyleSheet(baseSs);
    }
//	setBackgroundRole(QPalette::Window);
}

void LabelWidget::setText(const QString& name){
	label->setText(name);
}

void LabelWidget::setHighlightOn(){
	highlight = true;
	if(hasFocus()){
		setStyleSheet(selectedHighlightSs);
	} else {
    	setStyleSheet(highlightSs);
    }
}

void LabelWidget::setHighlightOff(){
	highlight = false;
	if(hasFocus()){
		setStyleSheet(selectedSs);
	} else {
    	setStyleSheet(baseSs);
    }
}
