/*
 * CX_ComAddRemoveObject.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: joost
 */

#include "CX_ComAddRemoveObject.h"
#include "CE_Defines.h"

#include "CE_CppnWidget.h"
#include "CE_Node.h"
#include "CE_Edge.h"

ComAddRemoveObject::ComAddRemoveObject(CppnWidget* widget, QList<QGraphicsItem*> items, bool add):widget(widget), add(add) {
	foreach(QGraphicsItem* item, items){
		Node* node =  qgraphicsitem_cast<Node*>(item);
		if(node){
			if(node->getType() != XML_TYPE_INPUT && node->getType() != XML_TYPE_OUTPUT){
				foreach(Edge* edge, node->incomingEdges()){
					if(edges.count(edge) == 0) edges.append(edge);
				}

				foreach(Edge* edge, node->outgoingEdges()){
					if(edges.count(edge) == 0) edges.append(edge);
				}

				if(nodes.count(node) == 0) nodes.append(node);
			}
		} else {
			Edge* edge =  qgraphicsitem_cast<Edge*>(item);
			if(edge){
				if(edges.count(edge) == 0) edges.append(edge);
			}
		}
	}

	ok = (nodes.count() + edges.count()) > 0;

	if(add){
		setText("add object");
	}else{
		setText("remove object");
	}
}


void ComAddRemoveObject::undo(){
//	std::cout << "undo: " << add <<std::endl;
	if(add){
		removeObjects();
	}else {
		addObjects();
	}
}

void ComAddRemoveObject::redo(){
//	std::cout << "redo: " << add <<std::endl;
	if(add){
		addObjects();
	}else {
		removeObjects();
	}
}


void ComAddRemoveObject::addObjects(){
	foreach(Node* node, nodes){
		widget->addNode(node);
		node->emitAdded();
	}

	foreach(Edge* edge, edges){
		widget->addEdge(edge);
	}

	widget->rebuildPhenotype();
}


void ComAddRemoveObject::removeObjects(){
	foreach(Edge* edge, edges){
		widget->removeEdge(edge);
	}

	foreach(Node* node, nodes){
		widget->removeNode(node);
		node->emitRemoved();
	}

	widget->rebuildPhenotype();
}
