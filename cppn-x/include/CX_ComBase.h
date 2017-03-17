/*
 * CX_ComBase.h
 *
 *  Created on: Jul 12, 2013
 *      Author: joost
 */

#ifndef CX_COMBASE_H_
#define CX_COMBASE_H_

#include <QUndoCommand>
#include <iostream>

class ComBase: public QUndoCommand {
public:
    ComBase(QUndoCommand * parent = 0): QUndoCommand(parent){}

	virtual bool isOk() const{return true;}
};

#endif /* CX_COMBASE_H_ */
