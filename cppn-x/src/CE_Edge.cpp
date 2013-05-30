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

#include "CE_Edge.h"


#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;
const double Edge::m_click_easy_width = 10.0;

//! [0]
Edge::Edge(GraphWidget *graphWidget, Node *sourceNode, Node *destNode, int id, qreal weight, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsLineItem(0,0,2,2,parent,scene), graphWidget(graphWidget), id(id),  arrowSize(5), currentWeight(weight), originalWeight(weight)
{
	this->setFlag(QGraphicsItem::ItemIsSelectable);
    source = sourceNode;
    dest = destNode;
    source->addOutgoingEdge(this);
    dest->addIncommingEdge(this);
    adjust();
}
//! [0]

//! [1]
Node *Edge::sourceNode() const
{
    return source;
}

Node *Edge::destNode() const
{
    return dest;
}
//! [1]

//! [2]
void Edge::adjust()
{
    if (!source || !dest)
        return;

    QLineF newline(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = newline.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
    	QPointF edgeOffset;
    	if(abs(newline.dx()) > abs(newline.dy())){
    		edgeOffset = QPointF((20/std::fabs(float(newline.dx())))*newline.dx(), (20/std::fabs(float(newline.dx())))*newline.dy());
    	} else {
    		edgeOffset = QPointF((20/std::fabs(float(newline.dy())))*newline.dx(), (20/std::fabs(float(newline.dy())))*newline.dy());
    	}

        sourcePoint = newline.p1() + edgeOffset;
        destPoint = newline.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = newline.p1();
    }

    setLine(QLineF(sourcePoint, destPoint));
}
//! [2]

//! [3]
QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//! [3]

//! [4]
void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if (!source || !dest)
		return;

	QLineF line(sourcePoint, destPoint);
	if (qFuzzyCompare(line.length(), qreal(0.)))
		return;
	//! [4]

	     //! [5]
	// Draw the line itself
	QColor color;
	if (this->isSelected())
	{
		if(currentWeight >= 0){
			color = QColor(0,255,0);
		} else {
			color = QColor(255,0,0);
		}
		QPen pen;
		pen.setColor(color);
		//pen.setWidth(3);
		pen.setWidthF(abs(currentWeight));
		painter->setPen(pen);
		QBrush brush;
		brush.setColor(color);
		brush.setStyle(Qt::SolidPattern);
		painter->setBrush(brush);
	}
	else
	{
		if(currentWeight >= 0){
			color = QColor(0,150,0);
		} else {
			color = QColor(150,0,0);
		}

		QPen pen;
		pen.setColor(color);
		//pen.setWidth(3);
		pen.setWidthF(abs(currentWeight));
		painter->setPen(pen);
		QBrush brush;
		brush.setColor(color);
		brush.setStyle(Qt::SolidPattern);
		painter->setBrush(brush);
	}

	//painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->drawLine(this->line());
	//! [5]

//! [6]
    // Draw the arrows
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;

//    QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + Pi / 3) * arrowSize,
//                                                  cos(angle + Pi / 3) * arrowSize);
//    QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
//                                                  cos(angle + Pi - Pi / 3) * arrowSize);
    QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
                                              cos(angle - Pi / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
                                              cos(angle - Pi + Pi / 3) * arrowSize);

    painter->setBrush(Qt::black);
//    painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}

QPainterPath Edge::shape() const
{
  //Thanks to norobro for posting this code at
  //http://www.qtcentre.org/threads/49201-Increase-margin-for-detecting-tooltip-events-of-QGraphicsLineItem
  QPainterPath path;
  QPainterPathStroker stroker;
  path.moveTo(line().p1());
  path.lineTo(line().p2());
  stroker.setWidth(m_click_easy_width);
  return stroker.createStroke(path);
}
//! [6]

void Edge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	graphWidget->edgeSelected(id, this);
}
