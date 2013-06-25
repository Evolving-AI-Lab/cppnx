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
#include <cmath>

#include "CE_Node.h"
#include "CE_FinalNodeView.h"

//! [0]
Node::Node(GraphWidget *graphWidget, std::string branch, std::string id, std::string type, std::string activationFunction_str, std::string label, std::string affinity, std::string bias, int width, int height, QColor color)
    : graph(graphWidget), branch(branch), id(id), nodetype(type),activationFunction_str(activationFunction_str), label(label), affinity(affinity), bias(bias), color(color), nodeView(0), finalNodeView(0)
{

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(1);

	if(activationFunction_str == XML_GAUSSIAN){
		activationFunction = act_functions::gaussian;
		activationFunction_short = "gau(x)";
	} else if(activationFunction_str == XML_LINEAR){
		activationFunction = act_functions::identity;
		activationFunction_short = "lin(x)";
	} else if(activationFunction_str == XML_SIN){
		activationFunction = act_functions::sin;
		activationFunction_short = "sin(x)";
	} else if(activationFunction_str == XML_SIGMOID){
		activationFunction = act_functions::sigmoid;
		activationFunction_short = "sig(x)";
	} else if(activationFunction_str == XML_COS){
		activationFunction = act_functions::cos;
		activationFunction_short = "cos(x)";
	} else {
		throw JGTL::LocatedException("File contains unknown activation function: '" + activationFunction_str + "'");
	}


    QImage* pixels = new QImage(width, height, QImage::Format_RGB32);
    pixels->fill(0);
    setPixels(pixels);
}
//! [0]

Node::~Node(){
	resetNodeView();
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
    return QRectF( -half_width - adjust, (-half_height - adjust) - footerBarSize, node_width + 2*adjust, node_height + 2*adjust + footerBarSize*2);
#endif
}
//! [8]

//! [9]
QPainterPath Node::shape() const
{
    QPainterPath path;

    path.addRect(-half_width, -half_height, node_width, node_height);

    return path;
}
//! [9]

//! [10]
void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	Q_UNUSED(option);
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

//	painter->setRenderHint(QPainter::Antialiasing);
    painter->drawImage(QRect(-half_width, -half_height, node_width, node_height), *pixels);
    painter->fillRect(QRect(-half_width, half_height, node_width, footerBarSize), color);

    if (this->isSelected()){
    	painter->setPen(QPen(Qt::black, 2));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height + footerBarSize));
    }else{
    	painter->setPen(QPen(Qt::black, 0));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height + footerBarSize));
    }

    painter->setPen(QPen(Qt::black, 0));
    painter->drawRect(QRect(-half_width, half_height, node_width, footerBarSize));



    if(color.lightness() < 128){
    	painter->setPen(QPen(Qt::white, 0));
    } else {
    	painter->setPen(QPen(Qt::black, 0));
    }

    if(nodetype == XML_TYPE_INPUT){
    	painter->drawText(-half_width,half_height+(footerBarSize-4), QString(label.c_str()));
    } else{
    	painter->drawText(-half_width,half_height+(footerBarSize-4), QString(activationFunction_short.c_str()));
    	painter->setPen(QPen(Qt::black, 0));
    	painter->drawText(-half_width,-half_height, QString(label.c_str()));
    }

}
//! [10]

void Node::updateAll(){
	update();
	if(nodeView) nodeView->update();
}


//! [11]
QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (Edge *edge, incomingEdgeList)
            edge->adjust();
//        graph->itemMoved(this);
        foreach (Edge *edge, outgoingEdgeList)
        	edge->adjust();
//        graph->itemMoved(this);
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
    graph->itemMoved(this);
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

void Node::setPixel(size_t index, const double& value){
	size_t localindex = index*4;
	//Grey does not use the min() function to prevent a bug on windows.
	char grey = 255;
	if(std::abs(value) < 1.0) grey = std::abs(value)*255;
	char alt(std::min(std::max(value, 0.0), 1.0)*255);
	pixels->bits()[localindex]=alt;
	pixels->bits()[localindex+1]=alt;
	pixels->bits()[localindex+2]=grey;



	if(nodeView){
		nodeView->setPixel(localindex, grey, alt);
	}
	if(finalNodeView){
//		std::cout << "Hallo" <<std::endl;
		char sat(std::min((value+1)/2, 1.0)*255);
//		int hue(std::min((value+1)/2, 1.0)*360);
//		int hue(std::min(std::abs(value), 1.0)*360);
		int hue(std::min(value+1, 2.0)*360);
		if(label == "saturation") finalNodeView->setSaturation(index, sat);
		if(label == "brightness" || label == "ink") finalNodeView->setValue(index, grey);
		if(label == "hue") finalNodeView->setHue(index, hue);
	}
}

void Node::resetNodeView(bool toDelete){
	if(nodeView){
		if(toDelete){
			nodeView->resetNode();
			delete nodeView;
		}
		nodeView = 0;
	}
}

void Node::setNodeView(NodeView* _nodeView){
	resetNodeView();
	nodeView = _nodeView;
}

