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

    enum ColorMode { hsv_mode, rgb_mode};

	void setHueNode(Node* node){
		_hueOrRedNode = node;
		colorMode = hsv_mode;
		hueImage = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
		connect(node, SIGNAL(imageReinitialized()), this,
				SLOT(updateHueOrRed()));
		connect(node, SIGNAL(added()), this, SLOT(updateHueOrRed()));
	}

	void setSaturationNode(Node* node){
		_satOrGreenNode = node;
		colorMode = hsv_mode;
		saturationImage = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
		connect(node, SIGNAL(imageReinitialized()), this,
				SLOT(updateSatOrGreen()));
		connect(node, SIGNAL(added()), this, SLOT(updateSatOrGreen()));
	}

	void setValueNode(Node* node){
		_valOrBlueNode = node;
		colorMode = hsv_mode;
		valueImage = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
		connect(node, SIGNAL(imageReinitialized()), this,
				SLOT(resizeNode()));
		connect(node, SIGNAL(added()), this, SLOT(updateValOrBlue()));
	}


	void setRNode(Node* node){
		_hueOrRedNode = node;
		colorMode = rgb_mode;
		hueImage = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
		connect(node, SIGNAL(imageReinitialized()), this,
				SLOT(updateHueOrRed()));
		connect(node, SIGNAL(added()), this, SLOT(updateHueOrRed()));
	}

	void setGNode(Node* node){
		_satOrGreenNode = node;
		colorMode = rgb_mode;
		saturationImage = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
		connect(node, SIGNAL(imageReinitialized()), this,
				SLOT(updateSatOrGreen()));
		connect(node, SIGNAL(added()), this, SLOT(updateSatOrGreen()));
	}

	void setBNode(Node* node){
		_valOrBlueNode = node;
		colorMode = rgb_mode;
		valueImage = node->getImage();
		connect(node, SIGNAL(imageChanged()), this, SLOT(update()));
		connect(node, SIGNAL(imageReinitialized()), this,
				SLOT(resizeNode()));
		connect(node, SIGNAL(added()), this, SLOT(updateValOrBlue()));
	}

	void reinitImage();

public slots:
	void update();
	void updateHueOrRed();
	void updateSatOrGreen();
	void updateValOrBlue();
	virtual void resizeNode();

private:
	Node* _hueOrRedNode;
	Node* _satOrGreenNode;
	Node* _valOrBlueNode;
	QSharedPointer<QImage> hueImage;
	QSharedPointer<QImage> saturationImage;
	QSharedPointer<QImage> valueImage;
	ColorMode colorMode;

	size_t _node_x_res;
	size_t _node_y_res;
};

#endif /* CE_FINALNODEVIEW_H_ */
