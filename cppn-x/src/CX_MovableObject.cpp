/*
 * CX_MovableObject.cpp
 *
 *  Created on: Jun 4, 2015
 *      Author: Joost Huizinga
 */
#include "CX_MovableObject.hpp"

QSet<int> MovableObject::movableTypes;

qreal MovableObject::getSortedValue(AxisEnumerator axisEnumerator) const{
    dbg::trace trace("movableobject", DBG_HERE);
    switch(axisEnumerator){
    case left:
        return pos().x() + this->boundingRect().left()*this->scale();
        break;
    case right:
        return pos().x() + this->boundingRect().right()*this->scale();
        break;
    case top:
        return pos().y() + this->boundingRect().top()*this->scale();
        break;
    case bottom:
        return pos().y() + this->boundingRect().bottom()*this->scale();
        break;
    default:
        throw std::out_of_range ("Unknown axis enumerator" + util::toString(axisEnumerator));
    }
}

void MovableObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    dbg::trace trace("movableobject", DBG_HERE);
    dbg::out(dbg::info, "movableobject") << "(" << pos().x() << "," << pos().y() << ") " << "(" << _previousPosition.x() << "," << _previousPosition.y() << ") " << std::endl;

    update();
    if(pos() != _previousPosition) emit updatePositionsRequest();
    QGraphicsItem::mouseReleaseEvent(event);
}


void MovableObject::updatePosition(){
    dbg::trace trace("movableobject", DBG_HERE);
    _previousPosition = pos();
    emit positionChanged(this);
}

void MovableObject::setPrevPos(QPointF point){
    dbg::trace trace("movableobject", DBG_HERE);
    _previousPosition = point;
}

QPointF MovableObject::getPrevPos(){
    dbg::trace trace("movableobject", DBG_HERE);
    return _previousPosition;
}
