/*
 * CE_FinalNodeView.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: joost
 */

#include <iostream>
#include "CE_FinalNodeView.h"
#include "CE_Cppn.h"

FinalNodeView::FinalNodeView():
	NodeView(0),
	_hueOrRedNode(0),
	_satOrGreenNode(0),
	_valOrBlueNode(0),
	hueImage(0),
	saturationImage(0),
	valueImage(0),
	colorMode(hsv_mode)
{
	resizeNode();
}

FinalNodeView::~FinalNodeView() {
	_pixels.clear();
	//if(_pixels) delete _pixels;
}

void FinalNodeView::reinitImage(){
//	if(_pixels) delete _pixels;
//	_pixels = 0;
	_pixels.clear();
	_pixels = QSharedPointer<QImage>(new QImage(_node_x_res, _node_y_res,
			QImage::Format_RGB32));
	_pixels->fill(0);
}

void FinalNodeView::update(){
//	unsigned char sat;
//	int hue;

//	unsigned char tempHue;
//	std::cout << "Marked for deletion: " << this->isDeleted() << std::endl;
//	if(_satOrGreenNode){
//		std::cout << "Green node marked for deletion: " << _satOrGreenNode->isDeleted() << std::endl;
//	} else {
//		std::cout << "Green node deleted" << std::endl;
//	}
//	if(_hueOrRedNode){
//		std::cout << "Red node marked for deletion: " << _hueOrRedNode->isDeleted() << std::endl;
//	} else {
//		std::cout << "Red node deleted" << std::endl;
//	}
//	if(_valOrBlueNode){
//		std::cout << "Blue node marked for deletion: " << _valOrBlueNode->isDeleted() << std::endl;
//	} else {
//		std::cout << "Blue node deleted" << std::endl;
//	}

	QColor color;
	if(saturationImage && hueImage && valueImage){
//		if(saturationImage->size().isEmpty() ||
//				hueImage->size().isEmpty() ||
//				valueImage->size().isEmpty())
//		{
//			return;
//		}
		if(colorMode == hsv_mode){
			int hue;
			unsigned char sat;
			unsigned char tempHue;
			unsigned char value;
			for(size_t index=0; index < _node_x_res*_node_y_res*4; index+=4){
				sat = saturationImage->bits()[index];
				tempHue = hueImage->bits()[index+2];
				value = valueImage->bits()[index+2];
				if(hueImage->bits()[index] > 0){
					hue = (tempHue*(360.0/255.0)+360);
				} else {
					hue = (-(tempHue*(360.0/255.0))+360);
				}
				color.setHsv(hue, sat, value);
				_pixels->bits()[index]=color.blue();
				_pixels->bits()[index+1]=color.green();
				_pixels->bits()[index+2]=color.red();
			}
		} else if(colorMode == rgb_mode){
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			for(size_t index=0; index < _node_x_res*_node_y_res*4; index+=4){
				red = hueImage->bits()[index+2];
				green = saturationImage->bits()[index+2];
				blue = valueImage->bits()[index+2];
				color.setRgb(red, green, blue);
				_pixels->bits()[index]=color.blue();
				_pixels->bits()[index+1]=color.green();
				_pixels->bits()[index+2]=color.red();
			}
		} else {
			std::cerr << "Unknown color mode: " << colorMode << std::endl;
		}
//		for(size_t index=0; index < _node_x_res*_node_y_res*4; index+=4){
//			if(colorMode == hsv_mode){
////				std::cout << "Retrieving pixel values" << std::endl;
//				sat = saturationImage->bits()[index];
//				tempHue = hueImage->bits()[index+2];
//				value = valueImage->bits()[index+2];
//				if(hueImage->bits()[index] > 0){
//					hue = (tempHue*(360.0/255.0)+360);
//				} else {
//					hue = (-(tempHue*(360.0/255.0))+360);
//				}
////				std::cout << "Retrieving pixel values done" << std::endl;
//				color.setHsv(hue, sat, value);
//			} else if(colorMode == rgb_mode){
////				std::cout << "Retrieving pixel values" << std::endl;
//				unsigned char red = hueImage->bits()[index+2];
//				unsigned char green = saturationImage->bits()[index+2];
//				unsigned char blue = valueImage->bits()[index+2];
////				std::cout << "Retrieving pixel values done" << std::endl;
//				color.setRgb(red, green, blue);
//			}
//
////			std::cout << "Setting pixel values" << std::endl;
//			_pixels->bits()[index]=color.blue();
//			_pixels->bits()[index+1]=color.green();
//			_pixels->bits()[index+2]=color.red();
////			std::cout << "Setting pixel values done" << std::endl;
//		}
	} else if(valueImage){
		unsigned char value;
		for(size_t index=0; index < _node_x_res*_node_y_res*4; index+=4){
			value = (valueImage->bits()[index+2]);
			color.setHsv(0, 0, value);
			_pixels->bits()[index]=color.blue();
			_pixels->bits()[index+1]=color.green();
			_pixels->bits()[index+2]=color.red();
		}
	}

	NodeView::update();
}

void FinalNodeView::updateHueOrRed(){
	if(_hueOrRedNode) hueImage = _hueOrRedNode->getImage();
}

void FinalNodeView::updateSatOrGreen(){
	if(_satOrGreenNode) saturationImage = _satOrGreenNode->getImage();
}

void FinalNodeView::updateValOrBlue(){
	if(_valOrBlueNode) valueImage = _valOrBlueNode->getImage();
}

void FinalNodeView::resizeNode(){
	if(_valOrBlueNode && _valOrBlueNode->getCppn()){
		_node_x_res = _valOrBlueNode->getCppn()->getResX();
		_node_y_res = _valOrBlueNode->getCppn()->getResY();
    } else {
    	_node_x_res = IMAGE_WIDTH;
    	_node_y_res = IMAGE_HEIGHT;
    }
	reinitImage();
	updateHueOrRed();
	updateSatOrGreen();
	updateValOrBlue();
//	if(_satOrGreenNode) saturationImage = _satOrGreenNode->getImage();
//	if(_hueOrRedNode) hueImage = _hueOrRedNode->getImage();
//	if(_valOrBlueNode) valueImage = _valOrBlueNode->getImage();
}
