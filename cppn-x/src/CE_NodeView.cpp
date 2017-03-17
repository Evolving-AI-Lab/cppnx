/*
 * CE_NodeView.cpp
 *
 *  Created on: Jun 5, 2013
 *      Author: joost
 */

#include "CE_NodeView.h"
#include "CE_Node.h"
#include <QPainter>
#include <cmath>

NodeView::NodeView(Node* node): node(node){
    dbg::trace trace("nodeview", DBG_HERE);
	init();
}

NodeView::NodeView(std::iostream &stream, std::map<std::string, Node*> nodeMap){
    dbg::trace trace("nodeview", DBG_HERE);
	std::string branch;
	std::string id;

	stream >> branch;
	stream >> id;

	node = nodeMap[branch + "_" + id];

	init();
}

NodeView::~NodeView() {
    dbg::trace trace("nodeview", DBG_HERE);
}


void NodeView::init(){
    dbg::trace trace("nodeview", DBG_HERE);
    setFlag(ItemIsSelectable);
    setCacheMode(DeviceCoordinateCache);

    if(node){
    	pixels = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
		connect(node, SIGNAL(removed()), this, SLOT(removeImage()));
		connect(node, SIGNAL(added()), this, SLOT(addImage()));
    } else {
        pixels = 0;
    }
    compatibillityId = 1;

    setIconSize(QSize(node_width, node_height));
    setIconTranslate(QPoint(half_width, half_height));
}

QRectF NodeView::boundingRect() const {
    dbg::trace trace("nodeview", DBG_HERE);
    qreal adjust = 2;
    return QRectF( -half_width - adjust, (-half_height - adjust) , node_width + 2*adjust, node_height + 2*adjust);
}

QPainterPath NodeView::shape() const {
    dbg::trace trace("nodeview", DBG_HERE);
    QPainterPath path;
    path.addRect(-half_width, -half_height, node_width, node_height);
    return path;
}

void NodeView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
    dbg::trace trace("nodeview", DBG_HERE);
	Q_UNUSED(option);

	if(pixels){
	    painter->drawImage(QRect(-half_width, -half_height, node_width, node_height), *pixels);
	} else {
	    painter->fillRect(QRect(-half_width, -half_height, node_width, node_height), Qt::black);
	}

    if (partOfContextMenuEvent){
    	painter->setPen(QPen(CONTEXT_EVENT_COLOR, 2));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height ));
    } else if (this->isSelected()){
    	QColor highlightColor;
		if(*parentHasFocus){
			highlightColor = QColor(SELECTED_COLOR);
		} else {
			highlightColor = QColor(NO_FOCUS_SELECTED_COLOR);
		}
    	painter->setPen(QPen(highlightColor, 2));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height ));
    }else{
    	painter->setPen(QPen(Qt::black, 0));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height ));
    }

}

void NodeView::update(){
    dbg::trace trace("nodeview", DBG_HERE);
	DragAndDropObject::update();
}

std::string NodeView::getNodeBranch() const{
    dbg::trace trace("nodeview", DBG_HERE);
    if(node){
        return node->getBranch();
    } else {
        return "";
    }
}

std::string NodeView::getNodeId() const{
    dbg::trace trace("nodeview", DBG_HERE);
    if(node){
        return node->getId();
    } else {
        return "final";
    }
}

std::string NodeView::getName() const{
    dbg::trace trace("nodeview", DBG_HERE);
    if(getNodeBranch() == ""){
        return getNodeId();
    } else {
        return getNodeBranch() + "_" + getNodeId();
    }
}

void NodeView::removeImage(){
    pixels = 0;
}

void NodeView::addImage(){
    if(node){
        pixels = node->getImage();
    }
}
