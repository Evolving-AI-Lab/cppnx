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

#ifndef NODE_H
#define NODE_H

#include "CE_Defines.h"
#include "CE_Edge.h"
#include "CE_CppnWidget.h"

#include <QGraphicsItem>
#include <QList>
#include <QPointer>

class Edge;
class GraphWidget;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE




//! [0]
class Node : public QGraphicsItem
{

public:
    Node(GraphWidget *graphWidget, std::string branch = "", std::string id = "", std::string type = "", std::string activationFunction_str=XML_SIGMOID, std::string label = "", int width = 256, int height = 256, QColor color = QColor(0,0,0));
    ~Node();

    void addIncommingEdge(Edge *edge);
    void addOutgoingEdge(Edge *edge);
    QList<Edge *> incomingEdges() const;
    QList<Edge *> outgoingEdges() const;

    enum { Type = UserType + 1 };
    int type() const { return Type; }

//    void calculateForces();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPixels(QImage* pixels_);
    void setPixel(int x, int y, char r, char g, char b);
    void setPixel(size_t index, char r, char g, char b);
    void setPixel(size_t index, char grey);

   // std::string getName(){return name;}
    std::string getType(){return nodetype;}

    void setActivationFunction(ActivationFunction _activationFunction){
    	activationFunction =_activationFunction;
    }

    ActivationFunction getActivationFunction(){
    	return activationFunction;
    }

    void setIndex(size_t _index){
    	index =_index;
    }

    size_t getIndex(){
    	return index;
    }

    std::string getLabel(){
    	return label;
    }

    std::string getBranch(){
    	return branch;
    }

    std::string getId(){
    	return id;
    }

    std::string getXmlActivationFunction(){
    	return activationFunction_str;
    }

    QColor getColor(){
    	return color;
    }

    void setColor(QColor _color){
    	color = _color;
    }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
private:
    QList<Edge *> incomingEdgeList;
    QList<Edge *> outgoingEdgeList;
    GraphWidget *graph;
    QImage* pixels;

    std::string branch;
    std::string id;
    std::string nodetype;
    std::string activationFunction_str;
    std::string label;
    ActivationFunction activationFunction;
    QColor color;

    size_t index;
};
//! [0]

#endif