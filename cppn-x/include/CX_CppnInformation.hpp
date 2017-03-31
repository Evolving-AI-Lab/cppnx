/*
 * CX_CppnInformation.hpp
 *
 *  Created on: Mar 30, 2017
 *      Author: joost
 */

#ifndef CPPN_X_INCLUDE_CX_CPPNINFORMATION_HPP_
#define CPPN_X_INCLUDE_CX_CPPNINFORMATION_HPP_

class CppnInformation {
public:
	CppnInformation():
		min_x(-1),
		max_x(1),
		min_y(-1),
		max_y(1),
		x_res(IMAGE_WIDTH),
		y_res(IMAGE_HEIGHT)
	{
		// Intentionally left blank.
	}

    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int x_res;
    int y_res;
};

#endif /* CPPN_X_INCLUDE_CX_CPPNINFORMATION_HPP_ */
