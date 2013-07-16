/*
 * CE_CommandNodeView.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#include "CE_CommandNodeView.h"

#include <iostream>

CommandNodeView::CommandNodeView(NodeViewWidget* nodeViewWidget, QList<QGraphicsItem*> items, bool add): nodeViewWidget(nodeViewWidget), add(add) {
	if(add){
//		std::cout << "add" <<std::endl;
		size_t index_mod = 0;
		foreach(QGraphicsItem* item, items){
			Node* node = qgraphicsitem_cast<Node*> (item);
//			if(node && !node->getNodeView()){
			NodeView* nodeView = new NodeView(node);
			Node::connect(node, SIGNAL(imageChanged()), nodeView, SLOT(update()));
			nodeView->setIndex(nodeViewWidget->getNrOfItems() + index_mod);
			nodeViewPairs.append(nodeView);
			index_mod++;
//			}
		}
		setText("add to sidebar");
	} else {
//		std::cout << "remove" <<std::endl;
		foreach(QGraphicsItem* item, items){
			NodeView* nodeView = qgraphicsitem_cast<NodeView*> (item);
			if(nodeView){
				nodeViewPairs.append(nodeView);
			}
		}

		setText("remove from sidebar");
	}

	ok = (nodeViewPairs.count() > 0);
}

CommandNodeView::~CommandNodeView() {
	// TODO Auto-generated destructor stub
}

void CommandNodeView::undo(){
	if(add){
		removeNodeViews();
	}else{
		addNodeViews();
	}
}

void CommandNodeView::redo(){
//	std::cout << "redo" <<std::endl;
	if(add){
		addNodeViews();
	}else{
		removeNodeViews();
	}
}
//
//void CommandNodeView::setNodeviewPositions(){
//	foreach(QGraphicsItem* item, nodeViewWidget->scene()->items()){
//		NodeView* node = util::multiCast<NodeView*, FinalNodeView*> (item);
//		if(node){
//			node->setPos(NodeView::half_width, NodeView::half_height + node->getPosition()*(NodeView::node_height + betweenNodeMargin));
//		}
//	}
//	nodeViewWidget->scene()->setSceneRect(0, 0, NodeView::node_width, nodeViewWidget->scene()->items().size()*(NodeView::node_height + betweenNodeMargin));
//}

void CommandNodeView::addNodeViews(){
	foreach(NodeView* nodeView, nodeViewPairs){
		nodeViewWidget->insertNodeView(nodeView, nodeView->getIndex());
	}

//	foreach(nodeViewPair_t pair, nodeViewPairs){
//		pair.first->setNodeView(pair.second);
//		nodeViewWidget->insertNodeView(pair.second, pair.second->getPosition());
//		pair.first->redraw();
//	}

}

void CommandNodeView::removeNodeViews(){
	foreach(NodeView* nodeView, nodeViewPairs){
		nodeViewWidget->deleteNodeView(nodeView);
	}

//	for(int i = nodeViewPairs.count()-1; i>=0; i--){
//		nodeViewPair_t pair = nodeViewPairs[i];
//		nodeViewWidget->deleteNodeView(pair.second);
//		pair.first->resetNodeView(false);
//	}
}
