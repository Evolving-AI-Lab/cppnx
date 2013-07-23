/*
 * CE_NodeViewWidget.h
 *
 *  Created on: Jul 2, 2013
 *      Author: joost
 */

#ifndef CE_NODEVIEWWIDGET_H_
#define CE_NODEVIEWWIDGET_H_

#include "CE_NodeView.h"
#include "CE_FinalNodeView.h"
#include "CX_ContextMenuGraphicsView.h"
#include "CX_DragAndDropGraphicsView.h"


class NodeViewWidget: public DragAndDropGraphicsView {
	Q_OBJECT
public:
	NodeViewWidget(QWidget* widget = 0);
	virtual ~NodeViewWidget();

	void clearNodeViews();
	void deleteNodeView(NodeView* nodeToDelete);
	void addNodeView(NodeView* node);
	void insertNodeView(NodeView* nodeView, size_t index);

	void setNodeviews(QList<NodeView*> nodeviews){
		clearNodeViews();
		foreach(NodeView* nodeview, nodeviews){
			addNodeView(nodeview);
		}
	}

	QList<NodeView*> getNodeviews(){
		QList<NodeView*> result;
		foreach(QGraphicsItem* item, objects){
			NodeView* nodeview = util::multiCast<NodeView*, FinalNodeView*>(item);
			if(nodeview) result.append(nodeview);
		}

		return result;
	}

	static const int sidebarMargin = 20;
    static const int betweenNodeMargin = 20;

    QAction* getDeleteViewNodeAction(){
    	return deleteViewNodeAction;
    }

public slots:
	void deleteNodeView();
	void addNodeView(QList<QGraphicsItem*> nodes);
	void selectionUpdated();
	void saveImage();
//	void moveNodes(uint source, uint target);
//	void ContextMenuEvent(SelectableObject* object, bool begin);

//signals:
//	void requestCommandExecution(QUndoCommand*);

private:
//    void setNodeviewPositions();
//	void setNodeviewPosition(NodeView* node, size_t index);
//	void setSidebarSceneRect();

//	QList<NodeView*> nodeViews;


    QAction* deleteViewNodeAction;
    QAction* saveImageAction;
    QMenu* nodeviewMenu;
};

#endif /* CE_NODEVIEWWIDGET_H_ */
