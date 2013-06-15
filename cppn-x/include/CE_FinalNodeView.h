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
public:
	FinalNodeView(int width = 256, int height = 256);
	virtual ~FinalNodeView();

	void setHue(const size_t& index, const int& value){
		hue[index] = value;
	}

	void setSaturation(const size_t& index, const unsigned char& value){
		saturation[index] = value;
	}

	void setValue(const size_t& index, const unsigned char& _value){
		value[index] = _value;
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

	void updateFinalView(const size_t& index);
private:
	int* hue;
	unsigned char* saturation;
	unsigned char* value;
};

#endif /* CE_FINALNODEVIEW_H_ */
