/*
 * CE_FinalNodeView.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: joost
 */

#include <iostream>
#include "CE_FinalNodeView.h"

FinalNodeView::FinalNodeView():NodeView(0) {
	pixels = new QImage(node_width, node_height, QImage::Format_RGB32);
	pixels->fill(0);

	hueImage=0;
	saturationImage=0;
	valueImage=0;
	compatibillityId = 1;
}

FinalNodeView::~FinalNodeView() {
	delete pixels;
}

void FinalNodeView::update(){
	unsigned char sat;
	int hue;
	unsigned char value;
	unsigned char tempHue;

	QColor color;
	if(saturationImage){
		for(size_t index=0; index < node_width*node_height*4; index+=4){
			sat = saturationImage->bits()[index];
			tempHue = hueImage->bits()[index+2];
			value = valueImage->bits()[index+2];

			if(hueImage->bits()[index] > 0){
				hue = (tempHue*(360.0/255.0)+360);
//				std::cout << "alt>0 from: " << (unsigned int) tempHue << " to: " << hue <<std::endl;
			} else {
				hue = (-(tempHue*(360.0/255.0))+360);
//				std::cout << "alt<=0 from: " << (unsigned int) tempHue << " to: " << hue <<std::endl;
			}

			color.setHsv(hue, sat, value);
			pixels->bits()[index]=color.blue();
			pixels->bits()[index+1]=color.green();
			pixels->bits()[index+2]=color.red();
		}
	}else{
		for(size_t index=0; index < node_width*node_height*4; index+=4){
			value = (valueImage->bits()[index+2]);
//			std::cout << "Value: "<< (int) value << std::endl;
			color.setHsv(0, 0, value);
			pixels->bits()[index]=color.blue();
			pixels->bits()[index+1]=color.green();
			pixels->bits()[index+2]=color.red();
		}
	}

	NodeView::update();
}
