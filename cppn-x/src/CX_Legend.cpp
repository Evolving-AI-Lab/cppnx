/*
 * CX_Legend.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: Joost Huizinga
 */

#include "CX_Legend.hpp"

const qreal Legend::lineHeight = 40;
const qreal Legend::boxWidth = 30;
const qreal Legend::boxHeight = 30;
const qreal Legend::boxTopMargin = 1;
const qreal Legend::leftMargin = 5;
const qreal Legend::rightMargin = 5;
const qreal Legend::topMargin = 5;
const qreal Legend::bottomMargin = 0;
const qreal Legend::boxToTextMargin = 5;
const qreal Legend::maxWidth = 2000;

Legend::Legend(QGraphicsItem * parent):
        MovableObject(parent){
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
}

void Legend::setLabels(const QList<Label*>& labels){
    _width = 0;
    _height = 0; //topMargin + bottomMargin;

    //I am just writing to this image because I do not actually want to write anything
    //I just want to know how big something is IFF it would have been painted somewhere.
    QPixmap dummy(maxWidth, lineHeight);
    QPainter painter(&dummy);
    QRectF rect;
    QRectF inputRect(0,0, maxWidth, lineHeight);
    _setFont(&painter);
    int r, g, b;


    QSet<int> colorSet;
    foreach(Label* label, labels){
        label->getColor().getRgb(&r, &g, &b);
        int hash = r + (g << 8) + (b << 16);

        if(!colorSet.contains(hash)){
            colorSet.insert(hash);
            _labels.append(label);
            painter.drawText(inputRect, Qt::TextSingleLine | Qt::AlignCenter | Qt::AlignBottom | Qt::NoClip, label->getText(), &rect);
            if(rect.width() > _width){
                _width = rect.width();
            }
            _height += rect.height();
        }
    }
    update();
//    std::cout << "labels set " << _width << " " << _height << std::endl;
}

QPainterPath Legend::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

QRectF Legend::boundingRect() const
{
//    std::cout << "Bounding box" << std::endl;
    return QRectF(0, 0, leftMargin + boxWidth + boxToTextMargin + _width + rightMargin, _height);
}

void Legend::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
//    std::cout << "paint " << _width << " " << _height << std::endl;
    Q_UNUSED(option);
//    qreal margin = 5;
//    qreal textVerticalOffset = 4;
//    qreal size = 25;
//    qreal maxWidth = 2000;
//    QFont font;
    QRectF rect;
    QRectF inputRect(leftMargin + boxWidth + boxToTextMargin, topMargin, _width, lineHeight+1);
    QRectF colorRect(leftMargin, topMargin + boxTopMargin, boxWidth, boxHeight);
    QPen pen;
//    pen.setWidth(50);
    pen.setColor(Qt::black);
    painter->setPen(pen);
//    font.setPointSize(size);
//    font.setStyleHint(QFont::SansSerif);
//    painter->setFont(font);

    _setFont(painter);

    foreach(Label* label, _labels){
//        std::cout << "Painted " <<label->getText().toStdString() <<  std::endl;
        painter->drawText(inputRect, Qt::TextSingleLine | Qt::AlignLeft | Qt::AlignBottom | Qt::NoClip, label->getText(), &rect);
        painter->fillRect(colorRect, label->getColor());

//        inputRect.adjust(0, 5, 0, 0);
        inputRect.translate(0, rect.height());
        colorRect.translate(0, rect.height());
    }

//    painter->setPen(QPen(Qt::black));
    painter->setPen(getSelectedPen());
    painter->drawRect(boundingRect());

//    painter->fillRect(boundingRect(), Qt::black);
//    painter->drawPoint(pos());
}

void Legend::_setFont(QPainter *painter){
    QFont font;
    font.setPointSize(lineHeight);
    font.setStyleHint(QFont::SansSerif);
    painter->setFont(font);
}
