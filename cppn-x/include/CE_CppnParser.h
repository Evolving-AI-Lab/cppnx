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
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
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
	CppnParser():data_version(""), line(""), nextLine(true), lineNumber(0), parseCounter(0){

	}

	shared_ptr<Cppn> parse(std::string fileName, GraphWidget* widget);

private:
	void parseLine(boost::regex regex);
	bool tryParseLine(boost::regex regex);
	bool parseCount(boost::regex regex=count);
	void parseHeader();
	void parseNode();
	void parseEdge();
	void parseFooter();
	void parseColorButton();


	std::ifstream myfile;
	shared_ptr<Cppn> cppn;
	boost::smatch m;

	std::string data_version;
	std::string line;
	bool nextLine;
	size_t lineNumber;

	int parseCounter;

	static const boost::regex any;

	static const boost::regex first_line;
	static const boost::regex ionode;
	static const boost::regex node;
	static const boost::regex link;

	static const boost::regex marking;
	static const boost::regex activation;
	static const boost::regex color;
	static const boost::regex position;

	static const boost::regex source;
	static const boost::regex target;
	static const boost::regex weight;


	static const boost::regex cppn_data;
	static const boost::regex nodes_count;
	static const boost::regex link_count;

	static const boost::regex buttons_count;
	static const boost::regex text;

	static const boost::regex count;
	static const boost::regex data;
};

#endif /* CPPNPARSER_H_ */
