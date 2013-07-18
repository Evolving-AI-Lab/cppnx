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
	init();
}

NodeView::NodeView(std::iostream &stream, std::map<std::string, Node*> nodeMap){
	std::string branch;
	std::string id;

	stream >> branch;
	stream >> id;

	node = nodeMap[branch + "_" + id];

	init();
}

NodeView::~NodeView() {
//	delete (pixels);
}


void NodeView::init(){
    setFlag(ItemIsSelectable);
    setCacheMode(DeviceCoordinateCache);
//    setPixels(pixels);
    if(node){
    	pixels = node->getImage();
//    	setDragImage(pixels);

    }
    compatibillityId = 1;

    setIconSize(QSize(node_width, node_height));
    setIconTranslate(QPoint(half_width, half_height));
}

QRectF NodeView::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -half_width - adjust, (-half_height - adjust) , node_width + 2*adjust, node_height + 2*adjust);
}

QPainterPath NodeView::shape() const
{
    QPainterPath path;
    path.addRect(-half_width, -half_height, node_width, node_height);
    return path;
}

void NodeView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	Q_UNUSED(option);

    painter->drawImage(QRect(-half_width, -half_height, node_width, node_height), *pixels);


    if (partOfContextMenuEvent){
    	painter->setPen(QPen(CONTEXT_EVENT_COLOR, 2));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height ));
    } else if (this->isSelected()){
    	painter->setPen(QPen(SELECTED_COLOR, 2));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height ));
    }else{
    	painter->setPen(QPen(Qt::black, 0));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height ));
    }
}

void NodeView::update(){
	DragAndDropObject::update();
}
