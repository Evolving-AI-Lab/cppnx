/*
 * CX_MovableObject.hpp
 *
 *  Created on: Jun 4, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_MOVABLEOBJECT_HPP_
#define CPPN_X_INCLUDE_CX_MOVABLEOBJECT_HPP_

//Standard includes
#include <stdexcept>

//QT includes
#include <QGraphicsWidget>

//Local includes
#include "CE_Defines.h"
#include "CX_SelectableObject.h"
#include "CE_Util.h"
#include "CX_Debug.hpp"



class MovableObject: public SelectableObject {
    Q_OBJECT
public:
    static QSet<int> movableTypes;

    enum AxisEnumerator {
        left = 0,
        right = 1,
        top = 2,
        bottom = 3,
        nr_of_edges = 4
    };

    size_t sortedIndex[nr_of_edges];

    MovableObject(QGraphicsItem * parent):SelectableObject(parent){}
    MovableObject():SelectableObject(){}
    virtual ~MovableObject(){}

    enum { Type = UserType + MOVABLE_OBJECT_TYPE };
    int type() const { return Type; }


    qreal getSortedValue(AxisEnumerator axisEnumerator) const;
    void setPrevPos(QPointF point);
    QPointF getPrevPos();
    void updatePosition();

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void updatePositionsRequest();
    void positionChanged(MovableObject*);

protected:
    QPointF _previousPosition;
};




#endif /* CPPN_X_INCLUDE_CX_MOVABLEOBJECT_HPP_ */
