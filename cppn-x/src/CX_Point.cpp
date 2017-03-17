/*
 * CX_Point.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: Joost Huizinga
 */
#include "CX_Point.hpp"

Point::Point(QGraphicsItem * parent):
SelectableObject(parent), _size(50){
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
}

QPainterPath Point::shape() const
{
//    std::cout << "Shape" << std::endl;
    QPainterPath path;

//    path.addEllipse(pos().x()-_size,pos().y()-_size,_size*2, _size*2);
    path.addRect(boundingRect());

    return path;
}

QRectF Point::boundingRect() const
{
//    std::cout << "Bounding box" << std::endl;
    return QRectF(-_size,-_size, _size*2, _size*2);
}

void Point::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
//    std::cout << "Here" << std::endl;
    Q_UNUSED(option);
//    painter->setPen(QPen(Qt::black, _size));
//    painter->drawRect(boundingRect());
    painter->fillRect(boundingRect(), Qt::black);
//    painter->drawPoint(pos());
}
