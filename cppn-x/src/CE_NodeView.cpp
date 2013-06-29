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

NodeView::NodeView(Node* node, int width, int height): node(node) {
    setFlag(ItemIsSelectable);
    setCacheMode(DeviceCoordinateCache);
    if(node) node->setNodeView(this);

    QImage* pixels = new QImage(width, height, QImage::Format_RGB32);
    pixels->fill(0);
    setPixels(pixels);
}

//NodeView::NodeView(int width, int height): node(0) {
//	setFlag(ItemIsSelectable);
//    setCacheMode(DeviceCoordinateCache);
//
//    QImage* pixels = new QImage(width, height, QImage::Format_RGB32);
//    pixels->fill(0);
//    setPixels(pixels);
//}

NodeView::~NodeView() {
	delete (pixels);
}



//! [1]

//! [8]
QRectF NodeView::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -half_width - adjust, (-half_height - adjust) , node_width + 2*adjust, node_height + 2*adjust);
}
//! [8]

//! [9]
QPainterPath NodeView::shape() const
{
    QPainterPath path;
    path.addRect(-half_width, -half_height, node_width, node_height);
    return path;
}
//! [9]

//! [10]
void NodeView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	Q_UNUSED(option);
//    painter->setPen(Qt::NoPen);
//    painter->setBrush(Qt::darkGray);
//    painter->drawEllipse(-7, -7, 20, 20);
//
//    QRadialGradient gradient(-3, -3, 10);
//    if (option->state & QStyle::State_Sunken) {
//        gradient.setCenter(3, 3);
//        gradient.setFocalPoint(3, 3);
//        gradient.setColorAt(1, QColor(Qt::yellow).light(120));
//        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
//    } else {
        //gradient.setColorAt(0, Qt::yellow);
        //gradient.setColorAt(1, Qt::darkYellow);
//    }
    //painter->setBrush(gradient);




    //std::cout << pixels << std::endl;
    //painter->drawEllipse(-10, -10, 20, 20);
   // std::cout << "Painting" << std::endl;

//	painter->setRenderHint(QPainter::Antialiasing);
    painter->drawImage(QRect(-half_width, -half_height, node_width, node_height), *pixels);

    if (this->isSelected()){
    	painter->setPen(QPen(Qt::black, 2));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height ));
    }else{
    	painter->setPen(QPen(Qt::black, 0));
    	painter->drawRect(QRect(-half_width, -half_height, node_width, node_height ));
    }

}
//! [10]


//! [12]
void NodeView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void NodeView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
//! [12]

//! [13]
void NodeView::setPixels(QImage* pixels_)
{
    pixels = pixels_;
}
//! [13]

void NodeView::setPixel(int x, int y, char r, char g, char b){
	pixels->setPixel(x, y, qRgb(r, g, b));
}

void NodeView::setPixel(size_t index, char r, char g, char b ){
	index = index*4;
	pixels->bits()[index]=r;
	pixels->bits()[index+1]=g;
	pixels->bits()[index+2]=b;
}

void NodeView::setPixel(const size_t& index, const char& grey){
	pixels->bits()[index]=grey;
	pixels->bits()[index+1]=grey;
	pixels->bits()[index+2]=grey;
}

void NodeView::setPixel(const size_t& index, const double& value){
	char grey(std::min(std::abs(value), 1.0)*255);
	char alt(std::min(std::max(value, 0.0), 1.0)*255);
	pixels->bits()[index]=alt;
	pixels->bits()[index+1]=alt;
	pixels->bits()[index+2]=grey;
}

void NodeView::setPixel(const size_t& index, const char& red, const char& grey){
	pixels->bits()[index]=grey;
	pixels->bits()[index+1]=grey;
	pixels->bits()[index+2]=red;
}
