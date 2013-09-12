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

#include <QPainter>
#include <iostream>
#include <math.h>

#include "CE_Edge.h"
#include "CE_LabelWidget.h"


const double Edge::m_click_easy_width = 10.0;


Edge::Edge(std::string branch,
		std::string id,
		Node *sourceNode,
		Node *destNode,
		qreal weight,
		qreal original_weight,
		Label* label,
		std::string note,
		LabelMode* labelMode,
		double bookendStart,
		double bookendEnd,
		double stepSize)
    : LabelableObject(label, note.c_str()),
      labelMode(labelMode),
      branch(branch),
      id(id),
//      arrowSize(5),
      currentWeight(weight),
      originalWeight(original_weight),
      _flash(0),
      bookendStart(bookendStart),
      bookendEnd(bookendEnd),
      stepSize(stepSize)
{

    source = sourceNode;
    dest = destNode;

    init();
}

Edge::Edge(std::iostream &stream, std::map<std::string, Node*> nodeMap, std::map<std::string, Label*> labelMap):LabelableObject(stream, labelMap)
{
	std::string sourceId;
	std::string targetId;

	stream >> branch;
//	std::cout << "Branch: "<< branch << std::endl;
	stream >> id;
//	std::cout << "Id: "<< id << std::endl;
	stream >> sourceId;
//	std::cout << "Source: "<< sourceId << std::endl;
	stream >> targetId;
//	std::cout << "Target: "<< targetId << std::endl;
	stream >> currentWeight;
//	std::cout << "Current weight: " << std::setprecision(17) << currentWeight << std::endl;
	stream >> originalWeight;
//	std::cout << "Original weight: " << originalWeight << std::endl;
	stream >> bookendStart;
//	std::cout << bookendStart << std::endl;
	stream >> bookendEnd;
//	std::cout << bookendEnd << std::endl;
	stream >> stepSize;
//	std::cout << stepSize << std::endl;


	source = nodeMap[sourceId];
	dest = nodeMap[targetId];
	init();
}

void Edge::init(){
	_flash = 0;
	labelMode = 0;
	this->setFlag(QGraphicsItem::ItemIsSelectable);
	this->setCacheMode(NoCache);
	adjust();
}


Node* Edge::sourceNode() const
{
    return source;
}

Node* Edge::destNode() const
{
    return dest;
}

void Edge::setWeight(qreal weight, bool update){
	if(weight == currentWeight) return;
	currentWeight = weight;
	emit weightChanged(this, currentWeight, update);
	if(update) this->update();
}

void Edge::adjust()
{
    if (!source || !dest)
        return;

    QLineF newline(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = newline.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
    	QPointF sourceOffset = QPointF(0, Node::half_height);
    	QPointF targetOffset = QPointF(0, Node::half_height+Node::footerBarSize);

        sourcePoint = newline.p1() - sourceOffset;
        destPoint = newline.p2() + targetOffset;
    } else {
        sourcePoint = destPoint = newline.p1();
    }

    _line = QLineF(sourcePoint, destPoint);
}

QRectF Edge::boundingRect() const
{
    if (!source || !dest) return QRectF();

    qreal extra = abs(_line.p1().y() - _line.p2().y()) / 2;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized().adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	bool drawBackground= false;
	if (!source || !dest)
		return;

	QPen pen;
	QPainterPath path = getPath();


	QLineF line(sourcePoint, destPoint);
	if (qFuzzyCompare(line.length(), qreal(0.)))
		return;

	//Set connection color
	QColor connectionColor;
	if(currentWeight == 0){
		connectionColor = QColor(150,150,150);
	} else if(currentWeight > 0){
		connectionColor = QColor(0,150,0);
	} else {
		connectionColor = QColor(150,0,0);
	}

	//Set basic colors
	QColor lineColor;
	QColor backgroundColor;
	QColor highlightColor;

	if (partOfContextMenuEvent){
		highlightColor = QColor(CONTEXT_EVENT_COLOR);
	} else if (this->isSelected()){
		if(*parentHasFocus){
			highlightColor = QColor(SELECTED_COLOR);
		} else {
			highlightColor = QColor(NO_FOCUS_SELECTED_COLOR);
		}
	}

	switch(*labelMode){
		case onlyLabels:
			if(!label->isDeleted()){
				lineColor = label->getColor();
			} else {
				lineColor = connectionColor;
			}
			backgroundColor = highlightColor;
		break;
		case(onlyConnectionSign):
			lineColor = connectionColor;
			backgroundColor = highlightColor;
		break;
		case(both):
			lineColor = connectionColor;
			if(!label->isDeleted()){
				backgroundColor = label->getColor();
				drawBackground = true;
				if (this->isSelected() || partOfContextMenuEvent){
					backgroundColor = backgroundColor.lighter();
				}
			} else {
				backgroundColor = highlightColor;
			}
		break;

	}

	//Modify colors
	if(_flash == -1){
		if(lineColor.value() < 10){
			lineColor.setHsv(lineColor.saturation(), lineColor.saturation(), lineColor.value()+40);
		}
		lineColor = lineColor.lighter();
		backgroundColor = backgroundColor.lighter();
		drawBackground = true;
	} else if(_flash == 1){
		lineColor = lineColor.darker();
		backgroundColor = backgroundColor.darker();
		drawBackground = true;
	} else if (this->isSelected() || partOfContextMenuEvent){
		if(lineColor.value() < 10){
			lineColor.setHsv(lineColor.saturation(), lineColor.saturation(), lineColor.value()+40);
		}
		lineColor = lineColor.lighter();
		drawBackground = true;
	}

	//Draw lines
	if(drawBackground){
		pen.setColor(backgroundColor);
		pen.setWidthF(10.0);
		painter->setPen(pen);
		painter->drawPath(path);
	}

	pen.setColor(lineColor);
	pen.setWidthF(abs(currentWeight)+2.0);
	painter->setPen(pen);
	painter->drawPath(path);
}

QPainterPath Edge::shape() const
{
	QPainterPathStroker stroker;
	stroker.setWidth(m_click_easy_width);
	return stroker.createStroke(getPath());
}
