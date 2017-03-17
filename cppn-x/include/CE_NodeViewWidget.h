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

	//Getters and setters
    QList<NodeView*> getNodeviews();
    void setNodeviews(QList<NodeView*> nodeviews);
    QMenu* getNodeviewMenu(){dbg::trace trace("nodeviewwidget", DBG_HERE); return nodeviewMenu;}
    FinalNodeView* getFinalNodeView(){dbg::trace trace("nodeviewwidget", DBG_HERE); return _finalNodeView;}

	void clearNodeViews();
	void addNodeView(NodeView* node);

    bool deletableNodeviewSelected();

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
    FinalNodeView* _finalNodeView;
};

#endif /* CE_NODEVIEWWIDGET_H_ */
