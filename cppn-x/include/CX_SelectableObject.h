/*
 * CX_SelectableObject.h
 *
 *  Created on: Jul 9, 2013
 *      Author: joost
 */

#ifndef CX_SELECTABLEOBJECT_H_
#define CX_SELECTABLEOBJECT_H_

#include <QGraphicsWidget>
#include "CE_Defines.h"

class SelectableObject: public QGraphicsWidget {
	Q_OBJECT
public:
	SelectableObject();
	virtual ~SelectableObject();

    enum { Type = UserType + SELECTABLE_OBJECT_TYPE };
    int type() const { return Type; }

	void setContextMenu(QMenu* _contextMenu){
		contextMenu = _contextMenu;
	}

	QMenu* getContextMenu(){
		return contextMenu;
	}

	void setPartOfContextMenuEvent(bool partOf){
		partOfContextMenuEvent = partOf;
	}

	void setHasFocus(bool* _parentHasFocus){
		parentHasFocus = _parentHasFocus;
	}

	signals:
	void contextMenuEvent(SelectableObject*, bool);
	void selectedHasChanged();
	void onSelected();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);


	QMenu* contextMenu;
	bool partOfContextMenuEvent;
	bool* parentHasFocus;
};

#endif /* CX_SELECTABLEOBJECT_H_ */
