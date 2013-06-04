/*
 * CE_ActivationFunctions.cpp
 *
 *  Created on: May 31, 2013
 *      Author: joost
 */

#include "CE_ActivationFunctions.h"
#include <cmath>
#include <iostream>

namespace act_functions{
	double signedSigmoidTable[6001];

	void initializeActivationFunctions(){
        for (int a=0;a<6001;a++)
        {
            signedSigmoidTable[a] = ((1 / (1+std::exp(float(-((a-3000)/1000.0))))) - 0.5)*2.0;
        }
	}

	double identity ( double input ) {
		return input;
	}

	double sigmoid ( double input ) {
//		std::cout << "Sigmoid" << std::endl;
		double retVal;
        if (input<-2.9||input>2.9)
        {
            retVal = (double)( ((1.0 / (1+std::exp(float(-input)))) - 0.5)*2.0 );
        }
        else
        {
            retVal = (double)signedSigmoidTable[int(input*1000.0)+3000];
        }

		return retVal;
	}

	double sin ( double input ) {
//		std::cout << "Sin" << std::endl;
		return std::sin(input);
	}

	double cos ( double input ) {
		return std::cos(input);
	}

	double gaussian ( double input ) {
		return 2*std::exp(-(std::pow(input,2)))-1;
	}
}
