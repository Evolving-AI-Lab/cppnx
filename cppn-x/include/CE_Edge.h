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

#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>

#include "CE_Node.h"
#include "CE_CppnWidget.h"
#include "CE_LabelableObject.h"
#include "CE_Cppn.h"



class Node;

//! [0]
class Edge : public LabelableObject
{
public:
    Edge(
    		GraphWidget *graphWidget,
    		std::string branch,
    		std::string id,
    		Node *sourceNode,
    		Node *destNode,
    		qreal weight,
    		qreal original_weight,
    		LabelWidget* label = 0,
    		std::string note = "",
    		QGraphicsItem *parent = 0,
    		QGraphicsScene *scene = 0
    		);

    Node *sourceNode() const;
    Node *destNode() const;

    void adjust();

    enum { Type = UserType + EDGE_TYPE };
    int type() const { return Type; }

    void resetWeight(){
    	setWeight(originalWeight);
    }

    void setWeight(qreal weight, bool update = false);

    qreal getOriginalWeight(){
    	return originalWeight;
    }

    qreal getWeight(){
    	return currentWeight;
    }

    void setIndex(size_t _index){
    	index =_index;
    }

    size_t getIndex(){
    	return index;
    }

    std::string getBranch(){
    	return branch;
    }

    std::string getId(){
    	return id;
    }

    void setCppn(Cppn* _cppn){
    	cppn = _cppn;
    }

    Cppn* getCppn(){
    	return cppn;
    }

    void flash(bool flashOn){
//    	std::cout << flashOn << " " << _flash <<std::endl;
    	if(flashOn && _flash != 0){
    		_flash = -1*_flash;
    	} else if(flashOn) {
    		_flash = 1;
    	} else {
    		_flash = 0;
    	}
    }

protected:
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    
private:

    std::string branch;
    std::string id;
    static const double m_click_easy_width;
    Node *source, *dest;

    QPointF sourcePoint;
    QPointF destPoint;
    qreal arrowSize;
 //   QString edge_id;

    GraphWidget *graphWidget;
    qreal currentWeight;
    qreal originalWeight;
    size_t index;

    int _flash;
    QLineF _line;

    Cppn* cppn;
};
//! [0]

#endif
