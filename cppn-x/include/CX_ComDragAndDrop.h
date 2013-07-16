/*
 * CX_ComDragAndDrop.h
 *
 *  Created on: Jul 10, 2013
 *      Author: joost
 */

#ifndef CX_COMDRAGANDDROP_H_
#define CX_COMDRAGANDDROP_H_

#include <QUndoCommand>

class DragAndDropGraphicsView;

class ComDragAndDrop: public QUndoCommand {
public:
	ComDragAndDrop(DragAndDropGraphicsView* dragAndDropGraphicsView, uint from, uint to):
		dragAndDropGraphicsView(dragAndDropGraphicsView), from(from), to(to){
		setText("move item");
	}

	virtual ~ComDragAndDrop(){
		//nix
	}

	void undo();
	void redo();

private:
	DragAndDropGraphicsView* dragAndDropGraphicsView;
	uint from;
	uint to;
};

#endif /* CX_COMDRAGANDDROP_H_ */
