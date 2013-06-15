/*
 * Visualize_defines.h
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

enum inputs{
	input_x =0,
	input_y =1,
	input_d =2,
	input_b =3,
	nr_of_inputs =4
};

typedef double (*ActivationFunctionPt)(double);

#endif /* VISUALIZE_DEFINES_H_ */
