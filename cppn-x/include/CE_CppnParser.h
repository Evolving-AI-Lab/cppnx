/*
 * cppnParser.h
 *
 *  Created on: May 23, 2013
 *      Author: joost
 */

//#include <exception>

#ifndef CPPNPARSER_H_
#define CPPNPARSER_H_

#include "CE_Defines.h"
//#include <boost/regex.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/shared_ptr.hpp>
#include <fstream>
#include <JGTL_LocatedException.h>
#include <map>
#include "CE_Cppn.h"
#include "CE_CppnWidget.h"
//#include <errno.h>
//#include <boost/exception/all.hpp>

//typedef boost::error_info<struct tag_errno,std::string> err_msg; //(1)

//class ParseException: public boost::exception, public std::exception { }; //(2)

typedef std::map<std::string, std::string> io_map_t;
//typedef std::map<std::string, std::string> io_map_t;

class Cppn;
class GraphWidget;

class CppnParser
{
public:
	CppnParser(std::string fileName, GraphWidget* widget);
	virtual ~CppnParser();

	Cppn* parse();

private:
	bool parseLine(std::string line, std::string expected);
	std::string parseParameter(const std::string& line, std::string::iterator& currentChar, std::string::iterator& expectedChar);
	bool parseExpected(const std::string& line, std::string::iterator& currentChar, const std::string& expected, std::string::iterator& expectedChar);
	void parseWhiteSpace(std::string::iterator& it);


//	void parseLine(boost::regex regex);
//	bool tryParseLine(boost::regex regex);
//	bool parseCount(boost::regex regex=count);
	void parseLine(std::string regex);
	bool tryParseLine(std::string regex);
//	bool parseCount(std::string regex);
	bool parseCount(std::string template_str);
	void parseHeader(bool store);
	void parseNode(bool store);
	void parseEdge(bool store);
//	void parseFooter(bool store);
	void parseColorButton(bool store);
	void parseGenome(bool store);
	void parseParent(bool store);


	std::istream* myfile;
	Cppn* cppn;
	GraphWidget* widget;
//	boost::smatch m;
	std::vector<std::string> m;

	std::string data_version;
	std::string line;
	bool nextLine;
	size_t lineNumber;

	int parseCounter;
};

#endif /* CPPNPARSER_H_ */
