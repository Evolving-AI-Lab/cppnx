/*
 * CX_ComChangeActivation.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: Joost Huizinga
 */

//Qt includes
#include <QList>

//Local includes
#include "CX_ComChangeActivation.hpp"
#include "CE_Util.h"
#include "CE_Cppn.h"

void CommandChangeActivation::init(CppnWidget* widget, QList<Node*> nodes, std::string activationFunction){
    _cppnWidget = widget;
    _activationFunction = activationFunction;
    _nodes = nodes;

    foreach(Node* node, _nodes){
        if(node){
            _previousActivationFunctions.append(node->getXmlActivationFunction());
        }
    }

    ok = (nodes.count() > 0) && widget;
    setText("set activation function: " + util::toQString(activationFunction));
}


void CommandChangeActivation::undo(){
    for(int i=0; i<_nodes.size(); ++i){
        Node* node = _nodes[i];
        if(node){
            _cppnWidget->getCppn()->setActivationFunction(node, _previousActivationFunctions[i]);
            node->update();
        }
    }
    _cppnWidget->rebuildPhenotype();
}

void CommandChangeActivation::redo(){
    foreach(Node* node, _nodes){
        if(node){
            _cppnWidget->getCppn()->setActivationFunction(node, _activationFunction);
            node->update();
        }
    }
    _cppnWidget->rebuildPhenotype();
}
