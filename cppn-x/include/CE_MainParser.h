/*
 * cppnParser.h
 *
 *  Created on: May 23, 2013
 *      Author: joost
 */

//#include <exception>

#ifndef MAINPARSER_H_
#define MAINPARSER_H_

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

#include "CE_Parser.hpp"
#include "CE_Cppn.h"
#include "CE_CppnWidget.h"
#include "CE_Label.h"
#include "CX_FileInformation.h"



typedef std::map<std::string, std::string> io_map_t;


class MainParser: public Parser
{
public:

	MainParser(QString fileName): Parser(fileName), parent(-1), max_generation(-1), data_version(""){
		//nix
	}
	void parse();

	int parent;
	int max_generation;
	std::map<int, int> storage_map;

private:
	bool parseLine(std::string line, std::string expected);
	std::string parseParameter(const std::string& line, std::string::iterator& currentChar, std::string::iterator& expectedChar);
	bool parseExpected(const std::string& line, std::string::iterator& currentChar, const std::string& expected, std::string::iterator& expectedChar);
	void parseWhiteSpace(std::string::iterator& it);



//	void parseLine(std::string regex);
	bool parseLine(std::string regex, bool stopOnFail = true);
//	bool tryParseLine(std::string regex);
	void parseLine(std::string regex, bool stopOnFail, std::vector<std::string> &tokens, size_t index, std::string defaultValue = "", std::string separetor = " ", std::string minVersion = "0.0", std::string maxVersion = "1.2");
	void copyTo(std::vector<std::string> &tokens, size_t from, size_t to, std::string fromSeparetor = " ", std::string toSeparetor = " ", std::string minVersion = "0.0", std::string maxVersion = "1.2");


	bool parseCount(std::string template_str);
	void parseHeader(std::vector<std::string> &tokens);


	enum fileInformation{
		dataVersion,
		fileInformationSize
	};

	enum branchInformation{
		parameters,
		branch,
		branchFrom,
		branchInformationSize
	};


	std::string data_version;


};

#endif /* MAINPARSER_H_ */
