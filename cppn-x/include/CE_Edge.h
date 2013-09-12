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
#include "CE_LabelableObject.h"

class Node;

class Edge : public LabelableObject
{
	Q_OBJECT
public:
    enum LabelMode {onlyLabels, onlyConnectionSign, both};
    enum LineMode {straight, curved};

    Edge(
    		std::string branch,
    		std::string id,
    		Node *sourceNode,
    		Node *destNode,
    		qreal weight,
    		qreal original_weight,
    		Label* label = 0,
    		std::string note = "",
    		LabelMode* labelMode = 0,
    		double bookendStart = -3.0,
    		double bookendEnd = 3.0,
    		double stepSize = 0.1
    		);

    Edge(std::iostream &stream, std::map<std::string, Node*> nodeMap, std::map<std::string, Label*> labelMap);

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

    std::string getName() const{
    	return branch + "_" + id;
    }


    void flash(bool flashOn){
    	if(flashOn && _flash != 0){
    		_flash = -1*_flash;
    	} else if(flashOn) {
    		_flash = 1;
    	} else {
    		_flash = 0;
    	}
    }

    void updateAll(){
    	emit requestUpdateAll();
    }

    void setLabelMode(LabelMode* _labelMode){
    	labelMode = _labelMode;
    }

    void setLineMode(LineMode* _lineMode){
    	lineMode = _lineMode;
    }

    void setBookendStart(double bookend){
    	bookendStart = bookend;
    	emit bookendsChanged(this);
    }

    void setBookendEnd(double bookend){
    	bookendEnd = bookend;
    	emit bookendsChanged(this);
    }

    void setBookends(double _bookendStart, double _bookendEnd, double _bookendStep){
    	bookendStart = _bookendStart;
    	bookendEnd = _bookendEnd;
    	stepSize = _bookendStep;
    	emit bookendsChanged(this);
    }

    double getBookendStart(){
    	return bookendStart;
    }

    double getBookendEnd(){
    	return bookendEnd;
    }

    double getStepsize(){
    	return stepSize;
    }

signals:
	void weightChanged(Edge*, double, bool);
	void requestUpdateAll();
	void bookendsChanged(Edge*);


protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    
private:
    void init();

    QPainterPath getPath() const{
    	  QPainterPath path;
    	  path.moveTo(_line.p1());

    	  if(*lineMode == straight){
    		  path.lineTo(_line.p2());
    	  } else {
    		  int offsetY = abs(_line.p1().y() - _line.p2().y()) / 2;
    		  path.cubicTo(_line.p1().x(), _line.p1().y() - offsetY, _line.p2().x(), _line.p2().y() + offsetY, _line.p2().x(), _line.p2().y());
    	  }

    	  return path;
    }


    LabelMode* labelMode;
    LineMode* lineMode;

    std::string branch;
    std::string id;
    static const double m_click_easy_width;
    Node *source, *dest;

    QPointF sourcePoint;
    QPointF destPoint;

    qreal currentWeight;
    qreal originalWeight;
    size_t index;

    int _flash;
    QLineF _line;

    double bookendStart;
    double bookendEnd;
    double stepSize;

};

#endif
