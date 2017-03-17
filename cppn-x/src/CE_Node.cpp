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

#include <CX_ComSetPos.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <iostream>
#include <cmath>

#include "CE_Node.h"
#include "CE_FinalNodeView.h"
#include "CE_CppnParser.h"
#include "CE_LabelWidget.h"
#include "CX_Debug.hpp"

//! [0]
Node::Node(
		std::string branch,
		std::string id,
		std::string type,
		std::string activationFunction_str,
		std::string xml_label,
		std::string affinity,
		std::string bias,
		Label* label,
		std::string note
		)
    : LabelableObject(label, note.c_str()),
      branch(branch),
      id(id),
      nodetype(type),
      activationFunction_str(activationFunction_str),
      xml_label(xml_label),
      affinity(affinity),
      bias(bias),
      _useCustomColor(false)

{
    dbg::trace trace("node", DBG_HERE);
	init();
	nodes_in_memory++;
	dbg::out(dbg::info, "node") << "Node created: Nodes in memory: " << nodes_in_memory << std::endl;
}

Node::Node(std::iostream &stream, std::map<std::string, Label*> labelMap):
        LabelableObject(stream, labelMap),
        _useCustomColor(false){
    dbg::trace trace("node", DBG_HERE);
	double x, y;

	stream >> affinity;
	dbg::out(dbg::info, "node") << "Affinity: " << affinity << std::endl;
	stream >> bias;
    dbg::out(dbg::info, "node") << "bias: " << bias << std::endl;
	xml_label = util::readString(stream);
    dbg::out(dbg::info, "node") << "xml_label: " << xml_label << std::endl;
	stream >> nodetype;
    dbg::out(dbg::info, "node") << "nodetype: " << nodetype << std::endl;
	stream >> branch;
    dbg::out(dbg::info, "node") << "branch: " << branch << std::endl;
	stream >> id;
    dbg::out(dbg::info, "node") << "id: " << id << std::endl;
	stream >> activationFunction_str;
    dbg::out(dbg::info, "node") << "activationFunction_str: " << activationFunction_str << std::endl;
	stream >> x;
    dbg::out(dbg::info, "node") << "x: " << x << std::endl;
	stream >> y;
    dbg::out(dbg::info, "node") << "y: " << y << std::endl;

	setPos(x, y);
	init();
	nodes_in_memory++;
	dbg::out(dbg::info, "node") << "Node created: Nodes in memory: " << nodes_in_memory << std::endl;
}

Node::~Node(){
    dbg::trace trace("node", DBG_HERE);
    nodes_in_memory--;
    dbg::out(dbg::info, "node") << "Node deleted: Nodes in memory: " << nodes_in_memory << std::endl;
	delete (pixels);
}

void Node::init(){
    dbg::trace trace("node", DBG_HERE);
	depth = 0;
	index = 0;

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(1);

    //Determine activation function
    setActivationFunction(activationFunction_str);

	//Determine type
    if(nodetype == XML_TYPE_INPUT){
        setNodeType(input);
    } else if(nodetype == XML_TYPE_OUTPUT){
        setNodeType(output);
    } else {
        setNodeType(hidden);
    }

    pixels = new QImage(256, 256, QImage::Format_RGB32);
    pixels->fill(0);
    labelMode = 0;
    module = 0;

}

void Node::setActivationFunction(const std::string& xmlActivationFunction){
    dbg::trace trace("node", DBG_HERE);
    activationFunction_str = xmlActivationFunction;
    act_functions::ActivationFunctionInfo actInfo = act_functions::getActivationFunction(xmlActivationFunction);
    activationFunction = actInfo.activationFunction;
    activationFunction_short = actInfo.shortName;

//    if(activationFunction_str == XML_GAUSSIAN){
//        activationFunction = act_functions::gaussian;
//        activationFunction_short = "gau()";
//    } else if(activationFunction_str == XML_LINEAR){
//        activationFunction = act_functions::identity;
//        activationFunction_short = "lin()";
//    } else if(activationFunction_str == XML_SIN){
//        activationFunction = act_functions::sin;
//        activationFunction_short = "sin()";
//    } else if(activationFunction_str == XML_SIGMOID){
//        activationFunction = act_functions::sigmoid;
//        activationFunction_short = "sig()";
//    } else if(activationFunction_str == XML_COS){
//        activationFunction = act_functions::cos;
//        activationFunction_short = "cos()";
//    } else if(activationFunction_str == XML_STEP){
//        activationFunction = act_functions::ustep;
//        activationFunction_short = "ustep()";
//    } else if(activationFunction_str == XML_U_SIGMOID){
//        activationFunction = act_functions::uSigmoid;
//        activationFunction_short = "usig()";
//    } else if(activationFunction_str == XML_U_GUASSIAN){
//        activationFunction = act_functions::uGaussian;
//        activationFunction_short = "ugau()";
//    } else {
//        throw CeParseException("File contains unknown activation function: '" + activationFunction_str + "'");
//    }
}

void Node::addIncommingEdge(Edge *edge)
{
    dbg::trace trace("node", DBG_HERE);
	incomingEdgeList << edge;
    edge->adjust();
}
void Node::addOutgoingEdge(Edge *edge)
{
    dbg::trace trace("node", DBG_HERE);
	outgoingEdgeList << edge;
    edge->adjust();
}

void Node::removeIncommingEdge(Edge *edge){
    dbg::trace trace("node", DBG_HERE);
	incomingEdgeList.removeAll(edge);
}

void Node::removeOutgoingEdge(Edge *edge){
    dbg::trace trace("node", DBG_HERE);
	outgoingEdgeList.removeAll(edge);
}


const QList<Edge *>& Node::incomingEdges() const
{
    dbg::trace trace("node", DBG_HERE);
    return incomingEdgeList;
}
const QList<Edge *>& Node::outgoingEdges() const
{
    dbg::trace trace("node", DBG_HERE);
    return outgoingEdgeList;
}


QRectF Node::boundingRect() const
{
    dbg::trace trace("node", DBG_HERE);
    qreal adjust = 0;
    return QRectF( -half_width - adjust,
            (-half_height - adjust) - footerBarSize,
            node_width + 2*adjust,
            node_height + 2*adjust + footerBarSize*2);
}

QPainterPath Node::shape() const
{
    dbg::trace trace("node", DBG_HERE);
    QPainterPath path;

    path.addRect(-half_width, -half_height, node_width, node_height);

    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	Q_UNUSED(option);

	dbg::trace trace("node", DBG_HERE);
	QColor labelColor;
//	std::cout << "Label address: " << (void*) label << std::endl;

	if(labelMode){
		if((*labelMode) == modules){
			labelColor = QColor();
			labelColor.setHsv(module*100+100, 255, 255);
		}else if(hasLabel()){
		    labelColor = label->getColor();
		} else {
		    labelColor= Qt::white;
		}
	} else {
		labelColor= Qt::white;
	}

    if(_useCustomColor){
        labelColor = _customColor;
    }

	QColor normalBorderColor;

    if(nodetype == XML_TYPE_OUTPUT){
    	normalBorderColor = QColor(Qt::darkYellow);
    } else {
    	normalBorderColor = QColor(Qt::black);
    }

    if(labelMode){
        switch((*labelMode)){
        case labels:
            painter->drawImage(QRect(-half_width, -half_height, node_width, node_height), *pixels);
            break;
        case labelsNoImage:
            painter->fillRect(QRect(-half_width, -half_height, node_width, node_height), labelColor);
            break;
        case modules:
            painter->fillRect(QRect(-half_width, -half_height, node_width, node_height), labelColor);
            break;
        default:
            throw LabelModeException("Unknown mode: " + util::toString(*labelMode));
            break;
        }
    } else {
        painter->fillRect(QRect(-half_width, -half_height, node_width, node_height), labelColor);
    }


	QRect footer(-half_width, half_height, node_width, footerBarSize);
	QRect header(-half_width, -half_height-headerBarSize, node_width, headerBarSize);
	QRect border(-half_width, -half_height - headerBarSize, node_width, node_height + footerBarSize+headerBarSize);

    painter->fillRect(footer, labelColor);
    painter->fillRect(header, labelColor);

    painter->setPen(QPen(normalBorderColor, 0));
    painter->drawRect(footer);
    painter->drawRect(header);

    if (partOfContextMenuEvent){
    	painter->setPen(QPen(CONTEXT_EVENT_COLOR, 2));
    }else if (this->isSelected()){
    	if (*parentHasFocus){
    		painter->setPen(QPen(SELECTED_COLOR, 2));
    	}else{
    		painter->setPen(QPen(NO_FOCUS_SELECTED_COLOR, 2));
    	}
    }else{
    	painter->setPen(QPen(normalBorderColor, 0));
    }

    painter->drawRect(border);

    if(labelColor.lightness() < 128){
    	painter->setPen(QPen(Qt::white, 0));
    } else {
    	painter->setPen(QPen(Qt::black, 0));
    }

    QRect rect;
    QFont font;

    font.setPointSize(6);
    font.setStyleHint(QFont::SansSerif);

    painter->setFont(font);
    painter->drawText(header, Qt::TextSingleLine | Qt::AlignCenter | Qt::NoClip, util::toQString(branch + "_" + id), &rect);
    painter->drawText(footer.adjusted(2, 0, 0, 0) , Qt::TextSingleLine | Qt::AlignLeft| Qt::NoClip,
            QString("d:") + util::toQString(depth), &rect);
    painter->drawText(footer.adjusted(0, 0, -2, 0), Qt::TextSingleLine | Qt::AlignRight| Qt::NoClip,
            util::toQString(activationFunction_short), &rect);
    painter->drawText(footer.adjusted(18, 0, 0, 0), Qt::TextSingleLine | Qt::AlignLeft| Qt::NoClip,
            util::toQString(xml_label.substr(0,1)).toUpper() , &rect);

}


void Node::updateAll(){
    dbg::trace trace("node", DBG_HERE);
	update();
	emit imageChanged();
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    dbg::trace trace("node", DBG_HERE);
    switch (change) {
    case ItemPositionHasChanged:
        foreach (Edge *edge, incomingEdgeList) edge->adjust();
        foreach (Edge *edge, outgoingEdgeList) edge->adjust();
        break;
    default:
        break;
    };

    return LabelableObject::itemChange(change, value);
}



//void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//    update();
//    QGraphicsItem::mousePressEvent(event);
//}


void Node::setPixel(size_t index, const double& value){
    dbg::trace trace("node", DBG_HERE);
	size_t localindex = index*4;
	//Grey does not use the min() function to prevent a bug on windows.
	unsigned char red = 255;
	if(std::abs(value) < 1.0) red = std::abs(value)*255;
	unsigned char green(std::min(std::max(value, 0.0), 1.0)*255);
	unsigned char blue(std::min(std::max(value, 0.0), 1.0)*255);
	if(value > 1.0) blue = (1-std::min(std::max(((std::abs(value)-1)/5), 0.0), 1.0))*255;
	if(value < -1.0) blue = std::min(std::max(((std::abs(value)-1)/5), 0.0), 1.0)*255;
	pixels->bits()[localindex]=blue;
	pixels->bits()[localindex+1]=green;
	pixels->bits()[localindex+2]=red;
}


void Node::redraw(){
    dbg::trace trace("node", DBG_HERE);
	emit updateRequest(this);
}

std::ostream& operator<< (std::ostream& os, const Node& obj){
    os << obj.getName();
    return os;
}


size_t Node::nodes_in_memory = 0;

