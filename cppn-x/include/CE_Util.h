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
#include <QString>

#include "CE_Defines.h"


namespace util{

template <typename Type1, typename Type2, typename Type3>
inline Type1 multiCast(QGraphicsItem *item){
	Type1 convertedItem = qgraphicsitem_cast<Type1>(item);
	if(convertedItem) return convertedItem;
	convertedItem = qgraphicsitem_cast<Type2>(item);
	if(convertedItem) return convertedItem;
	convertedItem= qgraphicsitem_cast<Type3>(item);
	return convertedItem;
}

template <typename Type1, typename Type2>
inline Type1 multiCast(QGraphicsItem *item){
	Type1 convertedItem1 = qgraphicsitem_cast<Type1>(item);
	if(convertedItem1) return convertedItem1;
	convertedItem1= qgraphicsitem_cast<Type2>(item);
	return convertedItem1;
}

template <typename Type>
inline QString toQString(Type value){
	return QString(toString(value).c_str());
}

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
