/*
 * CE_NodeView.h
 *
 *  Created on: Jun 5, 2013
 *      Author: joost
 */

#ifndef CE_NODEVIEW_H_
#define CE_NODEVIEW_H_

#include "CE_Defines.h"

#include <QGraphicsItem>



class Node;

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE



class NodeView : public QGraphicsItem {
public:
//	NodeView(int width = 256, int height = 256);
	NodeView(Node* node = 0, int width = 256, int height = 256);
	virtual ~NodeView();

    enum { Type = UserType + NODEVIEW_TYPE };
    virtual int type() const { return Type; }

//    void calculateForces();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPixels(QImage* pixels_);
    void setPixel(int x, int y, char r, char g, char b);
    void setPixel(size_t index, char r, char g, char b);
    void setPixel(const size_t& index, const char& grey);
    void setPixel(const size_t& index, const double& value);
    void setPixel(const size_t& index, const char& red, const char& grey);

    void resetNode(){
    	node = NULL;
    }

    Node* getNode(){
    	return node;
    }

    void setNode(Node* _node){
    	node = _node;
    }

    QImage* getImage(){
    	return pixels;
    }

    size_t getPosition(){
    	return position;
    }

    void setPosition(size_t _position){
    	position = _position;
    }

//    void setColor(QColor _color){
//    	color = _color;
//    }


    static const int node_width = 256;
    static const int node_height = 256;
    static const int half_width = 128;
    static const int half_height = 128;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    QImage* pixels;
    Node* node;
    size_t position;
};

#endif /* CE_NODEVIEW_H_ */
