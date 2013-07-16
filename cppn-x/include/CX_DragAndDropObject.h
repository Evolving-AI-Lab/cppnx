/*
 * CX_DragAndDropObject.h
 *
 *  Created on: Jul 9, 2013
 *      Author: joost
 */

#ifndef CX_DRAGANDDROPOBJECT_H_
#define CX_DRAGANDDROPOBJECT_H_

#include "CX_SelectableObject.h"

class DragAndDropObject: public SelectableObject {
	Q_OBJECT
public:
	DragAndDropObject();
	virtual ~DragAndDropObject();

	void setDragImage(QPixmap* _dragImage){
		dragImage = _dragImage;
	}

	void setDragImage(QImage* _dragImage){
		alternateDragImage = _dragImage;
	}

	void setIconSize(QSize _iconSize){
		iconSize = _iconSize;
	}

	void setIconTranslate(QPoint translate){
		iconTranslate = translate;
	}

	void setIndex(uint _index){
		if(index!= _index){
			index= _index;
			emit indexChanged(index);
		}
	}

	uint getIndex(){
		return index;
	}

signals:
	void requestMove(uint, uint);
	void indexChanged(uint);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    bool dragOver;
    QPixmap* dragImage;
    QImage* alternateDragImage;
	QSize iconSize;
	QPoint iconTranslate;
    uint index;
    uint compatibillityId;
};

#endif /* CX_DRAGANDDROPOBJECT_H_ */
