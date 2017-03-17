/*
 * CE_NodeView.h
 *
 *  Created on: Jun 5, 2013
 *      Author: joost
 */

#ifndef CE_NODEVIEW_H_
#define CE_NODEVIEW_H_

//QT includes
#include <QGraphicsItem>

//Local includes
#include "CE_Defines.h"
#include "CE_Node.h"
#include "CX_DragAndDropObject.h"
#include "CX_Debug.hpp"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class NodeView : public DragAndDropObject {
	Q_OBJECT
public:
	NodeView(Node* node = 0);
	NodeView(std::iostream &stream, std::map<std::string, Node*> nodeMap);
	virtual ~NodeView();

    enum { Type = UserType + NODEVIEW_TYPE };
    virtual int type() const {dbg::trace trace("nodeview", DBG_HERE); return Type; }

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QImage* getImage(){dbg::trace trace("nodeview", DBG_HERE); return pixels;}
    Node* getNode(){dbg::trace trace("nodeview", DBG_HERE); return node;}

    std::string getNodeBranch() const;
    std::string getNodeId() const;
    std::string getName() const;

    static const int node_width = 256;
    static const int node_height = 256;
    static const int half_width = 128;
    static const int half_height = 128;

public slots:
	void update();
	void removeImage();
	void addImage();

//	void remove(){dbg::trace trace("nodeview", DBG_HERE); emit requestRemove(this);}
//	void add(){dbg::trace trace("nodeview", DBG_HERE); emit requestAdd(this);}

//signals:
//	void requestRemove(NodeView*);
//	void requestAdd(NodeView*);

protected:
	void init();

    QImage* pixels;
    Node* node;
};

#endif /* CE_NODEVIEW_H_ */
