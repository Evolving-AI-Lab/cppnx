/*
 * CX_Legend.hpp
 *
 *  Created on: Apr 24, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_LEGEND_HPP_
#define CPPN_X_INCLUDE_CX_LEGEND_HPP_

//Qt includes
#include <QPainter>

//Local includes
#include "CX_MovableObject.hpp"
#include "CE_Label.h"

class Legend : public MovableObject {
    Q_OBJECT

public:
    static const qreal lineHeight;
    static const qreal boxWidth;
    static const qreal boxHeight;
    static const qreal boxTopMargin;
    static const qreal leftMargin;
    static const qreal rightMargin;
    static const qreal topMargin;
    static const qreal bottomMargin;
    static const qreal boxToTextMargin;
    static const qreal maxWidth;
//    static const qreal textVerticalOffset = 4;

    Legend(QGraphicsItem * parent = 0);

    void setLabels(const QList<Label*>& labels);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


public slots:

protected:
    void _setFont(QPainter *painter);

    qreal _width;
    qreal _height;
    QList<Label*> _labels;
};




#endif /* CPPN_X_INCLUDE_CX_LEGEND_HPP_ */
