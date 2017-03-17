/*
 * CE_CommandNodeView.h
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDNODEVIEW_H_
#define CE_COMMANDNODEVIEW_H_

#include <QGraphicsView>

#include "CE_NodeViewWidget.h"
#include "CX_ComBase.h"

class CommandNodeView: public ComBase {
public:
    CommandNodeView(NodeViewWidget* nodeViewWidget, QList<QGraphicsItem*> items, bool add=true);
	virtual ~CommandNodeView();

	void undo();
	void redo();

	bool isOk() const{return ok;}

private:
	static const int betweenNodeMargin = 20;

	void addNodeViews();
	void removeNodeViews();

	NodeViewWidget* _nodeViewWidget;
	QList<NodeView*> nodeViewPairs;


	bool add;
	bool ok;
};

#endif /* CE_COMMANDNODEVIEW_H_ */
