/*
 * CX_ComDragAndDrop.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: joost
 */

#include "CX_ComDragAndDrop.h"
#include "CX_DragAndDropGraphicsView.h"

//ComDragAndDrop::ComDragAndDrop() {
//	// TODO Auto-generated constructor stub
//
//}

//ComDragAndDrop::~ComDragAndDrop() {
//	// TODO Auto-generated destructor stub
//}
//

void ComDragAndDrop::undo(){
	dragAndDropGraphicsView->move(to, from);
}

void ComDragAndDrop::redo(){
	dragAndDropGraphicsView->move(from, to);
}
