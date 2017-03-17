/*
 * CE_CommandNodeView.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

//Standard includes
#include <iostream>

//Local includes
#include "CX_ComNodeView.h"
#include "CX_Debug.hpp"

CommandNodeView::CommandNodeView(NodeViewWidget* nodeViewWidget, QList<QGraphicsItem*> items, bool add):
    add(add)
{
    dbg::trace trace("commandnodeview", DBG_HERE);
    _nodeViewWidget = nodeViewWidget;

	if(add){
		size_t index_mod = 0;
		foreach(QGraphicsItem* item, items){
			Node* node = qgraphicsitem_cast<Node*> (item);
			NodeView* nodeView = new NodeView(node);
			nodeView->setIndex(nodeViewWidget->getNrOfItems() + index_mod);
			nodeViewPairs.append(nodeView);
			index_mod++;
		}
		setText("add to sidebar");
	} else {
		foreach(QGraphicsItem* item, items){
			NodeView* nodeView = qgraphicsitem_cast<NodeView*> (item);
			if(nodeView){
				nodeViewPairs.append(nodeView);
			}
		}

		setText("remove from sidebar");
	}

    dbg::out(dbg::info, "commandnodeview") << "Number of node view pairs: " << nodeViewPairs.count() << std::endl;
	ok = (nodeViewPairs.count() > 0);
}

CommandNodeView::~CommandNodeView() {
    dbg::trace trace("commandnodeview", DBG_HERE);
}

void CommandNodeView::undo(){
    dbg::trace trace("commandnodeview", DBG_HERE);
	if(add){
		removeNodeViews();
	}else{
		addNodeViews();
	}
}

void CommandNodeView::redo(){
    dbg::trace trace("commandnodeview", DBG_HERE);
	if(add){
		addNodeViews();
	}else{
		removeNodeViews();
	}
}

void CommandNodeView::addNodeViews(){
    dbg::trace trace("commandnodeview", DBG_HERE);
	foreach(NodeView* nodeView, nodeViewPairs){
	    _nodeViewWidget->insertNodeView(nodeView);
	}
}

void CommandNodeView::removeNodeViews(){
    dbg::trace trace("commandnodeview", DBG_HERE);
	foreach(NodeView* nodeView, nodeViewPairs){
	    _nodeViewWidget->deleteNodeView(nodeView);
	}
}
