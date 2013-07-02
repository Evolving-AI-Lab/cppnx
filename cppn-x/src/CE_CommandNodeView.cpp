/*
 * CE_CommandNodeView.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#include "CE_CommandNodeView.h"
#include <iostream>


bool CommandNodeView::nodeViewCompare(const nodeViewPair_t first, const nodeViewPair_t second){
	return first.second->getPosition() < second.second->getPosition();
}

CommandNodeView::CommandNodeView(QGraphicsView* sidebar, QList<QGraphicsItem*> items, bool add): sidebar(sidebar), add(add) {
	if(add){
//		std::cout << "add" <<std::endl;
		size_t index_mod = 0;
		foreach(QGraphicsItem* item, items){
			Node* node = qgraphicsitem_cast<Node*> (item);
			if(node){
				NodeView* nodeView = new NodeView();
				nodeView->setPosition(sidebar->scene()->items().count() + index_mod);
				nodeViewPairs.append(nodeViewPair_t(node, nodeView));
				index_mod++;
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

//		foreach(nodeViewPair_t pair, nodeViewPairs){
//			std::cout << pair.second->getPosition() <<std::endl;
//		}

		qSort(nodeViewPairs.begin(), nodeViewPairs.end(), nodeViewCompare);
		setText("remove from sidebar");

//		foreach(nodeViewPair_t pair, nodeViewPairs){
//			std::cout << pair.second->getPosition() <<std::endl;
//		}
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
	foreach(QGraphicsItem* item, sidebar->scene()->items()){
		NodeView* node = util::multiCast<NodeView*, FinalNodeView*> (item);
		if(node){
			node->setPos(NodeView::half_width, NodeView::half_height + node->getPosition()*(NodeView::node_height + betweenNodeMargin));
		}
	}
	sidebar->scene()->setSceneRect(0, 0, NodeView::node_width, sidebar->scene()->items().size()*(NodeView::node_height + betweenNodeMargin));
}

void CommandNodeView::addNodeViews(){
//	std::cout << "addNodeViews" <<std::endl;
	std::vector<size_t> index_mods(sidebar->scene()->items().count(), size_t(0));

	size_t mod_index = 0;
	int increment = 0;
	foreach(nodeViewPair_t pair, nodeViewPairs){
		while(mod_index < (pair.second->getPosition() - increment)  && mod_index < index_mods.size()){
			index_mods[mod_index] = increment;
			mod_index++;
		}
		increment++;
	}

	while(mod_index < index_mods.size() ){
		index_mods[mod_index] = increment;
		mod_index++;
	}

	foreach(QGraphicsItem* item, sidebar->scene()->items()){
		NodeView* node = util::multiCast<NodeView*, FinalNodeView*> (item);
		if(node){
			node->setPosition(node->getPosition() + index_mods[node->getPosition()]);
		}
	}

	foreach(nodeViewPair_t pair, nodeViewPairs){
		pair.first->setNodeView(pair.second);
		sidebar->scene()->addItem(pair.second);
		pair.first->redraw();
	}
	setNodeviewPositions();
}

void CommandNodeView::removeNodeViews(){
//	std::cout << "removeNodeViews" <<std::endl;

	std::vector<size_t> index_mods(sidebar->scene()->items().count(), size_t(0));

	size_t mod_index = 0;
	int decrement = 0;
	foreach(nodeViewPair_t pair, nodeViewPairs){
		while(mod_index < (pair.second->getPosition()) ){
			index_mods[mod_index] = decrement;
			mod_index++;
		}
		index_mods[mod_index] = 0;
		mod_index++;
		decrement++;
	}

	while(mod_index < index_mods.size() ){
		index_mods[mod_index] = decrement;
		mod_index++;
	}

	foreach(QGraphicsItem* item, sidebar->scene()->items()){
		NodeView* node = util::multiCast<NodeView*, FinalNodeView*> (item);
		if(node){
			node->setPosition(node->getPosition() - index_mods[node->getPosition()]);
		}
	}

	foreach(nodeViewPair_t pair, nodeViewPairs){
//		std::cout << "remove one" <<std::endl;
		sidebar->scene()->removeItem(pair.second);
		pair.first->resetNodeView(false);
	}
	setNodeviewPositions();
}
