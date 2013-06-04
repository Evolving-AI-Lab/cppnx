/*
 * CE_ActivationFunctions.h
 *
 *  Created on: May 31, 2013
 *      Author: joost
 */

#ifndef CE_ACTIVATIONFUNCTIONS_H_
#define CE_ACTIVATIONFUNCTIONS_H_

namespace act_functions{
	void initializeActivationFunctions();
	double identity ( double input);
	double sigmoid ( double input);
	double sin ( double input);
	double cos ( double input);
	double gaussian ( double input);
}


#endif /* CE_ACTIVATIONFUNCTIONS_H_ */
