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

#include <QtGui>
#include <QGraphicsItem>
#include <QList>
#include <QPointer>

#include "CE_Defines.h"
#include "CE_Edge.h"
//#include "CE_CppnWidget.h"
#include "CE_LabelableObject.h"




class Label;
class Edge;
class NodeView;
class FinalNodeView;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE




//! [0]
class Node : public LabelableObject
{
	Q_OBJECT
public:
    Node(
    		std::string branch = "",
    		std::string id = "",
    		std::string type = "",
    		std::string activationFunction_str=XML_SIGMOID,
    		std::string xml_label = "",
    		std::string affinity = "",
    		std::string bias = "",
    		int width = 256,
    		int height = 256,
    		Label* label = 0,
    		std::string note = ""
    );

    Node(std::iostream &stream, std::map<std::string, Label*> labelMap);
    ~Node();

    void init();

    void addIncommingEdge(Edge *edge);
    void addOutgoingEdge(Edge *edge);
    QList<Edge *> incomingEdges() const;
    QList<Edge *> outgoingEdges() const;

    enum { Type = UserType + NODE_TYPE };
    int type() const { return Type; }

//    void calculateForces();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPixels(QImage* pixels_);
    void setPixel(int x, int y, char r, char g, char b);
    void setPixel(size_t index, char r, char g, char b);
    void setPixel(size_t index, char grey);
    void setPixel(size_t index, const double& value);

    void updateAll();

   // std::string getName(){return name;}
    std::string getType(){return nodetype;}

    void setActivationFunction(ActivationFunctionPt _activationFunction){
    	activationFunction =_activationFunction;
    }

    ActivationFunctionPt getActivationFunction(){
    	return activationFunction;
    }

    void setIndex(size_t _index){
    	index =_index;
    }

    size_t getIndex(){
    	return index;
    }

    std::string getXmlLabel(){
    	return xml_label;
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

//    void setNodeView(NodeView* _nodeView);
//    void resetNodeView(bool toDelete = true);

//    NodeView* getNodeView(){
//    	return nodeView;
//    }

    QImage* getImage(){
    	return pixels;
    }

//    void setFinalNodeView(FinalNodeView* _nodeView){
//    	finalNodeView = _nodeView;
//    }

    std::string getAffinity(){return affinity;};
    std::string getBias(){return bias;};

//    void setCppn(Cppn* _cppn){
//    	cppn = _cppn;
//    }

    void redraw();

    void setPrevPos(QPointF point);
    QPointF getPrevPos();
    void updatePosition();

    void setDepth(int _depth){
    	depth =_depth;
    }

    int getDepth(){
    	return depth;
    }

    void finishImage(){
    	update();
    }


    size_t y_index;
    size_t x_index;

    static const int node_width = 40;
    static const int node_height = 40;
    static const int half_width = 20;
    static const int half_height = 20;
    static const int footerBarSize = 10;
    static const int headerBarSize = 10;



signals:
	void updatePositionsRequest();
	void positionChanged(Node*);
	void updateRequest(Node*);
	void imageChanged();

protected:
//	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
private:
    QList<Edge *> incomingEdgeList;
    QList<Edge *> outgoingEdgeList;
//    CppnWidget *graph;
    QImage* pixels;


    std::string branch;
    std::string id;
    std::string nodetype;
    std::string activationFunction_short;
    std::string activationFunction_str;
    std::string xml_label;
    std::string affinity;
    std::string bias;

    ActivationFunctionPt activationFunction;

    size_t index;
//    NodeView* nodeView;
//    FinalNodeView* finalNodeView;
//    Cppn* cppn;

    QPointF previousPosition;

    int depth;

};
//! [0]

#endif
