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
#include "CX_Debug.hpp"
#include "CX_Point.hpp"

class Node;

class Edge : public LabelableObject
{
	Q_OBJECT
public:
	//The label mode of this edge
	//May show only the label (unless the edge is unlabeled),
	//only the sign of the connection (not showing the label),
	//or both
    enum LabelMode {
    	onlyLabels,
		onlyConnectionSign,
		both,
		showLabel,
		noLabel,
    	modules};

    //Toggle showing the sign of the connection
    enum EdgeSignMode {noSign, showSign, showSignIfNoLabel};

    //Toggles between straight lines or brazier curves
    enum LineMode {straight, curved};

    //Toggles between straight lines or brazier curves
    enum EdgeLineWidthMode {fixedWidth, weightWidth};

    //Toggles between showing or hiding line annotation (e. g. the "1" shown in a circle on top of the line)
    enum EdgeLAnnotationMode {showAnnotation, hideAnnotation};

    //Tracks the number of edges in memory based on calls to the constructor and destructor.
    //Used for debugging purposes only
    static size_t edgesInMemory;

    //Color of excitatory edges
    static QColor excitatoryEdgeColor;

    //Color of inhibitory edges
    static QColor inhibitoryEdgeColor;

    //Color of zero edges
    static QColor zeroEdgeColor;

    //Color of unlabeled edges
    static QColor unlabeledEdgeColor;


    Edge(
    		std::string branch,
    		std::string id,
    		Node *sourceNode = 0,
    		Node *destNode = 0,
    		qreal weight = 0.0,
    		qreal original_weight = 0.0,
    		Label* label = 0,
    		std::string note = "",
    		LabelMode* labelMode = 0,
    		double bookendStart = -3.0,
    		double bookendEnd = 3.0,
    		double stepSize = 0.1
    		);

    Edge(std::iostream &stream, std::map<std::string, Node*> nodeMap, std::map<std::string, Label*> labelMap);
    ~Edge();

    Node *sourceNode() const;
    Node *destNode() const;

    void setSourceNode(Node* node);
    void setTargetNode(Node* node);

    void adjust();

    enum { Type = UserType + EDGE_TYPE };
    int type() const { return Type; }

    void increaseCurveOffset(){
    	_curveOffset += 1.0;
    }

    void decreaseCurveOffset(){
    	_curveOffset -= 1.0;
    }

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

    void setSignMode(EdgeSignMode* signMode){
        _signMode = signMode;
    }


    void setLineMode(LineMode* _lineMode){
    	lineMode = _lineMode;
    }

    void setLineWidthMode(EdgeLineWidthMode* lineWidthMode){
        _lineWidthMode = lineWidthMode;
    }

    void setAnnotationMode(EdgeLAnnotationMode* annotationMode){
        _annotationMode = annotationMode;
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

    void setFavourite(size_t index){
        _favourite_index = index;
        update();
    }

    void setCustomColor(QColor color){
        _customColor = color;
        _useCustomColor = QBool(true);
        update();
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
//    		  std::cout << _curveOffset << std::endl;
//    		  qreal offsetY = (abs(_line.p1().y() - _line.p2().y()) / 2);
    		  path.cubicTo(_curvePoint1, _curvePoint2, _line.p2());

//    		  path.cubicTo(_line.p1().x(), _line.p1().y() - offsetY + _curveOffset, _line.p2().x(), _line.p2().y() + offsetY + _curveOffset, _line.p2().x(), _line.p2().y());
    	  }

    	  return path;
    }


    LabelMode* labelMode;
    LineMode* lineMode;
    EdgeSignMode* _signMode;
    EdgeLineWidthMode* _lineWidthMode;
    EdgeLAnnotationMode* _annotationMode;

    std::string branch;
    std::string id;
    static const double m_click_easy_width;
    Node *source, *dest;

    QPointF sourcePoint;
    QPointF destPoint;

    QPointF _curvePoint1;
    QPointF _curvePoint2;

    qreal currentWeight;
    qreal originalWeight;
    size_t index;

    size_t _favourite_index;

    int _flash;
    QLineF _line;

    double bookendStart;
    double bookendEnd;
    double stepSize;

    qreal _curveOffset;

    QBool _useCustomColor;
    QColor _customColor;
//    Point* _point;

};

std::ostream& operator<< (std::ostream& os, const Edge& obj);

#endif
