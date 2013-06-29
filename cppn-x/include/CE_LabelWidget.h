/*
 * CE_LabelWidget.h
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

#include "CE_LabelableObject.h"
#include "CE_Defines.h"
#include "CE_Util.h"


class LabelableObject;
class QLabel;
class QPushButton;
class QObject;
class QHBoxLayout;


class LabelWidget : public QWidget{
	Q_OBJECT

public:

	LabelWidget(QString text = "", QColor color = Qt::white, bool isDeleted = true, QWidget *parent = 0);
	virtual ~LabelWidget();

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

	void registerObject(){
		registerdObjects++;
	}

	void unregisterObject(){
		registerdObjects--;

		if(registerdObjects==0){
			delete this;
		}
	}

	bool isDeleted(){
		return deleted;
	}

	void setDeleted(bool _deleted = true){
		deleted = _deleted;
		if(deleted){
			hide();
			colorAction->setShortcut(tr(""));
		} else {
			show();
		}
	}

	void setId(id_t _id){
		id = _id;
		if(id>0){
			QString shortcut = "Alt+" + util::toQString(id);
			colorAction->setShortcut(shortcut);
		}
	}

	id_t getId(){
		return id;
	}

private:
	QLabel* label;
	QPushButton* colorButton;
	QPushButton* deleteButton;
	QHBoxLayout* colorButtonLayout;
	QColor color;
	QAction* colorAction;
	QAction* deleteAction;

	unsigned int registerdObjects;
	id_t id;
	bool deleted;
};

#endif /* CE_COLORBUTTON_H_ */
