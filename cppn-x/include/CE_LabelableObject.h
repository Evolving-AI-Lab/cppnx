/*
 * CE_LabelableObject.h
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#ifndef CE_LABELABLEOBJECT_H_
#define CE_LABELABLEOBJECT_H_

#include "CE_LabelWidget.h"
#include <QGraphicsItem>

class LabelWidget;

class LabelableObject : public QGraphicsItem {
public:
	LabelableObject(LabelWidget* label = 0, QString note = "");

	virtual ~LabelableObject();

	LabelWidget* getLabel();
	void setLabel(LabelWidget* _label);

	QString getNote(){
		return note;
	}

	void setNote(QString _note){
		note = _note;
	}

protected:
	LabelWidget* label;
	QString note;
};

#endif /* CE_LABELABLEOBJECT_H_ */
