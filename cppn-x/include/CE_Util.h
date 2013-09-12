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

class QMenu;
class QAction;

namespace util{

template <typename Type1, typename Type2, typename Type3>
class Triple{
public:
	Triple(Type1 element1, Type2 element2, Type3 element3):
		first(element1),second(element2),third(element3){
	}

	Type1 first;
	Type2 second;
	Type3 third;
};

inline std::string readString(std::iostream &stream){
	std::string temp;
	std::string result;

	stream >> temp;
//	temp.substr(1);
	result.append(temp);

	while (temp.substr(temp.size()-1, 1) != "\""){
//		std::cout << temp.substr(temp.size()-1, 1) << std::endl;
		stream >> temp;
		result.append(" ");
		result.append(temp);
	}

	return result.substr(1, result.size()-2);

}


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
	stream << std::setprecision(17) << value;
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

QString getSupportedExtentions();
QString getSupportedFilename(QWidget* parent, const QString& text);

QString getBase(const QString& fileName);
QString getExtention(const QString& fileName);

void insertFront(QMenu* menu, QAction* action);

}
#endif /* CE_UTIL_H_ */
