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
#include "CX_Debug.hpp"

class CppnWidget;
class Node;
class Edge;

class ComAddRemoveObject: public ComBase {
public:
    enum AddOrRemove {addEnum, removeEnum};
//    typedef QPair<QGraphicsItem*, AddOrRemove> pair_t;

    ComAddRemoveObject(QUndoCommand * parent = 0):ComBase(parent), _widget(0), ok(false){}
	ComAddRemoveObject(CppnWidget* widget, QList<QGraphicsItem*> objects, bool add);
//	virtual ~ComAddRemoveObject(){
//		//nix
//	}

	void init(CppnWidget* widget);
	void addObject(QGraphicsItem* object);
	void removeObject(QGraphicsItem* object);

	void undo();
	void redo();

	bool isOk() const{return ok;}

private:
//	void addObjects();
//	void removeObjects();

	void _addObject(QGraphicsItem* item);
	void _removeObject(QGraphicsItem* item);

	CppnWidget* _widget;
	QList<QGraphicsItem*> _objects;
	QMap<QGraphicsItem*, AddOrRemove> _addORemoveMap;
//	QList<QPair<Node*, AddOrRemove> > nodes;
//	QList<QPair<Edge*, AddOrRemove> > edges;
//	QList<Edge*> edges;
//	bool add;
	bool ok;
};

#endif /* CX_COMADDREMOVEOBJECT_H_ */
