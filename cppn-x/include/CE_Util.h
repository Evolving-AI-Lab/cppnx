/*
 * CE_Util.h
 *
 *  Created on: Jun 14, 2013
 *      Author: joost
 */

#ifndef CE_UTIL_H_
#define CE_UTIL_H_
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>


namespace util{
template <typename Type>
inline std::string toString(Type value){
	std::ostringstream stream;
	stream << std::setprecision(16) << value;
	return stream.str();
}


inline int toInt(std::string value){
	std::stringstream stream;
	int result;
	stream << value;
	stream >> result;
	return result;
}

inline double toDouble(std::string value){
	std::stringstream stream;
	double result;
	stream << value;
	stream >> result;
	return result;
}



}


#endif /* CE_UTIL_H_ */
