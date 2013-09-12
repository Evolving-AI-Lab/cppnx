/*
 * CE_NodeView.h
 *
 *  Created on: Jun 5, 2013
 *      Author: joost
 */

#ifndef CE_NODEVIEW_H_
#define CE_NODEVIEW_H_

#include "CE_Defines.h"
#include "CE_Node.h"
#include "CX_DragAndDropObject.h"

#include <QGraphicsItem>

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
    virtual int type() const { return Type; }

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QImage* getImage(){
    	return pixels;
    }

    Node* getNode(){
    	return node;
    }

    std::string getNodeBranch(){
    	if(node){
    		return node->getBranch();
    	} else {
    		return "";
    	}
    }

    std::string getNodeId(){
    	if(node){
    		return node->getId();
    	} else {
    		return "final";
    	}
    }

    static const int node_width = 256;
    static const int node_height = 256;
    static const int half_width = 128;
    static const int half_height = 128;

public slots:
	void update();
	void remove(){
		emit requestRemove(this);
	}
	void add(){
		emit requestAdd(this);
	}

signals:
	void requestRemove(NodeView*);
	void requestAdd(NodeView*);

protected:
	void init();

    QImage* pixels;
    Node* node;
};

#endif /* CE_NODEVIEW_H_ */
