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
//	NodeView(Node* node = 0, int width = 256, int height = 256);
	NodeView(Node* node = 0);
	virtual ~NodeView();

    enum { Type = UserType + NODEVIEW_TYPE };
    virtual int type() const { return Type; }

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

//    void resetNode(){
//    	node = NULL;
//    }

//    Node* getNode(){
//    	return node;
//    }

//    void setNode(Node* _node){
//    	node = _node;
//    }

    QImage* getImage(){
    	return pixels;
    }

//    size_t getPosition(){
//    	return position;
//    }
//
//    void setPosition(size_t _position){
//    	position = _position;
//    }

    static const int node_width = 256;
    static const int node_height = 256;
    static const int half_width = 128;
    static const int half_height = 128;

public slots:
	void update();

protected:
    QImage* pixels;
    Node* node;
//    size_t position;
};

#endif /* CE_NODEVIEW_H_ */
