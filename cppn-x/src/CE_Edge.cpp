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


Edge::Edge(GraphWidget *graphWidget, std::string branch, std::string id, Node *sourceNode, Node *destNode, qreal weight, qreal original_weight, LabelWidget* label, std::string note, QGraphicsItem *parent, QGraphicsScene *scene)
    : LabelableObject(graphWidget->getWindow(), label, note.c_str()), branch(branch), id(id),  arrowSize(5), graphWidget(graphWidget), currentWeight(weight), originalWeight(original_weight), _flash(0)
{
	this->setFlag(QGraphicsItem::ItemIsSelectable);
	this->setCacheMode(NoCache);
    source = sourceNode;
    dest = destNode;
    source->addOutgoingEdge(this);
    dest->addIncommingEdge(this);
    adjust();
    cppn=0;
}
//! [0]

//! [1]
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
	if(cppn) cppn->setWeight(this, weight, update);
	if(update) this->update();
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
//    	QPointF edgeOffset;
//    	if(abs(newline.dx()) > abs(newline.dy())){

    	QPointF sourceOffset = QPointF(0, Node::half_height);
    	QPointF targetOffset = QPointF(0, Node::half_height+Node::footerBarSize);

//    		edgeOffset = QPointF((20/std::fabs(float(newline.dx())))*newline.dx(), (20/std::fabs(float(newline.dx())))*newline.dy());
//    	} else {
//    		edgeOffset = QPointF((20/std::fabs(float(newline.dy())))*newline.dx(), (20/std::fabs(float(newline.dy())))*newline.dy());
//    	}

        sourcePoint = newline.p1() - sourceOffset;
        destPoint = newline.p2() + targetOffset;
    } else {
        sourcePoint = destPoint = newline.p1();
    }

    _line = QLineF(sourcePoint, destPoint);
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
	bool drawBackground= false;
	if (!source || !dest)
		return;

	QPen pen;
	QBrush brush;

	QLineF line(sourcePoint, destPoint);
	if (qFuzzyCompare(line.length(), qreal(0.)))
		return;
	//! [4]

	     //! [5]
	// Draw the line itself


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

	switch(graphWidget->getWindow()->labelMode){
		case Window::onlyLabels:
			if(!label->isDeleted()){
				lineColor = label->getColor();
			} else {
				lineColor = connectionColor;
			}
			backgroundColor = QColor(150,150,150);
		break;
		case(Window::onlyConnectionSign):
			lineColor = connectionColor;
			backgroundColor = QColor(150,150,150);
		break;
		case(Window::both):
			lineColor = connectionColor;
			if(!label->isDeleted()){
				backgroundColor = label->getColor();
				drawBackground = true;
			} else {
				backgroundColor = QColor(150,150,150);
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
	} else if (this->isSelected()){
		if(lineColor.value() < 10){
			lineColor.setHsv(lineColor.saturation(), lineColor.saturation(), lineColor.value()+40);
		}
		lineColor = lineColor.lighter();
		backgroundColor = backgroundColor.lighter();
		drawBackground = true;
	}

	//Draw lines
	if(drawBackground){
		pen.setColor(backgroundColor);
		pen.setWidthF(6.0);
		painter->setPen(pen);
		painter->drawLine(_line);
	}

	pen.setColor(lineColor);
	pen.setWidthF(abs(currentWeight)+2.0);
	brush.setColor(lineColor);
	brush.setStyle(Qt::SolidPattern);
	painter->setBrush(brush);
	painter->setPen(pen);
	painter->drawLine(_line);
	//! [5]

//! [6]
    // Draw the arrows
//    double angle = ::acos(line.dx() / line.length());
//    if (line.dy() >= 0)
//        angle = TwoPi - angle;
//
////    QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + Pi / 3) * arrowSize,
////                                                  cos(angle + Pi / 3) * arrowSize);
////    QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
////                                                  cos(angle + Pi - Pi / 3) * arrowSize);
//    QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
//                                              cos(angle - Pi / 3) * arrowSize);
//    QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
//                                              cos(angle - Pi + Pi / 3) * arrowSize);
//
//    painter->setBrush(Qt::black);
////    painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
//    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}

QPainterPath Edge::shape() const
{
  //Thanks to norobro for posting this code at
  //http://www.qtcentre.org/threads/49201-Increase-margin-for-detecting-tooltip-events-of-QGraphicsLineItem
  QPainterPath path;
  QPainterPathStroker stroker;
  path.moveTo(_line.p1());
  path.lineTo(_line.p2());
  stroker.setWidth(m_click_easy_width);
  return stroker.createStroke(path);
}

void Edge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
//    QMenu menu(graphWidget->getWindow());
//    QAction* action = new QAction("Dummy action", graphWidget->getWindow());
//    menu.addAction(action);
//    menu.exec(event->screenPos());
}
//! [6]
