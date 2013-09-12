/*
 * CE_defines.h
 *
 *  Created on: May 23, 2013
 *      Author: joost
 */

#ifndef VISUALIZE_DEFINES_H_
#define VISUALIZE_DEFINES_H_

//#include <boost/shared_ptr.hpp>
#include <string>
#include <iostream>



//Input values for NEAT_GeneticNodeGene
#define TYPE_INPUT "NetworkSensor"
#define TYPE_OUTPUT "NetworkOutputNode"
#define TYPE_HIDDEN "HiddenNode"




#define XML_TYPE_INPUT "in"
#define XML_TYPE_OUTPUT "out"
#define XML_TYPE_HIDDEN "hidden"
#define XML_GAUSSIAN "gaussian(x)"
#define XML_LINEAR "identity(x)"
#define XML_SIN "sin(x)"
#define XML_COS "cos(x)"
#define XML_SIGMOID "sigmoid(x)"

#define DRAW_POS_ZERO 0
#define DRAW_POS_HALF_MAX numeric_limits<double>::max()/2
#define RANDOM_ACTIVATION_FALSE false
#define RANDOM_ACTIVATION_TRUE true

//Input values for NEAT_GeneticIndividual
#define CREATE_TOPOLOGY_TRUE true
#define CREATE_TOPOLOGY_FALSE false
#define EDGE_DENSITY_FULLY_CONNECTED 1
#define EDGE_DENSITY_NOT_CONNECTED 0

#define INPUT_X "x"			//The name of the node taking the x-axis as input
#define INPUT_Y "y"			//The name of the node taking the y-axis as input
#define INPUT_D "d"			//The name of the node taking the y-axis as input
#define INPUT_BIAS "bias"	//The name of the node bias node
#define OUTPUT "ink"		//The name of the output node
#define OUTPUT_INK "ink"
#define OUTPUT_SATURATION "saturation"
#define OUTPUT_HUE "hue"
#define OUTPUT_BRIGTHNESS "brightness"

#define NODE_TYPE 1
#define EDGE_TYPE 2
#define NODEVIEW_TYPE 3
#define FINALVIEW_TYPE 4
#define SELECTABLE_OBJECT_TYPE 5
#define LABELABLE_OBJECT_TYPE 6
#define DRAG_AND_DROP_OBJECT_TYPE 7
#define LABEL_TYPE 8

#define SET_WEIGHT_ID 1
#define RESET_WEIGHT_ID 2
#define SET_POS_ID 3
#define SET_BOOKEND_ID 4

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

//#define SELECTED_COLOR Qt::blue
//#define CONTEXT_EVENT_COLOR Qt::cyan

#define SELECTED_COLOR QColor(150,200,255)
#define CONTEXT_EVENT_COLOR QColor(226,226,255)
#define NO_FOCUS_SELECTED_COLOR QColor(150,150,150)

//    static const int width = 256;
//    static const int height = 256;

enum inputs{
	input_x =0,
	input_y =1,
	input_d =2,
	input_b =3,
	nr_of_inputs =4
};

typedef double (*ActivationFunctionPt)(double);
typedef unsigned int id_t;



#endif /* VISUALIZE_DEFINES_H_ */
