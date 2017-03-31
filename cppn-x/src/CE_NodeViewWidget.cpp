/*
 * CE_NodeViewWidget.cpp
 *
 *  Created on: Jul 2, 2013
 *      Author: joost
 */

#include <CX_ComNodeView.h>
#include <QtGlobal>

#include "CE_NodeViewWidget.h"
#include "CE_Util.h"
#include "CX_Debug.hpp"

NodeViewWidget::NodeViewWidget(QWidget* widget): DragAndDropGraphicsView(widget){
    dbg::trace trace("nodeviewwidget", DBG_HERE);

	//Create scene
	scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene()->setSceneRect(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);

	//Widget settings
	setMinimumSize(IMAGE_WIDTH + sidebarMargin, IMAGE_HEIGHT + sidebarMargin);
	setAlignment(Qt::AlignTop);
    setDragMode(ScrollHandDrag);

    connect(scene()	, SIGNAL(selectionChanged()), this, SLOT(selectionUpdated()));

    nodeviewMenu = new QMenu(tr("nodeview"));
    _finalNodeView = 0;
}

NodeViewWidget::~NodeViewWidget() {
    dbg::trace trace("nodeviewwidget", DBG_HERE);
}

void NodeViewWidget::setNodeviews(QList<NodeView*> nodeviews){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
    clearNodeViews();
    foreach(NodeView* nodeview, nodeviews){
        addNodeView(nodeview);
    }
}

QList<NodeView*> NodeViewWidget::getNodeviews(){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
    QList<NodeView*> result;
    foreach(QGraphicsItem* item, objects){
        NodeView* nodeview = util::multiCast<NodeView*, FinalNodeView*>(item);
        if(nodeview) result.append(nodeview);
    }

    return result;
}



void NodeViewWidget::clearNodeViews(){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
	clearObjects();
	_finalNodeView = 0;
}



void NodeViewWidget::deleteNodeView(NodeView* nodeToDelete){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
	removeDragAndDropObject(nodeToDelete);
	if(nodeToDelete == _finalNodeView){
	    _finalNodeView = 0;
	}
}

void NodeViewWidget::insertNodeView(NodeView* nodeView){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
	insertNodeView(nodeView, nodeView->getIndex());
}

void NodeViewWidget::insertNodeView(NodeView* nodeView, size_t index){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
	insertDragAndDropObject(nodeView, index);
	nodeView->setContextMenu(nodeviewMenu);
	FinalNodeView* finalNodeView = qgraphicsitem_cast<FinalNodeView*>(nodeView);
	if(finalNodeView){
	    _finalNodeView = finalNodeView;
	}
}

void NodeViewWidget::addNodeView(NodeView* nodeView){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
	insertNodeView(nodeView, objects.count());
}

void NodeViewWidget::addNodeView(QList<QGraphicsItem*> nodes){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
	emit requestCommandExecution(new CommandNodeView(this, nodes, true));
}

void NodeViewWidget::deleteNodeView(){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
	emit requestCommandExecution(new CommandNodeView(this, scene()->selectedItems(), false));
}

bool NodeViewWidget::deletableNodeviewSelected(){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
    foreach(QGraphicsItem* item, scene()->selectedItems()){
        NodeView* nodeview = qgraphicsitem_cast<NodeView*>(item);
        if(nodeview) return true;
    }
    return false;
}

void NodeViewWidget::selectionUpdated(){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
	bool nodeIsViewSelected = false;

	foreach(QGraphicsItem* item, scene()->selectedItems()){
		if(qgraphicsitem_cast<NodeView*>(item)) nodeIsViewSelected = true;
	}
}

void NodeViewWidget::saveImage(){
    dbg::trace trace("nodeviewwidget", DBG_HERE);
	std::vector<NodeView*> nodes;
	foreach(QGraphicsItem* item, scene()->selectedItems()){
		NodeView* nodeview = util::multiCast<NodeView*, FinalNodeView*>(item);
		if(nodeview){
			nodes.push_back(nodeview);
		}
	}
	util::saveBatch(this, nodes);
}
