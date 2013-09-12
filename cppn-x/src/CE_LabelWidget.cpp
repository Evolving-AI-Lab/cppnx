/*
 * CE_LabelWidget.cpp
 *
 *  Created on: Jul 2, 2013
 *      Author: joost
 */

#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QSignalMapper>
#include <QMenu>
#include <QInputDialog>
#include <QColorDialog>



#include "CE_LabelWidget.h"
//#include "CE_Window.h"
#include "CE_VerticalScrollArea.h"
#include "CE_CommandLabelObject.h"
#include "CE_CommandAddLabel.h"

LabelWidget::LabelWidget() {
    applyLabelAction = new QAction(tr("Apply label"), this);
    applyLabelAction->setShortcut(tr("Alt+`"));
    applyLabelAction->setStatusTip(tr("Apply the selected label."));

    addLabelAction = new QAction(tr("New &label"), this);
    addLabelAction->setShortcut(tr("Alt+L"));
    addLabelAction->setStatusTip(tr("Create a new label"));

    unlabelAction = new QAction(tr("Unlabel"), this);
    unlabelAction->setShortcut(tr("Alt+0"));
    unlabelAction->setStatusTip(tr("Remove a label from an existing object."));

    renameAction = new QAction(tr("Rename..."), this);
    renameAction->setShortcut(tr("F2"));
    renameAction->setStatusTip(tr("Change the name of the selected label."));

    changeColorAction = new QAction(tr("Change color..."), this);
    changeColorAction->setShortcut(tr("F3"));
    changeColorAction->setStatusTip(tr("Change the color of the select label."));



    labelMenu = new QMenu(tr("&Labels"), this);
    nodeContextMenu = new QMenu(tr("&Labels"), this);

    labelContextMenu = new QMenu(tr("Label context menu"));
    labelContextMenu->addAction(renameAction);
    labelContextMenu->addAction(changeColorAction);



	//Create 'add label' button
	addLabelButton = new QPushButton;
	addLabelButton->setText(tr("add label"));

	//Create 'unlabel' button
	unlabelButton = new QPushButton;
	unlabelButton->setText(tr("unlabel"));
	unlabelButton->setDisabled(true);

	//Create 'add label' text field
	labelName = new QLineEdit;

	dragAndDropLabelBar = new DragAndDropGraphicsView();
	dragAndDropLabelBar->setMinimumSize(220, 210);
	dragAndDropLabelBar->setAlignment(Qt::AlignTop);
//	dragAndDropLabelBar->setAlignment(Qt::AlignHCenter);
	dragAndDropLabelBar->setDragMode(QGraphicsView::ScrollHandDrag);
	dragAndDropLabelBar->scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
	dragAndDropLabelBar->scene()->setSceneRect(0, 0, 100, 100);
	dragAndDropLabelBar->setContentsMargin(5.0);
	dragAndDropLabelBar->setTopMargin(2.0);
	dragAndDropLabelBar->setBackgroundRole(QPalette::Window);
	connect(dragAndDropLabelBar, SIGNAL(requestCommandExecution(ComBase*)),this, SIGNAL(requestCommandExecution(ComBase*)));
	connect(dragAndDropLabelBar, SIGNAL(objectsMoved()),this, SLOT(rebuildMenu()));
	connect(dragAndDropLabelBar->scene(), SIGNAL(selectionChanged()),this, SLOT(updateSelection()));
	connect(dragAndDropLabelBar->scene(), SIGNAL(selectionChanged()),this, SIGNAL(selectionChanged()));



	//Create color layout
    colorMainLayout = new QVBoxLayout();
    colorMainLayout->setAlignment(Qt::AlignTop);
    colorMainLayout->addWidget(labelName);
    colorMainLayout->addWidget(addLabelButton);
    colorMainLayout->addWidget(unlabelButton);
    colorMainLayout->addWidget(dragAndDropLabelBar);
    colorMainLayout->setMargin(0);

    setLayout(colorMainLayout);
//    setContentsMargins(0,0,0,0);


    //Connect add label button
    connect(addLabelButton, SIGNAL(clicked()), addLabelAction, SLOT(trigger()));
    connect(labelName, SIGNAL(returnPressed()),addLabelAction,SLOT(trigger()));
    connect(unlabelButton, SIGNAL(clicked()),unlabelAction,SLOT(trigger()));

    connect(addLabelAction, SIGNAL(triggered()), this, SLOT(addLabel()));
    connect(unlabelAction, SIGNAL(triggered()), this, SLOT(unlabel()));
    connect(renameAction, SIGNAL(triggered()), this, SLOT(rename()));
    connect(changeColorAction, SIGNAL(triggered()), this, SLOT(recolor()));
    connect(applyLabelAction, SIGNAL(triggered()), this, SLOT(applyLabel()));



    rebuildMenu();
    labelSelected(false);
    labelableObjectSelected(false);
    singleLabelSelected(false);
}

LabelWidget::~LabelWidget() {
	// TODO Auto-generated destructor stub
}

void LabelWidget::clear(){
	QList<Label*> toDelete;
	for(int i=0; i<dragAndDropLabelBar->getNrOfItems(); i++){
		Label* label = qgraphicsitem_cast<Label*>(dragAndDropLabelBar->getItem(i));
		if(label) toDelete.append(label);
	}

	foreach(Label *label, toDelete){
		removeLabel(label);
	}
}

void LabelWidget::addLabel(){
	//Get label text
    bool ok = true;
    QString labelText = labelName->displayText();
    labelName->clear();
    if(labelText.isEmpty()) labelText = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("Label name:"), QLineEdit::Normal, "", &ok);
    if(!ok) return;

    //Get label color
	QColor color = QColorDialog::getColor(Qt::white, this);
    if(!color.isValid()) return;

    Label* label = new Label(labelText, color, false);
//    std::cout << "newLabel: " << label <<std::endl;
    emit requestCommandExecution(new CommandAddLabel(this, label, true));
    emit applyLabel(label);
    setWindowModified(true);
}

void LabelWidget::addLabel(Label* label){
	label->registerObject();
	label->setContextMenu(labelContextMenu);

	if(label->isDeleted()){
		dragAndDropLabelBar->insertDragAndDropObject(label, label->getIndex());
	} else {
		dragAndDropLabelBar->addDragAndDropObject(label);
	}

	rebuildMenu();

    connect(label, SIGNAL(applyLabel(Label*)), this, SIGNAL(applyLabel(Label*)));
    connect(label, SIGNAL(deleteLabel(Label*)), this, SLOT(requestRemoveLabel(Label*)));
    connect(label, SIGNAL(requestCommandExecution(ComBase*)),this, SIGNAL(requestCommandExecution(ComBase*)));
    connect(label, SIGNAL(labelChanged()),this, SIGNAL(labelsChanged()));
    emit labelsChanged();
}

void LabelWidget::setLabels(QList<Label*> labels){
	clear();
	foreach(Label* label, labels){
		addLabel(label);
	}
}

void LabelWidget::requestRemoveLabel(Label* label){
	emit requestCommandExecution(new CommandAddLabel(this, label, false));
}

void LabelWidget::removeLabel(Label* label){
	label->setDeleted();
    disconnect(label, SIGNAL(applyLabel(Label*)), this, SIGNAL(applyLabel(Label*)));
    disconnect(label, SIGNAL(deleteLabel(Label*)), this, SLOT(requestRemoveLabel(Label*)));
    disconnect(label, SIGNAL(requestCommandExecution(ComBase*)),this, SIGNAL(requestCommandExecution(ComBase*)));
    disconnect(label, SIGNAL(labelChanged()),this, SIGNAL(labelsChanged()));
//    disconnect(label, SIGNAL(beforeSelected()),this, SIGNAL(selectionChanged()));

	dragAndDropLabelBar->removeDragAndDropObject(label);
	rebuildMenu();
	label->unregisterObject();
    emit labelsChanged();
}

int LabelWidget::getNrOfLabels(){
	return dragAndDropLabelBar->getNrOfItems();
}

Label* LabelWidget::getLabel(size_t i){
	return qgraphicsitem_cast<Label*>(dragAndDropLabelBar->getItem(i));
}

QList<Label*> LabelWidget::getLabels(){
	QList<Label*> result;

	for(int i=0; i<getNrOfLabels(); i++){
		result.append(getLabel(i));
	}

	return result;
}

void LabelWidget::labelableObjectSelected(bool selected){
	labelableObjectIsSelected = selected;
	unlabelAction->setEnabled(selected);
	unlabelButton->setEnabled(selected);
	applyLabelAction->setEnabled(selected && singleLabelIsSelected);
}

void LabelWidget::labelSelected(bool labelSelected){
	labelIsSelected = labelSelected;
}

void LabelWidget::singleLabelSelected(bool singleLabelSelected){
	singleLabelIsSelected = singleLabelSelected;
	renameAction->setEnabled(singleLabelSelected);
	changeColorAction->setEnabled(singleLabelSelected);
	applyLabelAction->setEnabled(singleLabelSelected && labelableObjectIsSelected);
}


void LabelWidget::unlabel(){
	emit applyLabel(new Label());
}

void LabelWidget::rebuildMenu(){
	QList<QAction*> labelActions;

	labelMenu->clear();
    labelMenu->addAction(renameAction);
    labelMenu->addAction(changeColorAction);
	labelMenu->addSeparator();
	labelMenu->addAction(addLabelAction);
	labelMenu->addAction(applyLabelAction);
	labelMenu->addAction(unlabelAction);
	labelMenu->addSeparator();

	nodeContextMenu->clear();
	nodeContextMenu->addAction(addLabelAction);
	nodeContextMenu->addAction(applyLabelAction);
	nodeContextMenu->addAction(unlabelAction);
	nodeContextMenu->addSeparator();

	for(int i=0; i<getNrOfLabels(); i++){
		getLabel(i)->setDeleted(false);
		labelActions.append(getLabel(i)->getColorAction());
	}

	labelMenu->addActions(labelActions);
	nodeContextMenu->addActions(labelActions);
}

void LabelWidget::requestDelete(){
	emit LabelWidget::requestCommandExecution(new CommandAddLabel(this, dragAndDropLabelBar->scene()->selectedItems(), false));
}

void LabelWidget::updateSelection(){
	int labelsSelected = 0;
	foreach(QGraphicsItem* item, dragAndDropLabelBar->scene()->selectedItems()){
		if(qgraphicsitem_cast<Label*>(item)){
			labelsSelected++;
			if(labelsSelected > 1) break;
		}
	}

	labelSelected(labelsSelected > 0);
	singleLabelSelected(labelsSelected == 1);
}

void LabelWidget::rename(){
	if(dragAndDropLabelBar->scene()->selectedItems().size() == 1){
		Label* label = qgraphicsitem_cast<Label*>(dragAndDropLabelBar->scene()->selectedItems().front());
		if(label) label->changeLabelName();
	}
}

void LabelWidget::recolor(){
	if(dragAndDropLabelBar->scene()->selectedItems().size() == 1){
		Label* label = qgraphicsitem_cast<Label*>(dragAndDropLabelBar->scene()->selectedItems().front());
		if(label) label->changeLabelColor();
	}
}

void LabelWidget::applyLabel(){
	if(dragAndDropLabelBar->scene()->selectedItems().size() == 1){
		Label* label = qgraphicsitem_cast<Label*>(dragAndDropLabelBar->scene()->selectedItems().front());
		if(label) label->applyLabel();
	}
}


