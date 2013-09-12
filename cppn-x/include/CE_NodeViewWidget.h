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
	void addNodeView(NodeView* node);


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

    QMenu* getNodeviewMenu(){
    	return nodeviewMenu;
    }

    bool deletableNodeviewSelected(){
		foreach(QGraphicsItem* item, scene()->selectedItems()){
			NodeView* nodeview = qgraphicsitem_cast<NodeView*>(item);
			if(nodeview) return true;
		}
		return false;
    }

	static const int sidebarMargin = 20;
    static const int betweenNodeMargin = 20;

public slots:
	void insertNodeView(NodeView* nodeView, size_t index);
	void insertNodeView(NodeView* nodeView);
	void deleteNodeView(NodeView* nodeToDelete);

	void deleteNodeView();
	void addNodeView(QList<QGraphicsItem*> nodes);
	void selectionUpdated();
	void saveImage();

private:
    QMenu* nodeviewMenu;
};

#endif /* CE_NODEVIEWWIDGET_H_ */
