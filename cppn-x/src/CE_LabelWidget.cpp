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
    addLabelAction = new QAction(tr("Create &label"), this);
    addLabelAction->setShortcut(tr("Alt+L"));
    addLabelAction->setStatusTip(tr("Create a new label"));

    unlabelAction = new QAction(tr("Unlabel"), this);
    unlabelAction->setShortcut(tr("Alt+0"));
    unlabelAction->setStatusTip(tr("Remove a label from an existing object."));

    deleteAction = new QAction(tr("Delete label"), this);

    labelMenu = new QMenu(tr("&Labels"), this);


	//Create 'add label' button
	addLabelButton = new QPushButton;
	addLabelButton->setText(tr("add label"));

	unlabelButton = new QPushButton;
	unlabelButton->setText(tr("unlabel"));
	unlabelButton->setDisabled(true);


	//Create 'add label' text field
	labelName = new QLineEdit;

//    colorLabelLayout = new QVBoxLayout();
//    colorLabelLayout->setAlignment(Qt::AlignTop);
//    colorLabelLayout->setSpacing(1);

    QPushButton* test = new QPushButton;
    test->setText(tr("add label"));

//	labelBar = new VerticalScrollArea();
//	labelBar->m_scrollAreaWidgetContents->setLayout(colorLabelLayout);

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
	connect(dragAndDropLabelBar, SIGNAL(requestCommandExecution(QUndoCommand*)),this, SIGNAL(requestCommandExecution(QUndoCommand*)));
	connect(dragAndDropLabelBar, SIGNAL(objectsMoved()),this, SLOT(rebuildMenu()));
	connect(dragAndDropLabelBar->scene(), SIGNAL(selectionChanged()),this, SLOT(updateSelection()));



	//Create color layout
    colorMainLayout = new QVBoxLayout();
    colorMainLayout->setAlignment(Qt::AlignTop);
    colorMainLayout->addWidget(labelName);
    colorMainLayout->addWidget(addLabelButton);
    colorMainLayout->addWidget(unlabelButton);
//    colorMainLayout->addWidget(labelBar);
    colorMainLayout->addWidget(dragAndDropLabelBar);
    colorMainLayout->setMargin(0);


    //Create signal mappers
//    deleteSignalMapper = new QSignalMapper (this) ;
//    colorSignalMapper = new QSignalMapper (this) ;
//    connect (deleteSignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(removeLabel(QWidget*))) ;
//    connect (colorSignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(applyLabel(QWidget*)));

    setLayout(colorMainLayout);
//    setContentsMargins(0,0,0,0);


    //Connect add label button
    connect(addLabelButton, SIGNAL(clicked()), addLabelAction, SLOT(trigger()));
    connect(labelName, SIGNAL(returnPressed()),addLabelAction,SLOT(trigger()));
    connect(unlabelButton, SIGNAL(clicked()),unlabelAction,SLOT(trigger()));

    connect(addLabelAction, SIGNAL(triggered()), this, SLOT(addLabel()));
    connect(unlabelAction, SIGNAL(triggered()), this, SLOT(unlabel()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(requestDelete()));

    rebuildMenu();
    labelSelected(false);
    labelableObjectSelected(false);
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

//	foreach(QGraphicsItem* item, dragAndDropLabelBar->scene()->items()){
//		Label* label = qgraphicsitem_cast<Label*>(item);
//		if(label) toDelete.append(label);
//	}

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
//	std::cout << "addLabelWidget: " << label <<std::endl;
	label->registerObject();

	if(label->isDeleted()){
		dragAndDropLabelBar->insertDragAndDropObject(label, label->getIndex());
	} else {
		dragAndDropLabelBar->addDragAndDropObject(label);
	}
//	if(label->getId() == 0){
////		colorLabelLayout->addWidget(label);
////		label->setId(colorLabelLayout->count());
//		dragAndDropLabelBar->addDragAndDropObject(label);
//	} else {
////		colorLabelLayout->insertWidget(label->getId()-1, label);
//		dragAndDropLabelBar->insertDragAndDropObject(label, label->getId()-1);
//	}

	rebuildMenu();
//	label->setDeleted(false);
//    setIds();

//    if(label->getIndex() < (getNrOfLabels()-1)){
//    	labelMenu->insertAction(getLabel(label->getIndex()+1)->getColorAction(), label->getColorAction());
//    } else {
//    	labelMenu->addAction(label->getColorAction());
//    }

    //Map actions
//    deleteSignalMapper -> setMapping (label->getDeleteAction(), label);
//    colorSignalMapper -> setMapping (label->getColorAction(), label);

	label->getContextMenu()->insertAction(label->getContextMenu()->actions().front(), deleteAction);
    connect(label, SIGNAL(applyLabel(Label*)), this, SIGNAL(applyLabel(Label*)));
    connect(label, SIGNAL(deleteLabel(Label*)), this, SLOT(requestRemoveLabel(Label*)));
    connect(label, SIGNAL(requestCommandExecution(QUndoCommand*)),this, SIGNAL(requestCommandExecution(QUndoCommand*)));
    connect(label, SIGNAL(labelChanged()),this, SIGNAL(labelsChanged()));
//    connect(label, SIGNAL(indexChanged(uint)),this, SLOT(indexChanged(uint)));
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
    disconnect(label, SIGNAL(requestCommandExecution(QUndoCommand*)),this, SIGNAL(requestCommandExecution(QUndoCommand*)));
    disconnect(label, SIGNAL(labelChanged()),this, SIGNAL(labelsChanged()));
//    disconnect(label, SIGNAL(indexChanged(uint)),this, SLOT(indexChanged(uint)));


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
//	return qobject_cast<Label*>(dynamic_cast <QWidgetItem*>(colorLabelLayout->itemAt(i))->widget());
}

QList<Label*> LabelWidget::getLabels(){
	QList<Label*> result;

	for(int i=0; i<getNrOfLabels(); i++){
		result.append(getLabel(i));
	}

	return result;
}

void LabelWidget::labelableObjectSelected(bool selected){
	unlabelAction->setEnabled(selected);
	unlabelButton->setEnabled(selected);
}

void LabelWidget::labelSelected(bool labelSelected){
	labelIsSelected = labelSelected;
	deleteAction->setEnabled(labelSelected);
}


void LabelWidget::unlabel(){
	emit applyLabel(new Label());
}

//void LabelWidget::setIds(){
//	foreach(QGraphicsItem* item, dragAndDropLabelBar->scene()->items()){
//		Label* label = qgraphicsitem_cast<Label*>(item);
//		if(label) label->setDeleted(false);
//	}
//}

//void LabelWidget::indexChanged(uint index){
//	Label* label = qgraphicsitem_cast<Label*>(dragAndDropLabelBar->getItem(index));
//	if(label) label->setDeleted(false);
//}
//
//void LabelWidget::objectsMoved(){
//	std::cout << "Objects Moved" << std::endl;
//	setIds();
//}

void LabelWidget::rebuildMenu(){
	labelMenu->clear();
    labelMenu->addAction(addLabelAction);
    labelMenu->addAction(unlabelAction);
    labelMenu->addAction(deleteAction);
    labelMenu->addSeparator();
	for(int i=0; i<getNrOfLabels(); i++){
		getLabel(i)->setDeleted(false);
		labelMenu->addAction(getLabel(i)->getColorAction());
	}
}

void LabelWidget::requestDelete(){
	emit LabelWidget::requestCommandExecution(new CommandAddLabel(this, dragAndDropLabelBar->scene()->selectedItems(), false));
}

void LabelWidget::updateSelection(){
	labelIsSelected = false;
	foreach(QGraphicsItem* item, dragAndDropLabelBar->scene()->selectedItems()){
		if(qgraphicsitem_cast<Label*>(item)){
			labelIsSelected = true;
			break;
		}
	}

	labelSelected(labelIsSelected);
}

