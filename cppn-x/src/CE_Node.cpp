/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <iostream>

#include "CE_Node.h"

//! [0]
Node::Node(GraphWidget *graphWidget, std::string branch, std::string id, std::string type, std::string activationFunction_str, std::string label, int width, int height, QColor color)
    : graph(graphWidget), branch(branch), id(id), nodetype(type),activationFunction_str(activationFunction_str), label(label), color(color)
{

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(1);

	if(activationFunction_str == XML_GAUSSIAN){
		activationFunction = ACTIVATION_FUNCTION_GAUSSIAN;
	} else if(activationFunction_str == XML_LINEAR){
		activationFunction = ACTIVATION_FUNCTION_LINEAR;
	} else if(activationFunction_str == XML_SIN){
		activationFunction = ACTIVATION_FUNCTION_SIN;
	} else if(activationFunction_str == XML_SIGMOID){
		activationFunction = ACTIVATION_FUNCTION_SIGMOID;
	} else if(activationFunction_str == XML_COS){
		activationFunction = ACTIVATION_FUNCTION_COS;
	} else {
		throw JGTL::LocatedException("File contains unknown activation function: '" + activationFunction_str + "'");
	}


    QImage* pixels = new QImage(width, height, QImage::Format_RGB32);
    pixels->fill(0);
    setPixels(pixels);
}
//! [0]

Node::~Node(){
	delete (pixels);
}

void Node::addIncommingEdge(Edge *edge)
{
	incomingEdgeList << edge;
    edge->adjust();
}
void Node::addOutgoingEdge(Edge *edge)
{
	outgoingEdgeList << edge;
    edge->adjust();
}
QList<Edge *> Node::incomingEdges() const
{
    return incomingEdgeList;
}
QList<Edge *> Node::outgoingEdges() const
{
    return outgoingEdgeList;
}

//! [1]

//! [8]
QRectF Node::boundingRect() const
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    // Add some extra space around the circle for easier touching with finger
    qreal adjust = 30;
    return QRectF( -10 - adjust, -10 - adjust,
                  20 + adjust * 2, 20 + adjust * 2);
#else
    qreal adjust = 2;
    return QRectF( -20 - adjust, (-20 - adjust) - 10, 40 + 2*adjust, 40 + 2*adjust + 10);
#endif
}
//! [8]

//! [9]
QPainterPath Node::shape() const
{
    QPainterPath path;
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    // Add some extra space around the circle for easier touching with finger
    path.addEllipse( -40, -40, 80, 80);
#else
    path.addRect(-20, -20, 40, 40);

    //path.addEllipse(-10, -10, 20, 20);
#endif
    return path;
}
//! [9]

//! [10]
void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
//    painter->setPen(Qt::NoPen);
//    painter->setBrush(Qt::darkGray);
//    painter->drawEllipse(-7, -7, 20, 20);
//
//    QRadialGradient gradient(-3, -3, 10);
//    if (option->state & QStyle::State_Sunken) {
//        gradient.setCenter(3, 3);
//        gradient.setFocalPoint(3, 3);
//        gradient.setColorAt(1, QColor(Qt::yellow).light(120));
//        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
//    } else {
        //gradient.setColorAt(0, Qt::yellow);
        //gradient.setColorAt(1, Qt::darkYellow);
//    }
    //painter->setBrush(gradient);




    //std::cout << pixels << std::endl;
    //painter->drawEllipse(-10, -10, 20, 20);
   // std::cout << "Painting" << std::endl;


    painter->drawImage(QRect(-20, -20, 40, 40), *pixels);
    if (this->isSelected()){
    	painter->setPen(QPen(color, 2));
    	painter->drawRect(QRect(-20, -20, 40, 40));
    }else{
    	painter->setPen(QPen(color, 0));
    	painter->drawRect(QRect(-20, -20, 40, 40));
    }
    painter->drawText(-20,-20, QString(label.c_str()));

}
//! [10]

//! [11]
QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (Edge *edge, incomingEdgeList)
            edge->adjust();
        graph->itemMoved();
        foreach (Edge *edge, outgoingEdgeList)
        	edge->adjust();
        graph->itemMoved();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}
//! [11]

//! [12]
void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
//! [12]

//! [13]
void Node::setPixels(QImage* pixels_)
{
    pixels = pixels_;
}
//! [13]

void Node::setPixel(int x, int y, char r, char g, char b){
	pixels->setPixel(x, y, qRgb(r, g, b));
}

void Node::setPixel(size_t index, char r, char g, char b ){
	index = index*4;
	pixels->bits()[index]=r;
	pixels->bits()[index+1]=g;
	pixels->bits()[index+2]=b;
}

void Node::setPixel(size_t index, char grey){
	index = index*4;
	pixels->bits()[index]=grey;
	pixels->bits()[index+1]=grey;
	pixels->bits()[index+2]=grey;
}
