/*
 * CE_Label.cpp
 *
 *  Created on: Jun 3, 2013
 *      Author: joost
 */

#include "CE_Label.h"


#include <QMenu>
#include <QContextMenuEvent>
#include <QGraphicsProxyWidget>


Label::Label(QString text, QColor color, bool isDeleted):
color(color){
	deleted = isDeleted;
	labelName = text;
	init();
}

Label::Label(std::iostream &stream){
	std::string text;
	int r;
	int g;
	int b;

//	stream >> colorStr;
	text = util::readString(stream);
	stream >> r;
	stream >> g;
	stream >> b;

	labelName = QString(text.c_str());
	color = QColor(r,g,b);
	deleted = false;

	init();
}

void Label::init(){
	highlight=false;
	id=0;
	registerdObjects=0;


	//Create actions
	colorAction = new QAction(labelName, this);
	changeColorAction = new QAction(tr("Change color..."), this);
	renameAction = new QAction(tr("Rename..."), this);

	//Create colored button
	colorButton = new QPushButton();
    QString qss = QString("background-color: %1").arg(color.name());
    colorButton->setStyleSheet(qss);
    colorButton->setFixedSize(30, 30);

    //Add buttons to this widget
    colorButtonLayout = new QGraphicsLinearLayout();
    QGraphicsProxyWidget *buttonProxy = new QGraphicsProxyWidget();
    buttonProxy->setWidget(colorButton);
    colorButtonLayout->addItem(buttonProxy);
    colorButtonLayout->setAlignment(buttonProxy, Qt::AlignRight | Qt::AlignVCenter);
    colorButtonLayout->setMaximumWidth(200);

    setLayout(colorButtonLayout);

    connect(colorButton, SIGNAL(clicked()), colorAction, SLOT(trigger()));
    connect(changeColorAction, SIGNAL(triggered()), this, SLOT(changeLabelColor()));
    connect(renameAction, SIGNAL(triggered()), this, SLOT(changeLabelName()));
    connect(colorAction, SIGNAL(triggered()), this, SLOT(applyLabel()));

    setAutoFillBackground(true);

    QMenu* menu = new QMenu("label");
    menu->addAction(renameAction);
    menu->addAction(changeColorAction);
    this->setContextMenu(menu);

    //Set Size
	boundingBox = QRectF(0, 0, 200, 50);
	setIconSize(QSize(200, 50));
	setPreferredSize(200, 50);
}

Label::~Label() {
	delete(colorButton);
}


void Label::setColor(QColor _color){
	color = _color;
    QString qss = QString("background-color: %1").arg(color.name());
    colorButton->setStyleSheet(qss);
    emit labelChanged();
//    window->getGraphWidget()->updateAll();
}

void Label::changeLabelColor(){
	CommandChangeLabelColor* command = new CommandChangeLabelColor(this);
	if(command->isOk()) emit requestCommandExecution(command);
}

void Label::changeLabelName(){
	CommandChangeLabelName* command = new CommandChangeLabelName(this);
//	std::cout << command->isOk() << std::endl;
	if(command->isOk()) emit requestCommandExecution(command);
}

void Label::setHighlightOn(){
	highlight = true;
	update();
//	if(hasFocus()){
////		setStyleSheet(selectedHighlightSs);
//	} else {
////    	setStyleSheet(highlightSs);
//    }
}

void Label::setHighlightOff(){
	highlight = false;
	update();
//	if(hasFocus()){
////		setStyleSheet(selectedSs);
//	} else {
////    	setStyleSheet(baseSs);
//    }
}

QRectF Label::boundingRect() const
{
//    qreal adjust = 2;
    return boundingBox;
}

QPainterPath Label::shape() const
{
    QPainterPath path;
    path.addRect(boundingBox);
    return path;
}

void Label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	Q_UNUSED(option);
	if(highlight){
		painter->fillRect(boundingBox, palette().color(QPalette::Highlight));
	} else {
		painter->fillRect(boundingBox, palette().color(QPalette::Window));
	}
//	painter->drawText(QPoint(10,30), labelName);

	if(shortText.isEmpty()){
		shortText = labelName;
		QString dots = "";
		QRect rect;
		QPixmap tempPixmap(200, 50);
		QPainter tempPainter(&tempPixmap);

		tempPainter.drawText (10, 20, 140, 16, Qt::TextSingleLine | Qt::AlignLeft, shortText, &rect);
//		std::cout << rect.width() << " " << rect.height()  << std::endl;
		while(rect.width() > 140){
			dots = "...";
			shortText.resize(shortText.size()-1);
			tempPainter.drawText (10, 20, 140, 16, Qt::TextSingleLine | Qt::AlignLeft, shortText + dots, &rect);
//			std::cout << rect.width() << " " << rect.height()  << std::endl;
		}
		shortText += dots;
	}

//	QRect* rect = new QRect;
	painter->drawText (10, 20, 140, 16, Qt::TextSingleLine | Qt::AlignLeft, shortText);



    if (partOfContextMenuEvent){
    	painter->setPen(QPen(CONTEXT_EVENT_COLOR, 2));
    } else if (this->isSelected()){
    	painter->setPen(QPen(SELECTED_COLOR, 2));
    }else{
    	painter->setPen(QPen(Qt::black, 0));
    }

    painter->drawRect(boundingBox);

}

void Label::applyLabel(){
	emit applyLabel(this);
}

void Label::deleteLabel(){
	emit deleteLabel(this);
}
