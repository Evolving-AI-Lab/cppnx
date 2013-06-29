/*
 * CE_CommandNodeView.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#include "CE_CommandNodeView.h"
#include <iostream>

CommandNodeView::CommandNodeView(QGraphicsView* sidebar, QList<QGraphicsItem*> items, bool add): sidebar(sidebar), add(add) {
	if(add){
//		std::cout << "add" <<std::endl;
		foreach(QGraphicsItem* item, items){
			Node* node = qgraphicsitem_cast<Node*> (item);
			if(node){
				nodeViewPairs.append(nodeViewPair_t(node, new NodeView()));
			}
		}
		setText("add to sidebar");
	} else {
//		std::cout << "remove" <<std::endl;
		foreach(QGraphicsItem* item, items){
			NodeView* nodeView = qgraphicsitem_cast<NodeView*> (item);
			if(nodeView){
				nodeViewPairs.append(nodeViewPair_t(nodeView->getNode(), nodeView));
			}
		}
		setText("remove from sidebar");
	}

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

void CommandNodeView::setNodeviewPositions(){
	size_t index = 0;
	foreach(QGraphicsItem* item, sidebar->scene()->items()){
		NodeView* node = util::multiCast<NodeView*, FinalNodeView*> (item);
		if(node){
			node->setPos(NodeView::half_width, NodeView::half_height + index*(NodeView::node_height + betweenNodeMargin));
			index++;
		}
	}
	sidebar->scene()->setSceneRect(0, 0, NodeView::node_width, sidebar->scene()->items().size()*(NodeView::node_height + betweenNodeMargin));
}

void CommandNodeView::addNodeViews(){
//	std::cout << "addNodeViews" <<std::endl;
	foreach(nodeViewPair_t pair, nodeViewPairs){
		pair.first->setNodeView(pair.second);
		sidebar->scene()->addItem(pair.second);
		pair.first->redraw();
	}
	setNodeviewPositions();
}

void CommandNodeView::removeNodeViews(){
//	std::cout << "removeNodeViews" <<std::endl;
	foreach(nodeViewPair_t pair, nodeViewPairs){
		std::cout << "remove one" <<std::endl;
		sidebar->scene()->removeItem(pair.second);
		pair.first->resetNodeView(false);
	}
	setNodeviewPositions();
}
