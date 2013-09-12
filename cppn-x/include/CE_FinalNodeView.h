/*
 * CE_FinalNodeView.h
 *
 *  Created on: Jun 11, 2013
 *      Author: joost
 */

#ifndef CE_FINALNODEVIEW_H_
#define CE_FINALNODEVIEW_H_

#include "CE_NodeView.h"

class FinalNodeView: public NodeView {
	Q_OBJECT
public:
	FinalNodeView();
	virtual ~FinalNodeView();

    enum { Type = UserType + FINALVIEW_TYPE };
    virtual int type() const { return Type; }

	void setHueNode(Node* node){
		hueImage = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
	}

	void setSaturationNode(Node* node){
		saturationImage = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
	}

	void setValueNode(Node* node){
		valueImage = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
	}


public slots:
	void update();

private:
	QImage* hueImage;
	QImage* saturationImage;
	QImage* valueImage;
};

#endif /* CE_FINALNODEVIEW_H_ */
