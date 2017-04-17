/*
 * CE_NodeView.cpp
 *
 *  Created on: Jun 5, 2013
 *      Author: joost
 */

#include "CE_NodeView.h"
#include "CE_Node.h"
#include "CE_Cppn.h"
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
    resizeNode();

    if(node){
    	_pixels = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
		connect(node, SIGNAL(removed()), this, SLOT(removeImage()));
		connect(node, SIGNAL(added()), this, SLOT(addImage()));
    } else {
    	_pixels = 0;
    }
    compatibillityId = 1;
}

void NodeView::resizeNode(){
	_node_width = IMAGE_WIDTH;
	_node_height = IMAGE_HEIGHT;
    _half_width = _node_width/2;
	_half_height = _node_height/2;
    setIconSize(QSize(_node_width, _node_height));
    setIconTranslate(QPoint(_half_width, _half_height));
}

QRectF NodeView::boundingRect() const {
    dbg::trace trace("nodeview", DBG_HERE);
    qreal adjust = 2;
    return QRectF( -_half_width - adjust, (-_half_height - adjust) , _node_width + 2*adjust, _node_height + 2*adjust);
}

QPainterPath NodeView::shape() const {
    dbg::trace trace("nodeview", DBG_HERE);
    QPainterPath path;
    path.addRect(-_half_width, -_half_height, _node_width, _node_height);
    return path;
}

void NodeView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
    dbg::trace trace("nodeview", DBG_HERE);
	Q_UNUSED(option);

	if(_pixels){
	    painter->drawImage(QRect(-_half_width, -_half_height, _node_width, _node_height), *_pixels);
	} else {
	    painter->fillRect(QRect(-_half_width, -_half_height, _node_width, _node_height), Qt::black);
	}

    if (partOfContextMenuEvent){
    	painter->setPen(QPen(CONTEXT_EVENT_COLOR, 2));
    	painter->drawRect(QRect(-_half_width, -_half_height, _node_width, _node_height ));
    } else if (this->isSelected()){
    	QColor highlightColor;
		if(*parentHasFocus){
			highlightColor = QColor(SELECTED_COLOR);
		} else {
			highlightColor = QColor(NO_FOCUS_SELECTED_COLOR);
		}
    	painter->setPen(QPen(highlightColor, 2));
    	painter->drawRect(QRect(-_half_width, -_half_height, _node_width, _node_height ));
    }else{
    	painter->setPen(QPen(Qt::black, 0));
    	painter->drawRect(QRect(-_half_width, -_half_height, _node_width, _node_height ));
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
	_pixels = 0;
}

void NodeView::addImage(){
    if(node){
    	_pixels = node->getImage();
    }
}
