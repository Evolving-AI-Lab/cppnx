/*
 * CE_FinalNodeView.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: joost
 */

#include <iostream>
#include "CE_FinalNodeView.h"

FinalNodeView::FinalNodeView(int width, int height):NodeView(width, height) {
	hue = new int[width*height];
	saturation = new unsigned char[width*height];
	value = new unsigned char[width*height];

	for(int i=0; i<width*height; i++){
		hue[i]=0;
		saturation[i]=0;
		value[i]=0;
	}
}

FinalNodeView::~FinalNodeView() {
	delete[] hue;
	delete[] saturation;
	delete[] value;
}

void FinalNodeView::updateFinalView(const size_t& index){
	QColor color;
	color.setHsv(hue[index], saturation[index], value[index]);
	size_t pixelIndex = index*4;
	pixels->bits()[pixelIndex]=color.blue();
	pixels->bits()[pixelIndex+1]=color.green();
	pixels->bits()[pixelIndex+2]=color.red();
}
