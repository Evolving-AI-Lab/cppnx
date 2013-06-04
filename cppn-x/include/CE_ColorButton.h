/*
 * CE_ColorButton.h
 *
 *  Created on: Jun 3, 2013
 *      Author: joost
 */

#ifndef CE_COLORBUTTON_H_
#define CE_COLORBUTTON_H_

#include <QObject>
#include <QLabel>
#include <QPushButton>
#include <QPainter>

class QLabel;
class QPushButton;
class QObject;

class CE_ColorButton : public QObject{
	Q_OBJECT

public:
	CE_ColorButton(QLabel* label, QPushButton* colorButton, QPushButton* deleteButton, QColor color, QObject *parent = 0);
	CE_ColorButton(QString text, QColor color, QObject *parent = 0);
	virtual ~CE_ColorButton();

	QLabel* getLabel(){
		return label;
	}
	QPushButton* getColorButton(){
		return colorButton;
	}
	QPushButton* getDeleteButton(){
		return deleteButton;
	}

	QColor getColor(){
		return color;
	}

	std::string getText();

private:
	QLabel* label;
	QPushButton* colorButton;
	QPushButton* deleteButton;
	QColor color;
};

#endif /* CE_COLORBUTTON_H_ */
