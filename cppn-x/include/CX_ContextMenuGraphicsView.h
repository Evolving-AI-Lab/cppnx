/*
 * CX_ContextMenuGraphicsView.h
 *
 *  Created on: Jul 10, 2013
 *      Author: joost
 */

#ifndef CX_CONTEXTMENUGRAPHICSVIEW_H_
#define CX_CONTEXTMENUGRAPHICSVIEW_H_

#include <QGraphicsView>
#include "CX_ComBase.h"
#include "CX_SelectableObject.h"

class ContextMenuGraphicsView: public QGraphicsView {
	Q_OBJECT
public:
	ContextMenuGraphicsView(QWidget* parent = 0):QGraphicsView(parent){
		QGraphicsScene *scene = new QGraphicsScene(this);
		setScene(scene);
		thisHasFocus = false;
		partOfContextMenuEvent = false;
	}

	virtual ~ContextMenuGraphicsView(){
		//nix
	}

	bool getIsActive(){
		return this->hasFocus() || partOfContextMenuEvent;
	}

	bool thisHasFocus;
	bool partOfContextMenuEvent;

public slots:
	void ContextMenuEvent(SelectableObject* object, bool begin);
	void updateAll();

signals:
	void requestCommandExecution(ComBase*);
	void sceneModified();
	void focusChanged();

protected:
	void addContextMenuObject(SelectableObject* object){
		connect(object, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
		object->setHasFocus(&thisHasFocus);
		scene()->addItem(object);
	}

	void removeContextMenuObject(SelectableObject* object){
		disconnect(object, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
		scene()->removeItem(object);
	}

	void focusInEvent(QFocusEvent * event){
		thisHasFocus= true;
		update();
		updateAll();
		emit focusChanged();
		QGraphicsView::focusInEvent(event);
	}

	void focusOutEvent(QFocusEvent * event){
		thisHasFocus= false;
		update();
		updateAll();
		emit focusChanged();
		QGraphicsView::focusOutEvent(event);
	}
};

#endif /* CX_CONTEXTMENUGRAPHICSVIEW_H_ */
