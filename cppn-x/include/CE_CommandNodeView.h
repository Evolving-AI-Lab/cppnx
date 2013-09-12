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
//	typedef std::pair<Node*, NodeView*> nodeViewPair_t;

	CommandNodeView(NodeViewWidget* nodeViewWidget, QList<QGraphicsItem*> items, bool add=true);
	virtual ~CommandNodeView();

	void undo();
	void redo();

	bool isOk(){
		return ok;
	}

private:
	static const int betweenNodeMargin = 20;


	void addNodeViews();
	void removeNodeViews();
//	void setNodeviewPositions();

	NodeViewWidget* nodeViewWidget;
//	QList<nodeViewPair_t> nodeViewPairs;
	QList<NodeView*> nodeViewPairs;


	bool add;
	bool ok;
};

#endif /* CE_COMMANDNODEVIEW_H_ */
