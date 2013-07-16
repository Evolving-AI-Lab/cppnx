/*
 * CX_ContextMenuGraphicsView.h
 *
 *  Created on: Jul 10, 2013
 *      Author: joost
 */

#ifndef CX_CONTEXTMENUGRAPHICSVIEW_H_
#define CX_CONTEXTMENUGRAPHICSVIEW_H_

#include <QGraphicsView>
#include <QUndoCommand>
#include "CX_SelectableObject.h"

class ContextMenuGraphicsView: public QGraphicsView {
	Q_OBJECT
public:
	ContextMenuGraphicsView(QWidget* parent = 0):QGraphicsView(parent){
		QGraphicsScene *scene = new QGraphicsScene(this);
		setScene(scene);
	}

	virtual ~ContextMenuGraphicsView(){
		//nix
	}

public slots:
	void ContextMenuEvent(SelectableObject* object, bool begin);

signals:
	void requestCommandExecution(QUndoCommand*);
	void sceneModified();

protected:
	void addContextMenuObject(SelectableObject* object){
		connect(object, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
		scene()->addItem(object);
	}

	void removeContextMenuObject(SelectableObject* object){
		disconnect(object, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
		scene()->removeItem(object);
	}
};

#endif /* CX_CONTEXTMENUGRAPHICSVIEW_H_ */
