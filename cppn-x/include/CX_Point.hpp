/*
 * CX_Point.hpp
 *
 *  Created on: Apr 22, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_POINT_HPP_
#define CPPN_X_INCLUDE_CX_POINT_HPP_

//Qt includes
#include <QPainter>

//Local includes
#include "CX_SelectableObject.h"

class Point : public SelectableObject {
    Q_OBJECT

public:
    Point(QGraphicsItem * parent = 0);

//    Point(Label* label = 0, QString note = "");
//    Point(std::iostream &stream, std::map<std::string, Label*> labelMap);
//    virtual ~Point();
//
//    void init();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


public slots:

protected:
    qreal _size;

};



#endif /* CPPN_X_INCLUDE_CX_POINT_HPP_ */
