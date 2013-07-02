/*
 * CE_CommandNodeView.h
 *
 *  Created on: Jun 28, 2013
 *      Author: joost
 */

#ifndef CE_COMMANDNODEVIEW_H_
#define CE_COMMANDNODEVIEW_H_

#include <qundostack.h>
#include <QGraphicsView>

#include "CE_Node.h"
#include "CE_NodeView.h"

class CommandNodeView: public QUndoCommand {
public:
	typedef std::pair<Node*, NodeView*> nodeViewPair_t;
	static bool nodeViewCompare(const nodeViewPair_t first, const nodeViewPair_t second);

	CommandNodeView(QGraphicsView* sidebar, QList<QGraphicsItem*> items, bool add=true);
	virtual ~CommandNodeView();

	void undo();
	void redo();


private:
	static const int betweenNodeMargin = 20;


	void addNodeViews();
	void removeNodeViews();
	void setNodeviewPositions();

	QGraphicsView* sidebar;
	QList<nodeViewPair_t> nodeViewPairs;
	bool add;
};

#endif /* CE_COMMANDNODEVIEW_H_ */
