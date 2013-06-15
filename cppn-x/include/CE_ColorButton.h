/*
 * CE_ColorButton.h
 *
 *  Created on: Jun 3, 2013
 *      Author: joost
 */

#ifndef CE_COLORBUTTON_H_
#define CE_COLORBUTTON_H_

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QAction>
#include <QHBoxLayout>

class QLabel;
class QPushButton;
class QObject;
class QHBoxLayout;

class CE_ColorButton : public QWidget{
	Q_OBJECT

public:
	CE_ColorButton(QString text, QColor color, QWidget *parent = 0);
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

	QAction* getColorAction(){
		return colorAction;
	}

	QAction* getDeleteAction(){
		return deleteAction;
	}

private:
	QLabel* label;
	QPushButton* colorButton;
	QPushButton* deleteButton;
	QHBoxLayout* colorButtonLayout;
	QColor color;
	QAction* colorAction;
	QAction* deleteAction;
};

#endif /* CE_COLORBUTTON_H_ */
