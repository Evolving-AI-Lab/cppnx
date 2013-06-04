/*
 * CE_ColorButton.cpp
 *
 *  Created on: Jun 3, 2013
 *      Author: joost
 */

#include "CE_ColorButton.h"

CE_ColorButton::CE_ColorButton(QLabel* label, QPushButton* colorButton, QPushButton* deleteButton, QColor color, QObject *parent):
QObject(parent), label(label), colorButton(colorButton), deleteButton(deleteButton), color(color){


}

CE_ColorButton::CE_ColorButton(QString text, QColor color, QObject *parent):color(color){
	label = new QLabel(text);
	colorButton = new QPushButton;
	deleteButton = new QPushButton;

    QString qss = QString("background-color: %1").arg(color.name());
    colorButton->setStyleSheet(qss);
    colorButton->setFixedSize(30, 30);

    QPixmap pixmap(10, 10);
    QColor trans = QColor(0, 0, 0, 0);
    pixmap.fill(trans);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.drawLine (2,2, 8,8);
    painter.drawLine (2,8, 8,2);
    deleteButton->setIcon(pixmap);
    deleteButton->setFixedSize(10, 10);
}

CE_ColorButton::~CE_ColorButton() {
	delete(label);
	delete(colorButton);
	delete(deleteButton);

}

std::string CE_ColorButton::getText(){
	return label->text().toStdString();
}
