/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "CE_CppnWidget.h"


#include <QtGui>

#include <cmath>
#include "CE_Util.h"

#include "CE_CppnParser.h"
#include "CE_Cppn.h"
#include "CE_CppnWriter.h"
#include "CX_ComSetBookends.h"


const double CppnWidget::left_border_min = -400.0;
const double CppnWidget::right_border_min = 400.0;
const double CppnWidget::top_border_min = -600.0;
const double CppnWidget::bottom_border_min = 600.0;
const double CppnWidget::extra_space = 80;

//! [0]
CppnWidget::CppnWidget(QWidget* widget)
    : ContextMenuGraphicsView(widget)
{
	cppn = 0;
	blockWeightUpdates = false;
	labelMode = Edge::onlyLabels;

    left_border = left_border_min;
    right_border = right_border_min;
    top_border = top_border_min;
    bottom_border = bottom_border_min;

//    QGraphicsScene *scene = new QGraphicsScene(this);
    scene()->setSceneRect(left_border, top_border, (-left_border)+right_border, (-top_border)+bottom_border);
    connect(scene(), SIGNAL(selectionChanged()), this, SLOT(updateSelection()));
//    setScene(scene);


    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setDragMode(ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    addNodeviewAction = new QAction(tr("&Add to sidebar"), this);
    addNodeviewAction->setShortcut(tr("Alt+A"));
    addNodeviewAction->setStatusTip(tr("Add the selected node to the sidebar"));

    saveImageAction = new QAction(tr("&Export nodes as jpg"), this);
//    saveImageAction->setShortcut(tr("Alt+N"));
    saveImageAction->setStatusTip(tr("Saves the current images of the nodes as jpg pictures"));


    labelOnlyAction = new QAction(tr("Label only"), this);
    labelOnlyAction->setStatusTip(tr("Capture the current screen"));
    labelOnlyAction->setCheckable(true);

    signOnlyAction = new QAction(tr("Sign only"), this);
    signOnlyAction->setStatusTip(tr("Capture the current screen"));
    signOnlyAction->setCheckable(true);

    labelAndSignAction = new QAction(tr("Sign and label"), this);
    labelAndSignAction->setStatusTip(tr("Capture the current screen"));
    labelAndSignAction->setCheckable(true);

    viewGroup = new QActionGroup(this);
    viewGroup->addAction(labelOnlyAction);
    viewGroup->addAction(signOnlyAction);
    viewGroup->addAction(labelAndSignAction);
    labelOnlyAction->setChecked(true);

    circleAction = new QAction(tr("&Layers"), this);
    circleAction->setStatusTip(tr("Undo the last operation"));

    layerAction = new QAction(tr("&Circle"), this);
    layerAction->setStatusTip(tr("Undo the last operation"));

    connect(saveImageAction, SIGNAL(triggered()), this, SLOT(saveImage()));

    connect(addNodeviewAction, SIGNAL(triggered()), this, SLOT(addNodeView()));
    connect(labelOnlyAction, SIGNAL(triggered()), this, SLOT(setLabelView()));
    connect(signOnlyAction, SIGNAL(triggered()), this, SLOT(setSignView()));
    connect(labelAndSignAction, SIGNAL(triggered()), this, SLOT(setLabelAndSignView()));
    connect(circleAction, SIGNAL(triggered()), this, SLOT(positionNodesLayers()));
    connect(layerAction, SIGNAL(triggered()), this, SLOT(positionNodesCircle()));

    addAction(addNodeviewAction);
    addAction(labelOnlyAction);
    addAction(signOnlyAction);
    addAction(labelAndSignAction);
    addAction(circleAction);
    addAction(layerAction);

    setNodeSelected(false);

    nodeMenu = new QMenu(tr("Node"), this);
    nodeMenu->addAction(addNodeviewAction);
    nodeMenu->addAction(saveImageAction);

    edgeMenu = new QMenu(tr("Edge"), this);
}



void CppnWidget::updateAll(){
	foreach(QGraphicsItem* item, scene()->items()){
		item->update();
	}
}

void CppnWidget::setCppn(QList<Node*> nodes, QList<Edge*> edges)
{
	//Clean previous objects
	if(cppn) delete cppn;
	x_sorted.clear();
	y_sorted.clear();
	scene()->clear();

	cppn = new Cppn;

	foreach(Node* node, nodes){
		addNode(node);
	}

	foreach(Edge* edge, edges){
		addEdge(edge);
	}

	cppn->updateNodes();
}

void CppnWidget::itemMoved(Node * node)
{


	size_t x_index = node->x_index;
//	std::cout << "x_index: " << x_index << std::endl;
	while(x_index > 0 ? x_sorted[x_index-1]->pos().x() > node->pos().x() :  false){
		x_sorted[x_index] = x_sorted[x_index-1];
		x_sorted[x_index]->x_index = x_index;
		x_sorted[x_index-1]=node;
		x_index--;
	}
//	std::cout << "x_index: " << x_index << std::endl;
	while(x_index < (x_sorted.size()-1) ? x_sorted[x_index+1]->pos().x() < node->pos().x() :  false){
		x_sorted[x_index] = x_sorted[x_index+1];
		x_sorted[x_index]->x_index = x_index;
		x_sorted[x_index+1]=node;
		x_index++;
	}
//	std::cout << "x_index: " << x_index << std::endl;

	size_t y_index = node->y_index;
//	std::cout << "y_index: " << y_index << std::endl;
	while(y_index > 0 ? y_sorted[y_index-1]->pos().y() > node->pos().y() :  false){
		y_sorted[y_index] = y_sorted[y_index-1];
		y_sorted[y_index]->y_index = y_index;
		y_sorted[y_index-1]=node;
		y_index--;
	}
//	std::cout << "y_index: " << y_index << std::endl;
	while(y_index < (y_sorted.size()-1) ? y_sorted[y_index+1]->pos().y() < node->pos().y() :  false){
		y_sorted[y_index] = y_sorted[y_index+1];
		y_sorted[y_index]->y_index = y_index;
		y_sorted[y_index+1]=node;
		y_index++;
	}
//	std::cout << "y_index: " << y_index << std::endl;

	node->x_index = x_index;
	node->y_index = y_index;

	//			std::cout << (*it)->pos().x() << std::endl;



//	for(std::vector<Node*>::iterator it=x_sorted.begin(); it != x_sorted.end(); it++){
//		std::cout << (*it)->pos().x()  <<std::endl;
//	}
//
//	if(x_index == 0){
//		left_border = std::min(node->pos().x() - extra_space, left_border_min);
//	} else if(x_index == (x_sorted.size()-1)) {
//		std::cout << "Node x: " << node->pos().x() << std::endl;
//		right_border = std::max(node->pos().x() + extra_space, right_border_min);
//	}

	left_border = std::min(x_sorted[0]->pos().x() - extra_space, left_border_min);
	right_border = std::max(x_sorted[x_sorted.size()-1]->pos().x() + extra_space, right_border_min);
	top_border = std::min(y_sorted[0]->pos().y() - extra_space, top_border_min);
	bottom_border = std::max(y_sorted[y_sorted.size()-1]->pos().y() + extra_space, bottom_border_min);

//	if(y_index == 0){
//		top_border = std::min(node->pos().y() - extra_space, top_border_min);
//	} else if(y_index == (y_sorted.size()-1)) {
//		bottom_border = std::max(node->pos().y() + extra_space, bottom_border_min);
//	}

//	std::cout << "Borders: " << left_border << " " << top_border << " " << (-left_border) + right_border << " " << (-top_border) + bottom_border << std::endl;

	scene()->setSceneRect(QRectF(left_border,top_border,(-left_border) + right_border,(-top_border) + bottom_border));
}

void CppnWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(std::pow((double)2, event->delta() / 240.0));
}

void CppnWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}


void CppnWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void CppnWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

void CppnWidget::setWeight(double weight){
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	blockWeightUpdates=true;
	if(edge) emit requestCommandExecution(new CommandSetWeight(edge, weight));
	blockWeightUpdates=false;
//	emit sceneModified();
}

void CppnWidget::onBookendStartChanged(double bookendStart){
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge){
		edge->blockSignals(true);
//		std::cout << "Bookend start: " << bookendStart <<std::endl;
		emit requestCommandExecution(new ComSetBookends(edge, bookendStart, edge->getBookendEnd()));
		edge->blockSignals(false);
	}
}

void CppnWidget::onBookendEndChanged(double bookendEnd){
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge){
		edge->blockSignals(true);
//		std::cout << "Bookend end: " << bookendEnd <<std::endl;
		emit requestCommandExecution(new ComSetBookends(edge, edge->getBookendStart(), bookendEnd));
		edge->blockSignals(false);
	}

}

void CppnWidget::resetWeight(){
	emit requestCommandExecution(new CommandSetWeight(scene()->selectedItems()));
}

void CppnWidget::resetAllWeights(){
	emit requestCommandExecution(new CommandSetWeight(scene()->items()));
//	emit sceneModified();
}

void CppnWidget::weightUpdated(Edge* edge){
//	std::cout << "Update edge: " << edge->getBranch() << "_" << edge->getId() << std::endl;
	if(blockWeightUpdates) return;
	blockSignals(true);
	scene()->clearSelection();
	blockSignals(false);
	edge->setSelected(true);

//	emit edgeUpdated(edge);
//
//	if(blockWeightUpdates || scene()->selectedItems().count() != 1) return;
//	if(edge == scene()->selectedItems().front())
}

void CppnWidget::positionNodesLayers(){
	cppn->positionNodes();
	emit requestCommandExecution(new CommandSetPos(scene()->items()));
}

void CppnWidget::positionNodesCircle(){
	cppn->positionNodesCircle();
	emit requestCommandExecution(new CommandSetPos(scene()->items()));
}

void CppnWidget::addNode(Node* node){
//	std::cout << node->pos().x() << " " << node->pos().y() << std::endl;
	scene()->addItem(node);
	cppn->addNode(node);
	node->setPrevPos(node->pos());
	node->setContextMenu(nodeMenu);
	connect(node, SIGNAL(updatePositionsRequest()), this, SLOT(updatePreviousPositions()));
	connect(node, SIGNAL(positionChanged(Node*)), this, SLOT(itemMoved(Node*)));
	connect(node, SIGNAL(updateRequest(Node*)), cppn, SLOT(updateNode(Node*)));
	connect(node, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));


	//Set nodes in position vectors
	std::vector<Node*>::iterator it;
	for(it=x_sorted.begin(); it != x_sorted.end(); it++){
		if((*it)->pos().x() > node->pos().x()){
			x_sorted.insert(it, node);
			break;
		}
	}
	if(it == x_sorted.end()) x_sorted.push_back(node);
	for(size_t i=0; i<x_sorted.size(); i++){
		x_sorted[i]->x_index = i;
	}

	for(it=y_sorted.begin(); it != y_sorted.end(); it++){
		if((*it)->pos().y() > node->pos().y()){
			y_sorted.insert(it, node);
			break;
		}
	}
	if(it == y_sorted.end()) y_sorted.push_back(node);
	for(size_t i=0; i<y_sorted.size(); i++){
		y_sorted[i]->y_index = i;
	}

	itemMoved(node);
}

void CppnWidget::addEdge(Edge* edge){
	edge->setLabelMode(&labelMode);
	edge->setContextMenu(edgeMenu);
	scene()->addItem(edge);
	cppn->addConnection(edge);
	connect(edge, SIGNAL(weightChanged(Edge*, double, bool)), cppn, SLOT(setWeight(Edge*, double, bool)));
	connect(edge, SIGNAL(weightChanged(Edge*, double, bool)), this, SLOT(weightUpdated(Edge*)));
	connect(edge, SIGNAL(requestUpdateAll()), cppn, SLOT(updateNodes()));
	connect(edge, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
	connect(edge, SIGNAL(bookendsChanged(Edge*)), this, SLOT(weightUpdated(Edge*)));
}

void CppnWidget::applyLabel(Label* label){
	if(scene()->selectedItems().count() == 0) return;
	emit requestCommandExecution(new CommandLabelObject(scene()->selectedItems(), label));
	emit sceneModified();
}

void CppnWidget::addNodeView(){
	emit requestAddNodeview(scene()->selectedItems());
}

void CppnWidget::updateSelection(){
	nodeSelected = false;
	edgeSelected = false;

	foreach(QGraphicsItem* item, scene()->selectedItems()){
		if(qgraphicsitem_cast<Node*>(item)) nodeSelected = true;
		if(qgraphicsitem_cast<Edge*>(item)) edgeSelected = true;
		if(edgeSelected && nodeSelected) break;
	}
//	std::cout << edgeSelected << " " << scene()->selectedItems().count() << std::endl;

	if(edgeSelected && scene()->selectedItems().count() == 1){
		emit edgeUpdated(qgraphicsitem_cast<Edge*>(scene()->selectedItems().front()));
	} else {
		emit edgeUpdated(0);
	}

	emit labelableObjectSelected(edgeSelected || nodeSelected);

	setNodeSelected(nodeSelected);
	setEdgeSelected(edgeSelected);
}

void CppnWidget::updatePreviousPositions(){
	emit requestCommandExecution(new CommandSetPos(scene()->selectedItems()));
	foreach(QGraphicsItem* item, scene()->selectedItems()){
		Node* node = qgraphicsitem_cast<Node*>(item);
		if(node) itemMoved(node);
	}
}

void CppnWidget::setLabelView(){
	labelMode = Edge::onlyLabels;
	update();
}

void CppnWidget::setSignView(){
 	labelMode = Edge::onlyConnectionSign;
 	update();
}

void CppnWidget::setLabelAndSignView(){
	labelMode = Edge::both;
	update();
}

void CppnWidget::changeEvent(QEvent* event){
	switch(event->type ()){
	case(QEvent::EnabledChange):
		foreach(QAction* action, actions()){
			action->setEnabled(isEnabled());
		}
		if(isEnabled()){
			setNodeSelected(nodeSelected);
			setEdgeSelected(edgeSelected);
		}
	break;
	default:
		//Nix
	break;
	}
}

void CppnWidget::setNodeSelected(bool selected){
	nodeSelected = selected;
	addNodeviewAction->setEnabled(selected);
	saveImageAction->setEnabled(selected);
}

void CppnWidget::setEdgeSelected(bool selected){
	edgeSelected = selected;
}

void CppnWidget::saveImage(){
	if(nodeSelected){
		if(scene()->selectedItems().count() > 1){
			QString captureDirectory = QFileDialog::getExistingDirectory(this, tr("Chose save directory"), "", QFileDialog::ShowDirsOnly);
			if(captureDirectory.isEmpty()) return;

			foreach(QGraphicsItem* item, scene()->selectedItems()){
				Node* node = qgraphicsitem_cast<Node*>(item);
				if(node){
					QString newFileName = captureDirectory + "/node_" + util::toQString(node->getBranch()) + "_" + util::toQString(node->getId()) + ".jpg";
					node->getImage()->save(newFileName);
				}
			}
		} else {
			Node* node = qgraphicsitem_cast<Node*>(scene()->selectedItems().front());
			if(node){
				QString newFileName = QFileDialog::getSaveFileName(this, tr("Save Node Image"), "",tr("JPEG File (*.jpg);;All Files (*)"));
				if(newFileName.isEmpty()) return;
				node->getImage()->save(newFileName);
			}
		}
	}

}
