/*
 * CE_Util.h
 *
 *  Created on: Jun 14, 2013
 *      Author: joost
 */

#ifndef CE_UTIL_H_
#define CE_UTIL_H_

//Standard includes
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>

//QT includes
#include <QString>
#include <QMessageBox>
#include <QtGlobal>
#include <QDir>
#include <QGraphicsItem>
#include <QList>
#include <QStack>

//Local includes
#include "CE_Defines.h"
#include "CX_Random.hpp"
#include "CE_ActivationFunctions.h"

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
    dbg::trace trace("util", DBG_HERE);
	std::string temp;
	std::string result;

	stream >> temp;
	dbg::out(dbg::info, "util") << "String: " << temp << std::endl;

	result.append(temp);
	while (temp.substr(temp.size()-1, 1) != "\"" && stream.good()){
	    dbg::out(dbg::info, "util") << "Last character: "<< temp.substr(temp.size()-1, 1) << std::endl;
		stream >> temp;
		dbg::out(dbg::info, "util") << "String: " << temp  <<
		        " stream state: " << stream.good() <<
		        " " << stream.eof() <<
		        " " << stream.fail() <<
		        " " << std::endl;
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
inline std::string toString(Type value){
	std::ostringstream stream;
	stream << std::setprecision(17) << value;
	return stream.str();
}



template <typename Value>
inline std::string qListToString(QList<Value> container){
    std::ostringstream stream;
    foreach(Value value, container){
        stream << value << ", ";
    }
    return stream.str();
}

template <typename Value>
inline std::string qStackToString(QStack<Value> container){
    std::ostringstream stream;
    foreach(Value value, container){
        stream << value << ", ";
    }
    return stream.str();
}


template <typename Value>
inline std::string qPointerListToString(QList<Value> container){
    std::ostringstream stream;
    foreach(Value value, container){
        stream << *value << ", ";
    }
    return stream.str();
}


template <typename Value>
inline std::string qPointerStackToString(QStack<Value> container){
    std::ostringstream stream;
    foreach(Value value, container){
        stream << *value << ", ";
    }
    return stream.str();
}




template <typename Type>
inline QString toQString(Type value){
	return QString(toString(value).c_str());
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

template<typename Type>
inline double numericCast(std::string value){
    std::stringstream stream;
    Type result;
    stream << value;
    stream >> result;
    return result;
}



QString getSupportedExtentions();
QString getSupportedFilename(QWidget* parent, const QString& text);

QString getBase(const QString& fileName);
QString getNameFromBase(const QString& fileName);
QString getExtention(const QString& fileName);

void insertFront(QMenu* menu, QAction* action);

template<typename Object>
void saveBatch(QWidget* parent, std::vector<Object> objects){
	if(objects.size() > 1){
		QString newFileName = getSupportedFilename(parent, QMessageBox::tr("Chose save directory"));
		if(newFileName.isEmpty()) return;

		QString captureDirectory = util::getBase(newFileName);
		QString extention = util::getExtention(newFileName);
		if(QDir().exists(captureDirectory)){
			std::vector<QString> overwrittenFiles;
			foreach(Object item, objects){
				QString newFileName = captureDirectory + "/node_" + util::toQString(item->getName()) + extention;
				if(QDir().exists(newFileName)){
					overwrittenFiles.push_back(newFileName);
				}
			}

			QMessageBox msgBox;
			QString message = "Directory ";
			if(overwrittenFiles.empty()){
				message += " already exists!";
			} else {
				message += " already exists and files will be overwritten!";
			}
			msgBox.setText(message);

			QString informativeMessage = "Do you wish to continue?";
			msgBox.setInformativeText(informativeMessage);

			QString detailedText = "Files will be added to" + captureDirectory + ".\n";
			if(!overwrittenFiles.empty()){
				detailedText += "Also, the following files will be overwritten:\n";
				for(size_t i=0; i<overwrittenFiles.size(); ++i){
					detailedText += overwrittenFiles[i] + "\n";
				}
			}
			msgBox.setDetailedText(detailedText);

			//Add buttons
			QPushButton* abortButton = msgBox.addButton(QMessageBox::Abort);
			QPushButton* continueButton = msgBox.addButton(QMessageBox::tr("Continue"), QMessageBox::ActionRole);
			msgBox.setDefaultButton(abortButton);
			msgBox.exec();

			if (msgBox.clickedButton() == (QAbstractButton*) abortButton) {
				return;
			} else if (msgBox.clickedButton() == (QAbstractButton*) continueButton) {
				//Do nothing
			} else {
				//Panic
				Q_ASSERT(0);
			}

		}
		QDir().mkdir(captureDirectory);

		foreach(Object item, objects){
			QString newFileName = captureDirectory + "/node_" + util::toQString(item->getName()) + extention;
			item->getImage()->save(newFileName);
		}
	} else if(objects.size() == 1) {
		QString newFileName = util::getSupportedFilename(parent, QMessageBox::tr("Save node image"));
		if(newFileName.isEmpty()) return;
		objects.front()->getImage()->save(newFileName);
	}
}

size_t factorial(size_t n);
size_t combinations(size_t n, size_t k);




/**
 * Returns the string of a randomly selected activation function.
 *
 * Possible activation functions are defined in CE_Defines.h.
 */
inline std::string getRandomActivationFunction(){
    std::vector<std::string> activationFunctions = act_functions::getDefaultXmlActivationFunctions();
    size_t activationFunctionIndex = randIndex(activationFunctions.size());
    return activationFunctions[activationFunctionIndex];
}

}
#endif /* CE_UTIL_H_ */
