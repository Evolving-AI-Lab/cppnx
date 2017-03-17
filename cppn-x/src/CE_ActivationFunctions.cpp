/*
 * CE_ActivationFunctions.cpp
 *
 *  Created on: May 31, 2013
 *      Author: joost
 */

//Standard includes
#include <cmath>
#include <iostream>
#include <iomanip>

//Local includes
#include "CE_ActivationFunctions.h"
#include "CE_Defines.h"
#include "CX_InvalidCaseException.hpp"


namespace act_functions{
	//Facts derived from the PicBreeder source
	//frequency = 0.001;
	//resolution = (1.0 / frequency); (Meaning resolution = 1000)
#define FREQUENCY 0.001
#define RESOLUTION 1000
#define SIGMOID_LOW -7.6014025936126712
#define SIGMOID_HIGH 7.6013787517547611
#define SIGMOID_DIF 15.202781345367432
#define SIGMOID_SIZE 15203
#define GAUSSIAN_LOW -2.7579733983037391
#define GAUSSIAN_HIGH 2.7579733983037391
#define GAUSSIAN_DIF 5.5159467966074782
#define GAUSSIAN_SIZE 5516

	double signedSigmoidTable[SIGMOID_SIZE];
	double sigmoidLow;
	double sigmoidHigh;
	int sigmoidOffset;


	double signedGaussianTable[GAUSSIAN_SIZE];
	double gaussianLow;
	double gaussianHigh;
	int gaussianOffset;

//	double* table;
//	int offset;
//	double resolution;
//	double low;
//	double high;

	ActivationFunctionInfo getActivationFunction(const std::string& xmlActivationFunction){
	    ActivationFunctionInfo result;
	    if(xmlActivationFunction == XML_GAUSSIAN){
	        result.activationFunction = act_functions::gaussian;
	        result.shortName = "gau()";
	    } else if(xmlActivationFunction == XML_LINEAR){
	        result.activationFunction = act_functions::identity;
	        result.shortName = "lin()";
	    } else if(xmlActivationFunction == XML_SIN){
	        result.activationFunction = act_functions::sin;
	        result.shortName = "sin()";
	    } else if(xmlActivationFunction == XML_SIGMOID){
	        result.activationFunction = act_functions::sigmoid;
	        result.shortName = "sig()";
	    } else if(xmlActivationFunction == XML_COS){
	        result.activationFunction = act_functions::cos;
	        result.shortName = "cos()";
	    } else if(xmlActivationFunction == XML_STEP){
	        result.activationFunction = act_functions::ustep;
	        result.shortName = "ustep()";
	    } else if(xmlActivationFunction == XML_U_SIGMOID){
	        result.activationFunction = act_functions::uSigmoid;
	        result.shortName = "usig()";
	    } else if(xmlActivationFunction == XML_U_GUASSIAN){
	        result.activationFunction = act_functions::uGaussian;
	        result.shortName = "ugau()";
        } else if(xmlActivationFunction == XML_U_BOUNDED_LINEAR){
            result.activationFunction = act_functions::uBoundedLinear;
            result.shortName = "ublin()";
	    } else {
	        throw InvalidCaseException("Unknown activation function: '" + xmlActivationFunction + "'");
	    }
	    return result;
	}

	std::vector<std::string> getDefaultXmlActivationFunctions(){
	    std::vector<std::string> result;
	    result.push_back(XML_GAUSSIAN);
	    result.push_back(XML_LINEAR);
	    result.push_back(XML_SIN);
	    result.push_back(XML_COS);
	    result.push_back(XML_SIGMOID);
	    return result;
	}

	double actualSigmoid ( double input ) {
		return (1.0 / (1+std::exp(float(-input)))) * 2.0 - 1.0;
	}

	double actualGaussian ( double input ) {
		return std::exp(float(-input*input)) * 2.0 - 1.0;
	}

    double uSigmoid ( double input ) {
        return (1.0 / (1+std::exp(float(-input))));
    }

    double uGaussian ( double input ) {
        return std::exp(float(-input*input));
    }

    double uBoundedLinear ( double input) {
        return std::min(std::max(0.0, input), 1.0);
    }

	double findPoint(ActivationFunctionPt f, double center, double boundary, double tolerance){
		double extreme = (*f)(boundary);

		for (int i = 0; i < 1000; i++) {
			double m = (center + boundary) / 2.0;
			if (std::abs(f(m) - extreme) < tolerance)
				boundary = m;
			else {
				center = m;
			}
		}
		return boundary;
	}


	void SampledBoundedFunction(ActivationFunctionPt function, double* table, double& low, double& high, int& offset, int size){
		low = (findPoint(function, 0.0, -100.0, FREQUENCY) - FREQUENCY);
		high = (findPoint(function, 0.0, 100.0, FREQUENCY) + FREQUENCY);

		offset = ((int)(-low * RESOLUTION));
//		table = new double[(int)((high - low) * RESOLUTION) + 1];

		for (int i = 0; i < size; i++){
//			std::cout << "i: " << i << " : "<< (*function)((i - offset) * FREQUENCY) << std::endl;
			table[i] = (*function)((i - offset) * FREQUENCY);
		}
	}

	double valueAt(double* table, double low, double high, int offset, double x) {
	    if (x <= low) return table[0];
	    if (x >= high) return table[RESOLUTION - 1];
	    return table[(offset + (int)(x * RESOLUTION + 0.5))];
	}



	void initializeActivationFunctions(){
		SampledBoundedFunction(actualSigmoid, signedSigmoidTable, sigmoidLow, sigmoidHigh, sigmoidOffset, SIGMOID_SIZE);
		SampledBoundedFunction(actualGaussian, signedGaussianTable, gaussianLow, gaussianHigh, gaussianOffset, GAUSSIAN_SIZE);
//		std::cout << signedSigmoidTable[0] <<std::endl;
//        for (int a=0;a<6001;a++)
//        {
//        	float input = ((a-3000)/1000.0);
//            signedSigmoidTable[a] = ((1 / (1+std::exp(float(-input)))) - 0.5)*2.0;
//            signedGaussianTable[a] = 2*std::exp(-input*input)-1;
//        }
	}

	double identity ( double input ) {
		return input;
	}

	double sin ( double input ) {
//		std::cout << "Sin" << std::endl;
		return std::sin(input);
	}

	double cos ( double input ) {
		return std::cos(input);
	}

	double gaussian ( double input ) {
//		std::cout << std::setprecision(17) << "gaussian current: " << input << " low: " << gaussianLow << " high: " << gaussianHigh  << " offset: " << gaussianOffset << std::endl;
//		std::cout << "index: "<< gaussianOffset + (int)(input * RESOLUTION + 0.5) << " dif: " <<  gaussianHigh - gaussianLow << " value: " << signedGaussianTable[(gaussianOffset + (int)(input * RESOLUTION + 0.5))] << std::endl;
	    if (input <= gaussianLow) return signedGaussianTable[0];
	    if (input >= gaussianHigh) return signedGaussianTable[GAUSSIAN_SIZE - 1];
	    return signedGaussianTable[(gaussianOffset + (int)(input * RESOLUTION + 0.5))];
	}

	double sigmoid ( double input ) {
//		std::cout << std::setprecision(17) << "sigmoid current: " << input << " low: " << sigmoidLow << " high: " << sigmoidHigh << " offset: " << sigmoidOffset << std::endl;
//		std::cout << "index: "<< sigmoidOffset + (int)(input * RESOLUTION + 0.5)  << " dif: " <<  sigmoidHigh - sigmoidLow << " value: " << signedSigmoidTable[(sigmoidOffset + (int)(input * RESOLUTION + 0.5))] << std::endl;
	    if (input <= sigmoidLow) return signedSigmoidTable[0];
	    if (input >= sigmoidHigh) return signedSigmoidTable[SIGMOID_SIZE - 1];
	    return signedSigmoidTable[(sigmoidOffset + (int)(input * RESOLUTION + 0.5))];
	}

    double ustep ( double input ) {
        return input > 0 ? 1.0 : 0.0;
    }

}
