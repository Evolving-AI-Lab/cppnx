/*
 * CE_ActivationFunctions.h
 *
 *  Created on: May 31, 2013
 *      Author: joost
 */

#ifndef CE_ACTIVATIONFUNCTIONS_H_
#define CE_ACTIVATIONFUNCTIONS_H_

#include <string>
#include <vector>

#define XML_GAUSSIAN "gaussian(x)"
#define XML_LINEAR "identity(x)"
#define XML_SIN "sin(x)"
#define XML_COS "cos(x)"
#define XML_SIGMOID "sigmoid(x)"
#define XML_STEP "step(x)"
#define XML_U_GUASSIAN "ugaussian(x)"
#define XML_U_SIGMOID "usigmoid(x)"
#define XML_U_BOUNDED_LINEAR "ublin(x)"

typedef double (*ActivationFunctionPt)(double);

namespace act_functions{
    struct ActivationFunctionInfo{
        ActivationFunctionPt activationFunction;
        std::string shortName;
    };

    ActivationFunctionInfo getActivationFunction(const std::string& xmlActivationFunction);
    std::vector<std::string> getDefaultXmlActivationFunctions();
	void initializeActivationFunctions();
	double identity ( double input);
	double sigmoid ( double input);
	double sin ( double input);
	double cos ( double input);
	double gaussian ( double input);

    double ustep ( double input);
    double uSigmoid ( double input);
    double uGaussian ( double input);
    double uBoundedLinear ( double input);
}


#endif /* CE_ACTIVATIONFUNCTIONS_H_ */
