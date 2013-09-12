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
//#include "modularity.hpp"
//#include "opt_placement.hpp"


const double CppnWidget::left_border_min = -400.0;
const double CppnWidget::right_border_min = 400.0;
const double CppnWidget::top_border_min = -600.0;
const double CppnWidget::bottom_border_min = 600.0;
const double CppnWidget::extra_space = 80;

//! [0]
CppnWidget::CppnWidget(QWidget* widget)
    : ContextMenuGraphicsView(widget), x_sorted(Node::x_pos), y_sorted(Node::y_pos)
{
	cppn = 0;
	blockWeightUpdates = false;
	firstClick = false;
	labelMode = Edge::onlyLabels;
	lineMode = Edge::curved;
	nodeLabelMode = Node::labels;

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


    nodeLabelAction = new QAction(tr("Show node labels"), this);
    nodeLabelAction->setShortcut(tr("Ctrl+4"));
    nodeLabelAction->setStatusTip(tr("Shows the labels of each node"));
    nodeLabelAction->setCheckable(true);;

    nodeModuleAction = new QAction(tr("Show node modules"), this);
    nodeModuleAction->setShortcut(tr("Ctrl+5"));
    nodeModuleAction->setStatusTip(tr("Shows the modules of each node"));
    nodeModuleAction->setCheckable(true);

    nodeViewGroup = new QActionGroup(this);
    nodeViewGroup->addAction(nodeLabelAction);
    nodeViewGroup->addAction(nodeModuleAction);
    nodeLabelAction->setChecked(true);

    labelOnlyAction = new QAction(tr("Label only"), this);
    labelOnlyAction->setShortcut(tr("Ctrl+1"));
    labelOnlyAction->setStatusTip(tr("If an edge is labeled, show only the label color"));
    labelOnlyAction->setCheckable(true);

    signOnlyAction = new QAction(tr("Sign only"), this);
    signOnlyAction->setShortcut(tr("Ctrl+2"));
    signOnlyAction->setStatusTip(tr("Regardless of whether an edge is labeled, show only the color corresponding to its sign"));
    signOnlyAction->setCheckable(true);

    labelAndSignAction = new QAction(tr("Sign and label"), this);
    labelAndSignAction->setShortcut(tr("Ctrl+3"));
    labelAndSignAction->setStatusTip(tr("Show both the sign (color of the connection), and the label (color of the background) of the edge"));
    labelAndSignAction->setCheckable(true);

    curvedLineAction = new QAction(tr("Curved line"), this);;
    curvedLineAction->setShortcut(tr("Ctrl+6"));
    curvedLineAction->setStatusTip(tr("Show all lines as curved lines"));
    curvedLineAction->setCheckable(true);

    straightLineAction = new QAction(tr("Straight line"), this);;
    straightLineAction->setShortcut(tr("Ctrl+7"));
    straightLineAction->setStatusTip(tr("Show all lines as straight lines"));
    straightLineAction->setCheckable(true);

    lineModeGroup = new QActionGroup(this);;
    lineModeGroup->addAction(curvedLineAction);
    lineModeGroup->addAction(straightLineAction);
    curvedLineAction->setChecked(true);

    viewGroup = new QActionGroup(this);
    viewGroup->addAction(labelOnlyAction);
    viewGroup->addAction(signOnlyAction);
    viewGroup->addAction(labelAndSignAction);
    labelOnlyAction->setChecked(true);

    circleAction = new QAction(tr("&Layers"), this);
    circleAction->setShortcut(tr("Alt+I"));
    circleAction->setStatusTip(tr("Arrange nodes in layers"));

    ONPAction = new QAction(tr("&ONP"), this);
    ONPAction->setShortcut(tr("Alt+O"));
    ONPAction->setStatusTip(tr("Use the Optimal Neuron Placement to arrange nodes"));

    layerAction = new QAction(tr("&Circle"), this);
    layerAction->setShortcut(tr("Alt+P"));
    layerAction->setStatusTip(tr("Arrange nodes in a circle"));

    connect(addNodeviewAction, SIGNAL(triggered()), this, SLOT(addNodeView()));
    connect(labelOnlyAction, SIGNAL(triggered()), this, SLOT(setLabelView()));
    connect(signOnlyAction, SIGNAL(triggered()), this, SLOT(setSignView()));
    connect(labelAndSignAction, SIGNAL(triggered()), this, SLOT(setLabelAndSignView()));
    connect(circleAction, SIGNAL(triggered()), this, SLOT(positionNodesLayers()));
    connect(layerAction, SIGNAL(triggered()), this, SLOT(positionNodesCircle()));
    connect(ONPAction, SIGNAL(triggered()), this, SLOT(positionNodesONP()));

    connect(curvedLineAction, SIGNAL(triggered()), this, SLOT(setCurvedLines()));
    connect(straightLineAction, SIGNAL(triggered()), this, SLOT(setStraightLines()));

    connect(nodeLabelAction,SIGNAL(triggered()), this, SLOT(setNodeLabelView()));
    connect(nodeModuleAction,SIGNAL(triggered()), this, SLOT(setNodeModuleView()));

    addAction(straightLineAction);
    addAction(curvedLineAction);

    addAction(addNodeviewAction);
    addAction(labelOnlyAction);
    addAction(signOnlyAction);
    addAction(labelAndSignAction);
    addAction(circleAction);
    addAction(layerAction);
//    addAction(deleteAction);
    addAction(nodeLabelAction);
    addAction(nodeModuleAction);
    addAction(ONPAction);

    setNodeSelected(false);

    nodeMenu = new QMenu(tr("Node"), this);
//    nodeMenu->addAction(deleteAction);
    nodeMenu->addAction(addNodeviewAction);

    edgeMenu = new QMenu(tr("Edge"), this);
//    edgeMenu->addAction(deleteAction);
}

void CppnWidget::rebuildPhenotype(){
	cppn->updateNodes();
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

	connect(cppn, SIGNAL(newModularity(double)), this, SIGNAL(newModularity(double)));

	cppn->updateNodes();
}

void CppnWidget::itemMoved(Node * node)
{
	x_sorted.update(node);
	y_sorted.update(node);
	setSceneRect();
}

void CppnWidget::setSceneRect(){
	left_border = std::min(x_sorted.front()->pos().x() - extra_space, left_border_min);
	right_border = std::max(x_sorted.back()->pos().x() + extra_space, right_border_min);
	top_border = std::min(y_sorted.front()->pos().y() - extra_space, top_border_min);
	bottom_border = std::max(y_sorted.back()->pos().y() + extra_space, bottom_border_min);

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
		emit requestCommandExecution(new ComSetBookends(edge, bookendStart, edge->getBookendEnd(), edge->getStepsize()));
		edge->blockSignals(false);
	}
}

void CppnWidget::onBookendEndChanged(double bookendEnd){
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge){
		edge->blockSignals(true);
//		std::cout << "Bookend end: " << bookendEnd <<std::endl;
		emit requestCommandExecution(new ComSetBookends(edge, edge->getBookendStart(), bookendEnd, edge->getStepsize()));
		edge->blockSignals(false);
	}

}

void CppnWidget::onBookendStepChanged(double bookendStep){
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge){
		edge->blockSignals(true);
//		std::cout << "Bookend step: " << bookendStep <<std::endl;
		emit requestCommandExecution(new ComSetBookends(edge, edge->getBookendStart(), edge->getBookendEnd(), bookendStep));
		edge->blockSignals(false);
	}

}


void CppnWidget::resetWeight(){
	emit requestCommandExecution(new CommandSetWeight(scene()->selectedItems()));
}

void CppnWidget::resetAllWeights(){
	emit requestCommandExecution(new CommandSetWeight(scene()->items()));
}

void CppnWidget::weightUpdated(Edge* edge){
//	std::cout << "Update edge: " << edge->getBranch() << "_" << edge->getId() << std::endl;
	if(blockWeightUpdates) return;
	blockSignals(true);
	scene()->clearSelection();
	blockSignals(false);
	edge->setSelected(true);
}

void CppnWidget::positionNodesLayers(){
	cppn->positionNodes();
	emit requestCommandExecution(new CommandSetPos(scene()->items()));
}

void CppnWidget::positionNodesCircle(){
	cppn->positionNodesCircle();
	emit requestCommandExecution(new CommandSetPos(scene()->items()));
}

void CppnWidget::positionNodesONP(){
	cppn->positionNodesONP();
	emit requestCommandExecution(new CommandSetPos(scene()->items()));
}

void CppnWidget::addNode(Node* node){
	node->setPrevPos(node->pos());
	node->setContextMenu(nodeMenu);
	node->setLabelMode(&nodeLabelMode);

	addContextMenuObject(node);
	cppn->addNode(node);
	connect(node, SIGNAL(updatePositionsRequest()), this, SLOT(updatePreviousPositions()));
	connect(node, SIGNAL(positionChanged(Node*)), this, SLOT(itemMoved(Node*)));
	connect(node, SIGNAL(updateRequest(Node*)), cppn, SLOT(updateNode(Node*)));
	connect(node, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
	x_sorted.add(node);
	y_sorted.add(node);
	itemMoved(node);
}

void CppnWidget::removeNode(Node* node){
	node->setSelected(false);
	removeContextMenuObject(node);
	cppn->removeNode(node);
	disconnect(node, SIGNAL(updatePositionsRequest()), this, SLOT(updatePreviousPositions()));
	disconnect(node, SIGNAL(positionChanged(Node*)), this, SLOT(itemMoved(Node*)));
	disconnect(node, SIGNAL(updateRequest(Node*)), cppn, SLOT(updateNode(Node*)));
	disconnect(node, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
	x_sorted.remove(node);
	y_sorted.remove(node);
	setSceneRect();
}


void CppnWidget::addEdge(Edge* edge){
	edge->setLabelMode(&labelMode);
	edge->setLineMode(&lineMode);
	edge->setContextMenu(edgeMenu);

	addContextMenuObject(edge);
	cppn->addConnection(edge);
	connect(edge, SIGNAL(weightChanged(Edge*, double, bool)), cppn, SLOT(setWeight(Edge*, double, bool)));
	connect(edge, SIGNAL(weightChanged(Edge*, double, bool)), this, SLOT(weightUpdated(Edge*)));
	connect(edge, SIGNAL(requestUpdateAll()), cppn, SLOT(updateNodes()));
	connect(edge, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
	connect(edge, SIGNAL(bookendsChanged(Edge*)), this, SLOT(weightUpdated(Edge*)));
}


void CppnWidget::removeEdge(Edge* edge){
	edge->setSelected(false);
//	std::cout << "Removing edge: " << edge->getBranch() << "_" << edge->getId()  << " (" << edge << ")"<< std::endl;
	removeContextMenuObject(edge);
	cppn->removeConnection(edge);
	disconnect(edge, SIGNAL(weightChanged(Edge*, double, bool)), cppn, SLOT(setWeight(Edge*, double, bool)));
	disconnect(edge, SIGNAL(weightChanged(Edge*, double, bool)), this, SLOT(weightUpdated(Edge*)));
	disconnect(edge, SIGNAL(requestUpdateAll()), cppn, SLOT(updateNodes()));
	disconnect(edge, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
	disconnect(edge, SIGNAL(bookendsChanged(Edge*)), this, SLOT(weightUpdated(Edge*)));
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

	if(scene()->selectedItems().count() == 1){
		if(edgeSelected) emit edgeUpdated(qgraphicsitem_cast<Edge*>(scene()->selectedItems().front()));
		if(nodeSelected) emit nodeUpdated(qgraphicsitem_cast<Node*>(scene()->selectedItems().front()));
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

void CppnWidget::setCurvedLines(){
	lineMode = Edge::curved;
	update();
}

void CppnWidget::setStraightLines(){
	lineMode = Edge::straight;
	update();
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

void CppnWidget::setNodeLabelView(){
	nodeLabelMode = Node::labels;
	updateAll();
}

void CppnWidget::setNodeModuleView(){
	nodeLabelMode = Node::modules;
	updateAll();
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
}

void CppnWidget::setEdgeSelected(bool selected){
	edgeSelected = selected;
}

void CppnWidget::saveImage(){
	if(nodeSelected){
		if(scene()->selectedItems().count() > 1){
			QString newFileName = util::getSupportedFilename(this, tr("Chose save directory"));
			if(newFileName.isEmpty()) return;
			QString captureDirectory = util::getBase(newFileName);
			QString extention = util::getExtention(newFileName);
			QDir().mkdir(captureDirectory);

			foreach(QGraphicsItem* item, scene()->selectedItems()){
				Node* node = qgraphicsitem_cast<Node*>(item);
				if(node){
					QString newFileName = captureDirectory + "/node_" + util::toQString(node->getBranch()) + "_" + util::toQString(node->getId()) + extention;
					node->getImage()->save(newFileName);
				}
			}
		} else {
			Node* node = qgraphicsitem_cast<Node*>(scene()->selectedItems().front());
			if(node){
				QString newFileName = util::getSupportedFilename(this, tr("Save node image"));
				if(newFileName.isEmpty()) return;
				node->getImage()->save(newFileName);
			}
		}
	}

}

void CppnWidget::deleteObjects(){
	ComAddRemoveObject* command = new ComAddRemoveObject(this, scene()->selectedItems(), false);
	if(command->isOk()) emit requestCommandExecution(command);
}


void CppnWidget::deselectItems(){
	scene()->clearSelection();
}

void CppnWidget::flash(bool flashOn){
	if(scene()->selectedItems().count() != 1) return;
	Edge* edge = qgraphicsitem_cast<Edge*>(scene()->selectedItems().front());
	if(edge) edge->flash(flashOn);
}

void CppnWidget::mousePressEvent(QMouseEvent * event)
{
	if(firstClick && scene()->selectedItems().count() > 0){
		return;
	}
	QGraphicsView::mousePressEvent(event);
}

void CppnWidget::mouseReleaseEvent(QMouseEvent * event){
//	std::cout << "Mouse release" << std::endl;
	if(firstClick && scene()->selectedItems().count() > 0){
//		event->accept();
		firstClick = false;
		return;
	}
	ContextMenuGraphicsView::mouseReleaseEvent(event);
}

void CppnWidget::focusOutEvent ( QFocusEvent * event){
	firstClick = true;
	ContextMenuGraphicsView::focusOutEvent (event);
}
