/*
 * CE_CommandLabelObject.h
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDLABELOBJECT_H_
#define CE_COMMANDLABELOBJECT_H_

#include <qundostack.h>

class CommandLabelObject: public QUndoCommand {
public:
	CommandLabelObject();
	virtual ~CommandLabelObject();
};

#endif /* CE_COMMANDLABELOBJECT_H_ */
