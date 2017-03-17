/*
 * CX_ComChangeActivation.hpp
 *
 *  Created on: Apr 17, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_COMCHANGEACTIVATION_HPP_
#define CPPN_X_INCLUDE_CX_COMCHANGEACTIVATION_HPP_

//Qt includes
#include <QList>

//Local includes
#include "CX_ComBase.h"
#include "CE_CppnWidget.h"

class CppnWidget;

class CommandChangeActivation: public ComBase {
public:
    CommandChangeActivation(QUndoCommand * parent = 0):
        ComBase(parent), _cppnWidget(0), ok(false){}

    void init(CppnWidget* widget, QList<Node*> nodes, std::string activationFunction);
    void undo();
    void redo();
    bool isOk() const{return ok;}

private:
    CppnWidget* _cppnWidget;
    QList<Node*> _nodes;
    std::string _activationFunction;
    QList<std::string> _previousActivationFunctions;
    bool ok;
};



#endif /* CPPN_X_INCLUDE_CX_COMCHANGEACTIVATION_HPP_ */
