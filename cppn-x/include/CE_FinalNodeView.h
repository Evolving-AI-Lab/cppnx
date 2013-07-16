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
//	FinalNodeView(int width = 256, int height = 256);
	FinalNodeView();
	virtual ~FinalNodeView();

    enum { Type = UserType + FINALVIEW_TYPE };
    virtual int type() const { return Type; }

	void setHue(const size_t& index, const int& value){
		hue[index] = value;
	}

	void setSaturation(const size_t& index, const unsigned char& value){
		saturation[index] = value;
	}

	void setValue(const size_t& index, const unsigned char& _value){
		value[index] = _value;
	}

	void setHueImage(QImage* image){
		hueImage = image;
	}

	void setSaturationImage(QImage* image){
		saturationImage = image;
	}

	void setValueImage(QImage* image){
		valueImage = image;
	}

//	char getHue(const size_t& index){
//		return hue[index];
//	}
//
//	char getSaturation(const size_t& index){
//		return saturation[index];
//	}
//
//	char getValue(const size_t& index){
//		return value[index];
//	}
public slots:
	void updateFinalView(const size_t& index);
	void update();

private:
	int* hue;
	unsigned char* saturation;
	unsigned char* value;

	QImage* hueImage;
	QImage* saturationImage;
	QImage* valueImage;
};

#endif /* CE_FINALNODEVIEW_H_ */
