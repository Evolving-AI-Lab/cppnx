/*
 * CX_ComAddRemoveObject.h
 *
 *  Created on: Jul 26, 2013
 *      Author: joost
 */

#ifndef CX_COMADDREMOVEOBJECT_H_
#define CX_COMADDREMOVEOBJECT_H_

#include <QGraphicsItem>
#include <QList>

#include "CX_ComBase.h"

class CppnWidget;
class Node;
class Edge;

class ComAddRemoveObject: public ComBase {
public:
	ComAddRemoveObject(CppnWidget* widget, QList<QGraphicsItem*> objects, bool add);
	virtual ~ComAddRemoveObject(){
		//nix
	}

	void undo();
	void redo();

	bool isOk(){
		return ok;
	}

private:
	void addObjects();
	void removeObjects();

	CppnWidget* widget;
	QList<Node*> nodes;
	QList<Edge*> edges;
	bool add;
	bool ok;
};

#endif /* CX_COMADDREMOVEOBJECT_H_ */
