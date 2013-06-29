/*
 * CE_Label.cpp
 *
 *  Created on: Jun 3, 2013
 *      Author: joost
 */

#include "CE_LabelWidget.h"

LabelWidget::LabelWidget(QString text, QColor color, bool isDeleted, QWidget *parent):
		QWidget(parent), color(color){
	id=0;
	registerdObjects=0;
	deleted = isDeleted;
	//Create actions
	colorAction = new QAction(text, this);
	deleteAction = new QAction(tr("Delete ") + text, this);

	//Create label
	label = new QLabel(text);

	//Create colored button
	colorButton = new QPushButton();
    QString qss = QString("background-color: %1").arg(color.name());
    colorButton->setStyleSheet(qss);
    colorButton->setFixedSize(30, 30);

    //Create delete button
    QPixmap pixmap(10, 10);
    QColor trans = QColor(0, 0, 0, 0);
    pixmap.fill(trans);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.drawLine (2,2, 8,8);
    painter.drawLine (2,8, 8,2);
    deleteButton = new QPushButton();
    deleteButton->setIcon(pixmap);
    deleteButton->setFixedSize(10, 10);

    //Add buttons to this widget
    colorButtonLayout = new QHBoxLayout;
    colorButtonLayout->addWidget(label);
    colorButtonLayout->addWidget(colorButton);
    colorButtonLayout->addWidget(deleteButton);
    setLayout(colorButtonLayout);

    connect(colorButton, SIGNAL(clicked()), colorAction, SLOT(trigger()));
    connect(deleteButton, SIGNAL(clicked()), deleteAction, SLOT(trigger()));
}


LabelWidget::~LabelWidget() {
	delete(label);
	delete(colorButton);
	delete(deleteButton);

}

std::string LabelWidget::getText(){
	return label->text().toStdString();
}
