/*
 * CX_DragAndDropGraphicsView.h
 *
 *  Created on: Jul 10, 2013
 *      Author: joost
 */

#ifndef CX_DRAGANDDROPGRAPHICSVIEW_H_
#define CX_DRAGANDDROPGRAPHICSVIEW_H_

#include "CX_ContextMenuGraphicsView.h"
#include "CX_DragAndDropObject.h"
#include "CX_ComDragAndDrop.h"
#include <iostream>

class DragAndDropGraphicsView: public ContextMenuGraphicsView {
	Q_OBJECT
public:
	DragAndDropGraphicsView(QWidget* parent = 0): ContextMenuGraphicsView(parent){
		contentsMargin = 20.0;
		topMargin = 0;
		bottomMargin = 0;
		leftMargin = 0;
		rightMargin = 0;
	}

	virtual ~DragAndDropGraphicsView(){
		//nix
	}

	void move(uint source, uint target){
		objects.move(source, target);
		setPositions();
		emit objectsMoved();
	}

	void addDragAndDropObject(DragAndDropObject* object){
		insertDragAndDropObject(object, objects.count());
	}

	void insertDragAndDropObject(DragAndDropObject* object, size_t index){
		connect(object, SIGNAL(requestMove(uint, uint)), this, SLOT(requestMove(uint, uint)));
		objects.insert(index, object);
		addContextMenuObject(object);
	    setPositions();
	    setSceneRect();
	}

	void removeDragAndDropObject(DragAndDropObject* object){
		disconnect(object, SIGNAL(requestMove(uint, uint)), this, SLOT(requestMove(uint, uint)));
		objects.removeAll(object);
		removeContextMenuObject(object);
	    setPositions();
	    setSceneRect();
	}

	void setContentsMargin(qreal margin){
		contentsMargin = margin;
	}

	void setTopMargin(qreal margin){
		topMargin = margin;
	}

	void setBottomMargin(qreal margin){
		bottomMargin = margin;
	}

	void setLeftMargin(qreal margin){
		leftMargin = margin;
	}

	void setRightMargin(qreal margin){
		leftMargin = margin;
	}

	DragAndDropObject* getItem(int i){
		return objects[i];
	}

	int getNrOfItems(){
		return objects.count();
	}

	void clearObjects(){
		foreach(DragAndDropObject* object, objects){
			removeDragAndDropObject(object);
		}
	}

signals:
	void objectsMoved();

public slots:
	void requestMove(uint source, uint target){
		ComDragAndDrop* command= new ComDragAndDrop(this, source, target);
		if(command->isOk()){
			emit requestCommandExecution(new ComDragAndDrop(this, source, target));
		} else {
			delete command;
		}

	}

protected:
	qreal contentsMargin;
	qreal topMargin;
	qreal bottomMargin;
	qreal leftMargin;
	qreal rightMargin;



	void setSceneRect(){
		qreal height = topMargin + bottomMargin;
		qreal width = leftMargin + rightMargin;
		foreach(DragAndDropObject* object, objects){
			height += object->boundingRect().height() + contentsMargin;
			if(object->boundingRect().width() > width){
				width = object->boundingRect().width();
			}
		}


		 scene()->setSceneRect(0, 0, width, height);
	}

	void setPosition(DragAndDropObject* object, size_t index){
		object->setPos(-object->boundingRect().left() + leftMargin,topMargin +-object->boundingRect().top() + index*(object->boundingRect().height() + contentsMargin));
	}

	void setPositions(){
		for(int index=0; index<objects.count(); index++){
			setPosition(objects[index], index);
			objects[index]->setIndex(index);
		}
	}

	QList<DragAndDropObject*> objects;
};

#endif /* CX_DRAGANDDROPGRAPHICSVIEW_H_ */
