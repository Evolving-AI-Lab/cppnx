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
#include <cmath>

#include "CE_Edge.h"
#include "CE_LabelWidget.h"
#include "CX_Exceptions.h"
#include "CX_ModuleColor.h"


const double Edge::m_click_easy_width = 10.0;
size_t Edge::edgesInMemory = 0;

//Default colors
QColor Edge::excitatoryEdgeColor = QColor(0, 150, 0);
QColor Edge::inhibitoryEdgeColor = QColor(150, 0, 0);
QColor Edge::zeroEdgeColor = QColor(150, 150, 150);
QColor Edge::unlabeledEdgeColor = QColor(0, 0, 0);


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
      stepSize(stepSize),
      _useCustomColor(false)
{

    dbg::check_ptr(dbg::warning, sourceNode, DBG_HERE);
    dbg::check_ptr(dbg::warning, destNode, DBG_HERE);
    source = sourceNode;
    dest = destNode;

    init();
    edgesInMemory++;
    dbg::out(dbg::info, "edge") << "Edge created: Edges in memory: " << edgesInMemory << std::endl;
}

Edge::Edge(std::iostream &stream, std::map<std::string, Node*> nodeMap, std::map<std::string, Label*> labelMap):
        LabelableObject(stream, labelMap), _useCustomColor(false)
{
	std::string sourceId;
	std::string targetId;

	stream >> branch;
	stream >> id;
	stream >> sourceId;
	stream >> targetId;
	stream >> currentWeight;
	stream >> originalWeight;
	stream >> bookendStart;
	stream >> bookendEnd;
	stream >> stepSize;

	source = nodeMap[sourceId];
	dest = nodeMap[targetId];
	init();
	edgesInMemory++;
    dbg::out(dbg::info, "edge") << "Edge created: Edges in memory: " << edgesInMemory << std::endl;
}

Edge::~Edge(){
    edgesInMemory--;
    dbg::out(dbg::info, "edge") << "Edge deleted: Edges in memory: " << edgesInMemory << std::endl;
}

void Edge::init(){
//    _point = new Point(this);
    _favourite_index = 0;
	_curveOffset = 0.0;
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

void Edge::setSourceNode(Node* node){
    source = node;
}

void Edge::setTargetNode(Node* node){
    dest = node;
}

void Edge::setWeight(qreal weight, bool update){
	if(weight == currentWeight) return;
	currentWeight = weight;
	emit weightChanged(this, currentWeight, update);
	if(update) this->update();
}

void Edge::adjust(){
    if (!source || !dest)
        return;
    prepareGeometryChange();

    qreal topOffset = source->boundingRect().top();
    qreal bottomOffset = source->boundingRect().bottom();
    _line = QLineF(mapFromItem(source, 0, topOffset), mapFromItem(dest, 0, bottomOffset));

    sourcePoint = _line.p1();
    destPoint = _line.p2();

    qreal offsetY = (std::abs(_line.p1().y() - _line.p2().y()) / 2);
    _curvePoint1 = QPointF(_line.p1().x(), _line.p1().y() - offsetY + _curveOffset);
    _curvePoint2 = QPointF(_line.p2().x(), _line.p2().y() + offsetY + _curveOffset);
}

QRectF Edge::boundingRect() const{
    if (!source || !dest) return QRectF();
    qreal extra = std::abs(_line.p1().y() - _line.p2().y()) / 2 + 10;
    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(), destPoint.y() - sourcePoint.y())).normalized().adjusted(-extra, -extra, extra, extra);
//        .normalized().adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *){
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
		connectionColor = zeroEdgeColor;
	} else if(currentWeight > 0){
		connectionColor = excitatoryEdgeColor;
	} else {
		connectionColor = inhibitoryEdgeColor;
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

//	switch(*labelMode){
//		case onlyLabels:
//			if(hasLabel()){
//				lineColor = label->getColor();
//			} else {
//				lineColor = connectionColor;
//			}
//			backgroundColor = highlightColor;
//		break;
//		case(onlyConnectionSign):
//			lineColor = connectionColor;
//			backgroundColor = highlightColor;
//		break;
//		case(both):
//			lineColor = connectionColor;
//			if(hasLabel()){
//				backgroundColor = label->getColor();
//				drawBackground = true;
//				if (this->isSelected() || partOfContextMenuEvent){
//					backgroundColor = backgroundColor.lighter();
//				}
//			} else {
//				backgroundColor = highlightColor;
//			}
//		break;
//	}

//    QColor Edge::excitatoryEdgeColor = QColor(0, 150, 0);
//    QColor Edge::inhibitoryEdgeColor = QColor(150, 0, 0);
//    QColor Edge::zeroEdgeColor = QColor(150, 150, 150);
//    QColor Edge::unlabeledEdgeColor = QColor(0, 0, 0);

	//Handle sign mode
	backgroundColor = highlightColor;
	switch(*_signMode){
	case noSign:
	    lineColor = unlabeledEdgeColor;
	    break;
	case showSign:
	case showSignIfNoLabel:
	    lineColor = connectionColor;
	    break;
	default:
	    throw ModeException("Unknown edge sign mode: " + util::toString(*_signMode));
	}
//
//    if(*_signMode == noSign){
//        lineColor.setRgb(0.0, 0.0, 0.0); //Produce a black line
//    } else if(*_signMode == showSign || *_signMode == showSignIfNoLabel) {
//        lineColor = connectionColor;
//    } else {
//        //TODO: Throw some sort of error
//    }

	switch(*labelMode){
	case noLabel:
	    break;
	case showLabel:
	    if(hasLabel()){
	        if(*_signMode == showSign){
	            backgroundColor = label->getColor();
	            drawBackground = true;
	            if (this->isSelected() || partOfContextMenuEvent){
	                backgroundColor = backgroundColor.lighter();
	            }
	        } else {
	            lineColor = label->getColor();
	        }
	    }
        break;
	case modules:
		if(source->getModule() == dest->getModule()){
			lineColor = cx_colors::getModuleColor(source->getModule());
		} else {
			lineColor = QColor("black");
		}
		break;
	default:
	    throw ModeException("Unknown edge label mode: " + util::toString(*labelMode));
	}

	if(_useCustomColor){
	    lineColor = _customColor;
	}

//	//Handle label mode
//    if(*labelMode == noLabel){
//        //Do nothing
//    } else if(*labelMode == showLabel){
//        if(*_signMode == showSign && hasLabel()){
//            backgroundColor = label->getColor();
//            drawBackground = true;
//            if (this->isSelected() || partOfContextMenuEvent){
//                backgroundColor = backgroundColor.lighter();
//            }
//        } else if(hasLabel()){
//            lineColor = label->getColor();
//        }
//    } else {
//        //TODO: Throw some sort of error
//    }

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

	//Draw line background
	if(drawBackground){
		pen.setColor(backgroundColor);
		pen.setWidthF(10.0);
		painter->setPen(pen);
		painter->drawPath(path);
	}

	//Determine line width
	switch(*_lineWidthMode){
	case fixedWidth:
	    pen.setWidthF(3.0);
	    break;
	case weightWidth:
	    pen.setWidthF(std::abs(currentWeight)+2.0);
	    break;
	default:
	    throw ModeException("Unknown edge width mode: " + util::toString(*_lineWidthMode));
	}

	//Draw line foreground
	pen.setColor(lineColor);
	painter->setPen(pen);
	painter->drawPath(path);


//	QBrush brush;
//	brush.setColor(lineColor);
//	brush.setStyle(Qt::SolidPattern);
//
//	pen.setColor(Qt::black);
//	painter->setPen(pen);
//	painter->setBrush(lineColor);

	if(label && ((*_annotationMode) == showAnnotation)){
	    QStringList stringList = label->getText().split(":=");
	    if(stringList.size() > 1){
	        //	if(_favourite_index > 0){
	        qreal size = 20;
	        QFont font;
	        font.setPointSize(size*2);
	        font.setStyleHint(QFont::SansSerif);
	        painter->setFont(font);

	        QRect rect;
	        QPointF center = (line.p1() + line.p2())/2;
	        qreal half_width = size;
	        QRect textRect(center.x()-half_width, center.y()-half_width, half_width*2, half_width*2);

	        painter->setPen(Qt::NoPen);
	        painter->setBrush(lineColor);
	        painter->drawEllipse(center, half_width, half_width); //(QRectF(center.x()-half_width, center.y()-half_width, half_width*2, half_width*2), lineColor);

	        painter->setPen(Qt::white);
	        painter->setBrush(Qt::NoBrush);
	        painter->drawText(textRect, Qt::TextSingleLine | Qt::AlignCenter | Qt::NoClip, stringList[1], &rect);

	    }
	}

	//Draw points
//	painter->drawPoint(_curvePoint1);
//	painter->drawPoint(_curvePoint2);

}

QPainterPath Edge::shape() const
{
	QPainterPathStroker stroker;
	stroker.setWidth(m_click_easy_width);
	return stroker.createStroke(getPath());
}

std::ostream& operator<< (std::ostream& os, const Edge& obj) {
    os << obj.getName();
    return os;
}
