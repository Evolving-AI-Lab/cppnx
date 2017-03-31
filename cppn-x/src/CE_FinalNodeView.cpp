/*
 * CE_FinalNodeView.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: joost
 */

#include <iostream>
#include "CE_FinalNodeView.h"
#include "CE_Cppn.h"

FinalNodeView::FinalNodeView():NodeView(0) {
	_pixels = 0;
	reinitImage();
	hueImage=0;
	saturationImage=0;
	valueImage=0;
	compatibillityId = 1;
	colorMode = hsv_mode;
}

FinalNodeView::~FinalNodeView() {
	delete _pixels;
}

void FinalNodeView::reinitImage(){
	if(_pixels) delete _pixels;
	_pixels = 0;
	_pixels = new QImage(_node_x_res, _node_y_res, QImage::Format_RGB32);
	_pixels->fill(0);
}

void FinalNodeView::update(){
	unsigned char sat;
	int hue;
	unsigned char value;
	unsigned char tempHue;

	QColor color;
	if(saturationImage){
		for(size_t index=0; index < _node_x_res*_node_y_res*4; index+=4){
			if(colorMode == hsv_mode){
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
			} else if(colorMode == rgb_mode){
				unsigned char red = hueImage->bits()[index+2];
				unsigned char green = saturationImage->bits()[index+2];
				unsigned char blue = valueImage->bits()[index+2];
				color.setRgb(red, green, blue);
			}

			_pixels->bits()[index]=color.blue();
			_pixels->bits()[index+1]=color.green();
			_pixels->bits()[index+2]=color.red();
		}
	}else{
		for(size_t index=0; index < _node_x_res*_node_y_res*4; index+=4){
			value = (valueImage->bits()[index+2]);
//			std::cout << "Value: "<< (int) value << std::endl;
			color.setHsv(0, 0, value);
			_pixels->bits()[index]=color.blue();
			_pixels->bits()[index+1]=color.green();
			_pixels->bits()[index+2]=color.red();
		}
	}

	NodeView::update();
}

void FinalNodeView::resizeNode(){
	if(_valOrBlueNode && _valOrBlueNode->getCppn()){
		_node_x_res = _valOrBlueNode->getCppn()->getResX();
		_node_y_res = _valOrBlueNode->getCppn()->getResY();
    } else {
    	_node_x_res = IMAGE_WIDTH;
    	_node_y_res = IMAGE_HEIGHT;
    }

//    _half_width = _node_width/2;
//	_half_height = _node_height/2;
//    setIconSize(QSize(_node_width, _node_height));
//    setIconTranslate(QPoint(_half_width, _half_height));
	reinitImage();
	saturationImage = _satOrGreenNode->getImage();
	hueImage = _hueOrRedNode->getImage();
	valueImage = _valOrBlueNode->getImage();
}
