/*
 * CX_CXUndoCommand.h
 *
 *  Created on: Jul 12, 2013
 *      Author: joost
 */

#ifndef CX_CXUNDOCOMMAND_H_
#define CX_CXUNDOCOMMAND_H_

#include <QUndoCommand>

class CXUndoCommand: public QUndoCommand {
public:
	virtual bool modifiesFile(){
		return true;
	}
};

#endif /* CX_CXUNDOCOMMAND_H_ */
