/*
 * CX_ContextMenuGraphicsView.h
 *
 *  Created on: Jul 10, 2013
 *      Author: joost
 */

#ifndef CX_CONTEXTMENUGRAPHICSVIEW_H_
#define CX_CONTEXTMENUGRAPHICSVIEW_H_

#include <QGraphicsView>
#include <QAction>

#include "CX_ComBase.h"
#include "CX_SelectableObject.h"
#include "CX_Debug.hpp"

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

	void clear(){
	    foreach(SelectableObject* object, _selectableObjects){
	        removeContextMenuObject(object);
	    }
	    scene()->clear();
	}

	QAction* createAction(QString name, QString statusBarTip, const char *member);

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
	/**
	 * Adds an object to the graphics view and connects it to the local context menu.
	 *
	 * @param object The object to be added to the graphics view.
	 */
	void addContextMenuObject(SelectableObject* object){
	    dbg::out(dbg::info, "menu") << "Adding object: " << object << " to scene: " << scene() << std::endl;
		connect(object, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
		object->setHasFocus(&thisHasFocus);
		_selectableObjects.push_back(object);
		scene()->addItem(object);
	}

	void removeContextMenuObject(SelectableObject* object){
		disconnect(object, SIGNAL(contextMenuEvent(SelectableObject*, bool)), this, SLOT(ContextMenuEvent(SelectableObject*, bool)));
		_selectableObjects.removeAll(object);
		scene()->removeItem(object);
	}

	void focusInEvent(QFocusEvent * event){
		thisHasFocus = true;
		update();
		updateAll();
		emit focusChanged();
		QGraphicsView::focusInEvent(event);
	}

	void focusOutEvent(QFocusEvent * event){
		thisHasFocus = false;
		update();
		updateAll();
		emit focusChanged();
		QGraphicsView::focusOutEvent(event);
	}

	QList<SelectableObject*> _selectableObjects;
};

#endif /* CX_CONTEXTMENUGRAPHICSVIEW_H_ */
