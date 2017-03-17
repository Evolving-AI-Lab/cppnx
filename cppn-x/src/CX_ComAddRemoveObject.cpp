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

ComAddRemoveObject::ComAddRemoveObject(CppnWidget* widget, QList<QGraphicsItem*> items, bool add):_widget(widget) {
	foreach(QGraphicsItem* item, items){
	    if(add){
	        addObject(item);
	    } else {
	        removeObject(item);
	    }
//		Node* node =  qgraphicsitem_cast<Node*>(item);
//		if(node){
//			if(node->getType() != XML_TYPE_INPUT && node->getType() != XML_TYPE_OUTPUT){
//				foreach(Edge* edge, node->incomingEdges()){
//					if(edges.count(edge) == 0) edges.append(edge);
//				}
//
//				foreach(Edge* edge, node->outgoingEdges()){
//					if(edges.count(edge) == 0) edges.append(edge);
//				}
//
//				if(nodes.count(node) == 0) nodes.append(node);
//			}
//		} else {
//			Edge* edge =  qgraphicsitem_cast<Edge*>(item);
//			if(edge){
//				if(edges.count(edge) == 0) edges.append(edge);
//			}
//		}
	}

//	ok = (_objects.count()) > 0;

	if(add){
		setText("add object");
	}else{
		setText("remove object");
	}
}

void ComAddRemoveObject::init(CppnWidget* widget){
    _widget = widget;
}

void ComAddRemoveObject::addObject(QGraphicsItem* object){
    if(_addORemoveMap.count(object) > 0) return;
    _objects.append(object);
    _addORemoveMap[object] = addEnum;
    ok = (_objects.count()) > 0;

//    Node* node =  qgraphicsitem_cast<Node*>(object);
//    if(node){
//        if(node->getType() != XML_TYPE_INPUT && node->getType() != XML_TYPE_OUTPUT){
//            objects.append(pair_t(object, addEnum));
////            foreach(Edge* edge, node->incomingEdges()){
////                if(edges.count(edge) == 0) edges.append(edge);
////            }
////
////            foreach(Edge* edge, node->outgoingEdges()){
////                if(edges.count(edge) == 0) edges.append(edge);
////            }
////
////            if(nodes.count(node) == 0) nodes.append(node);
//        }
//    }
//
//    Edge* edge =  qgraphicsitem_cast<Edge*>(object);
//    if(edge) {
//        if(edges.count(edge) == 0) edges.append(edge);
//    }
}

void ComAddRemoveObject::removeObject(QGraphicsItem* object){
    if(_addORemoveMap.count(object) > 0) return;
    Node* node =  qgraphicsitem_cast<Node*>(object);
    if(node){
        if(node->getType() != XML_TYPE_INPUT && node->getType() != XML_TYPE_OUTPUT){
            foreach(Edge* edge, node->incomingEdges()){
                removeObject(edge);
//                _objects.append(edge, removeEnum);
//                _addORemoveMap[edge] = removeEnum;
//                if(edges.count(edge) == 0) edges.append(edge);
            }

            foreach(Edge* edge, node->outgoingEdges()){
                removeObject(edge);
//                objects.append(pair_t(edge, removeEnum));
//                if(edges.count(edge) == 0) edges.append(edge);
            }

            _objects.append(object);
            _addORemoveMap[object] = removeEnum;

//
//            if(nodes.count(node) == 0) nodes.append(node);
        }
    } else {
        _objects.append(object);
        _addORemoveMap[object] = removeEnum;
    }
    ok = (_objects.count()) > 0;
//
//    Edge* edge =  qgraphicsitem_cast<Edge*>(object);
//    if(edge) {
//        if(edges.count(edge) == 0) edges.append(edge);
//    }
}

void ComAddRemoveObject::undo(){
//	std::cout << "undo: " << add <<std::endl;
//	if(add){
//		removeObjects();
//	}else {
//		addObjects();
//	}
    QListIterator<QGraphicsItem*> iterator(_objects);
    iterator.toBack();
    while(iterator.hasPrevious()){
        QGraphicsItem* item = iterator.previous();
        if(_addORemoveMap[item] == addEnum){
            _removeObject(item);
        } else if(_addORemoveMap[item] == removeEnum){
            _addObject(item);
        } else {
            dbg::sentinel(DBG_HERE);
        }
    }
    _widget->rebuildPhenotype();
}

void ComAddRemoveObject::redo(){
//	std::cout << "redo: " << add <<std::endl;
//	if(add){
//		addObjects();
//	}else {
//		removeObjects();
//	}
    QListIterator<QGraphicsItem*> iterator(_objects);
    while(iterator.hasNext()){
        QGraphicsItem* item = iterator.next();
        if(_addORemoveMap[item] == addEnum){
            _addObject(item);
        } else if(_addORemoveMap[item]== removeEnum){
            _removeObject(item);
        } else {
            dbg::sentinel(DBG_HERE);
        }
    }
    _widget->rebuildPhenotype();
}

void ComAddRemoveObject::_addObject(QGraphicsItem* item){
    Node* node =  qgraphicsitem_cast<Node*>(item);
    if(node){
        _widget->addNode(node);
        node->emitAdded();
    }

    Edge* edge =  qgraphicsitem_cast<Edge*>(item);
    if(edge) {
        _widget->addEdge(edge);
    }
}

void ComAddRemoveObject::_removeObject(QGraphicsItem* item){
    Node* node =  qgraphicsitem_cast<Node*>(item);
    if(node){
        _widget->removeNode(node);
        node->emitRemoved();
    }

    Edge* edge =  qgraphicsitem_cast<Edge*>(item);
    if(edge) {
        _widget->removeEdge(edge);
    }
}


//void ComAddRemoveObject::addObjects(){
//	foreach(Node* node, nodes){
//		_widget->addNode(node);
//		node->emitAdded();
//	}
//
//	foreach(Edge* edge, edges){
//		_widget->addEdge(edge);
//	}
//
//	_widget->rebuildPhenotype();
//}
//
//
//void ComAddRemoveObject::removeObjects(){
//	foreach(Edge* edge, edges){
//		_widget->removeEdge(edge);
//	}
//
//	foreach(Node* node, nodes){
//		_widget->removeNode(node);
//		node->emitRemoved();
//	}
//
//	_widget->rebuildPhenotype();
//}
