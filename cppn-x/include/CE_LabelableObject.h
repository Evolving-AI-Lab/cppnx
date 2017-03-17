/*
 * CE_LabelableObject.h
 *
 *  Created on: Jun 27, 2013
 *      Author: joost
 */

#ifndef CE_LABELABLEOBJECT_H_
#define CE_LABELABLEOBJECT_H_

#include "CE_Label.h"
#include "CX_MovableObject.hpp"

class Label;

class LabelableObject : public MovableObject {
	Q_OBJECT

public:
	LabelableObject(Label* label = 0, QString note = "");
	LabelableObject(std::iostream &stream, std::map<std::string, Label*> labelMap);

	virtual ~LabelableObject();

    enum { Type = UserType + LABELABLE_OBJECT_TYPE };
    int type() const { return Type; }

	Label* getLabel();
	void setLabel(Label* _label);
	bool hasLabel();
	id_t getLabelId();

	QString getNote(){
		return note;
	}

	void setNote(QString _note){
		note = _note;
	}

	void init();

public slots:
	void onLabelSelected(){
		this->setSelected(true);
	}


protected:
	void makeLabel(Label* _label = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	Label* label;
	QString note;
};

#endif /* CE_LABELABLEOBJECT_H_ */
