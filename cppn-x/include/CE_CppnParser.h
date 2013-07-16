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
//#include <JGTL_LocatedException.h>
#include <map>
#include <stdio.h>
#include <string>
#include <sstream>

#include "CE_Cppn.h"
#include "CE_CppnWidget.h"
#include "CE_Label.h"
#include "CX_FileInformation.h"

class CeParseException : public std::exception
{
	std::string text;
//	char text[4096];

public:

	CeParseException(const std::string &_reason){
		text = _reason;
//		sprintf(text,"%s",_reason.c_str());
	}

	~CeParseException() throw (){

	}

	virtual const char* what() const throw(){
		return text.c_str();
	}
};

//#include <errno.h>
//#include <boost/exception/all.hpp>

//typedef boost::error_info<struct tag_errno,std::string> err_msg; //(1)

//class ParseException: public boost::exception, public std::exception { }; //(2)

typedef std::map<std::string, std::string> io_map_t;
//typedef std::map<std::string, std::string> io_map_t;

//class Cppn;
//class CppnWidget;

class CppnParser
{
public:
	CppnParser(std::string fileName);
	virtual ~CppnParser();

	void parse();

//	Cppn* getCppn(){
//		return cppn;
//	}

	QList<Label*> getLabels(){
		return labels;
	}

	QList<Node*> getNodes(){
		return nodes;
	}

	QList<Edge*> getEdges(){
		return edges;
	}

	FileInformation* getFileInformation(){
		return fileInformation;
	}

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
//	Cppn* cppn;
	FileInformation* fileInformation;
	QList<Label*> labels;
	QList<Node*> nodes;
	QList<Edge*> edges;

//	CppnWidget* widget;
//	boost::smatch m;
	std::vector<std::string> m;

	std::string data_version;
	std::string line;
	bool nextLine;
	size_t lineNumber;

	int parseCounter;

	std::map<std::string, Label*> oldLabelMap;
	std::map<id_t, Label*> labelMap;
    std::map<std::string, Node*> nodeMap;

};

#endif /* CPPNPARSER_H_ */
