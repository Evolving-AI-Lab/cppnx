/*
 * CE_NodeViewWidget.cpp
 *
 *  Created on: Jul 2, 2013
 *      Author: joost
 */

#include "CE_NodeViewWidget.h"
#include "CE_CommandNodeView.h"
#include "CE_Util.h"

NodeViewWidget::NodeViewWidget(QWidget* widget):
DragAndDropGraphicsView(widget){
	//Create scene
	scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene()->setSceneRect(0, 0, NodeView::node_width, NodeView::node_height);

	//Widget settings
	setMinimumSize(NodeView::node_width + sidebarMargin, NodeView::node_height + sidebarMargin);
	setAlignment(Qt::AlignTop);
    setDragMode(ScrollHandDrag);

    connect(scene()	, SIGNAL(selectionChanged()), this, SLOT(selectionUpdated()));

    nodeviewMenu = new QMenu(tr("nodeview"));
}

NodeViewWidget::~NodeViewWidget() {
	// TODO Auto-generated destructor stub
}




void NodeViewWidget::clearNodeViews(){
	clearObjects();
}



void NodeViewWidget::deleteNodeView(NodeView* nodeToDelete){
	removeDragAndDropObject(nodeToDelete);
}

void NodeViewWidget::insertNodeView(NodeView* nodeView){
	insertNodeView(nodeView, nodeView->getIndex());
}

void NodeViewWidget::insertNodeView(NodeView* nodeView, size_t index){
	insertDragAndDropObject(nodeView, index);
	nodeView->setContextMenu(nodeviewMenu);
}

void NodeViewWidget::addNodeView(NodeView* nodeView){
	insertNodeView(nodeView, objects.count());
}

void NodeViewWidget::addNodeView(QList<QGraphicsItem*> nodes){
	emit requestCommandExecution(new CommandNodeView(this, nodes, true));
}

void NodeViewWidget::deleteNodeView(){
	emit requestCommandExecution(new CommandNodeView(this, scene()->selectedItems(), false));
}

void NodeViewWidget::selectionUpdated(){
	bool nodeIsViewSelected = false;

	foreach(QGraphicsItem* item, scene()->selectedItems()){
		if(qgraphicsitem_cast<NodeView*>(item)) nodeIsViewSelected = true;
	}
}

void NodeViewWidget::saveImage(){
	if(scene()->selectedItems().count() > 1){
		QString newFileName = util::getSupportedFilename(this, tr("Chose save directory"));
		if(newFileName.isEmpty()) return;
		QString captureDirectory = util::getBase(newFileName);
		QString extention = util::getExtention(newFileName);
		QDir().mkdir(captureDirectory);

		foreach(QGraphicsItem* item, scene()->selectedItems()){
			NodeView* nodeview = util::multiCast<NodeView*, FinalNodeView*>(item);
			if(nodeview){
				QString newFileName;
				if(nodeview->getNode()){
					newFileName = captureDirectory + "/node_" + util::toQString(nodeview->getNode()->getBranch()) + "_" + util::toQString(nodeview->getNode()->getId()) + extention;
				} else {
					newFileName = captureDirectory + "/node_final" + extention;
				}
				nodeview->getImage()->save(newFileName);
			}
		}
	} else if(scene()->selectedItems().count() == 1) {
		NodeView* nodeview = util::multiCast<NodeView*, FinalNodeView*>(scene()->selectedItems().front());
		if(nodeview){
			QString fileNames = util::getSupportedExtentions();
			QString newFileName = QFileDialog::getSaveFileName(this, tr("Save Node Image"), "",fileNames);
			if(newFileName.isEmpty()) return;
			nodeview->getImage()->save(newFileName);
		}
	}
}
