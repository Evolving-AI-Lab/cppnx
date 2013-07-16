/*
 * CE_FinalNodeView.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: joost
 */

#include <iostream>
#include "CE_FinalNodeView.h"

//FinalNodeView::FinalNodeView(int width, int height):NodeView(0, width, height) {
//	hue = new int[width*height];
//	saturation = new unsigned char[width*height];
//	value = new unsigned char[width*height];
//
//	for(int i=0; i<width*height; i++){
//		hue[i]=0;
//		saturation[i]=0;
//		value[i]=0;
//	}
//}

FinalNodeView::FinalNodeView():NodeView(0) {
	pixels = new QImage(node_width, node_height, QImage::Format_RGB32);
	hue = new int[node_width*node_height];
	saturation = new unsigned char[node_width*node_height];
	value = new unsigned char[node_width*node_height];

	for(int i=0; i<node_width*node_height; i++){
		hue[i]=0;
		saturation[i]=0;
		value[i]=0;
	}

	hueImage=0;
	saturationImage=0;
	valueImage=0;
}

FinalNodeView::~FinalNodeView() {
	delete[] hue;
	delete[] saturation;
	delete[] value;
	delete pixels;
}

void FinalNodeView::updateFinalView(const size_t& index){
//	QColor color;
//	color.setHsv(hue[index], saturation[index], value[index]);
//	size_t pixelIndex = index*4;
//	pixels->bits()[pixelIndex]=color.blue();
//	pixels->bits()[pixelIndex+1]=color.green();
//	pixels->bits()[pixelIndex+2]=color.red();
}

void FinalNodeView::update(){
//	std::cout << "Updating final node view" << std::endl;

	unsigned char sat;
	int hue;
	unsigned char value;
	unsigned char tempHue;
//	size_t localindex;
	QColor color;
	if(saturationImage){
		for(size_t index=0; index < node_width*node_height*4; index+=4){
			//		localindex = index*4;

			sat = saturationImage->bits()[index];
			//		hue = (pixels->bits()[localindex+2]/255);
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
