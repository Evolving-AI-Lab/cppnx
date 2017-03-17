/*
 * CE_CppnParser.cpp
 *
 *  Created on: May 31, 2013
 *      Author: joost
 */

#include "CE_MainParser.h"
#include "CE_Xml.h"
#include "CE_Util.h"
#include <assert.h>
#include <limits>
#include <zip.h>






/**
 * Function used to retrieve the number of items in a <word count="8"> construction,
 * and then counting down to zero until the function has been called exactly 'count' times.
 * Used in the parseEach macro, and useful in loops like:
 * while(parseCount(template_str))
 */
bool MainParser::parseCount(std::string template_str){
	std::string regex  = openXml(template_str);
	if(parseCounter>1){
		parseCounter--;
		return true;
	} else if(parseCounter==1){
		parseCounter=0;
		return false;
	} else{
		parseLine(regex);
		parseCounter = util::toInt(m[1]);
		return parseCounter>0;
	}
}

/**
 * Advances the iterator until the next character is not a space.
 */
void MainParser::parseWhiteSpace(std::string::iterator& it){
	while((*it) == ' ') it++;
}

bool MainParser::parseExpected(const std::string& line, std::string::iterator& currentChar,const std::string& expected, std::string::iterator& expectedChar){
	while(currentChar != line.end() && expectedChar != expected.end()){
//		std::cout << "cur: " << int (*currentChar) << " exp: " << int (*expectedChar) <<std::endl;
		if((*currentChar)!=(*expectedChar)) return false;
		currentChar++;
		expectedChar++;
	}

//	std::cout << (expectedChar == expected.end()) << std::endl;
	return expectedChar == expected.end();
}

std::string MainParser::parseParameter(const std::string& line, std::string::iterator& currentChar, std::string::iterator& expectedChar){
	std::string result = "";
	while(currentChar != line.end()){
		if( (*currentChar) == (*expectedChar) ){
//			if(result == "") result = "\"\"";
			return result;
		}
		result.append(1, (*currentChar));
		currentChar++;
	}
//	if(result == "") result = "\"\"";

	return result;
}

/**
 * Parses a line based on the expected string.
 * Returns true if the line was parsed successfully and false otherwise.
 *
 * An example of an expected string would be:
 * <branchFrom branch="*" id="*">
 * Where the * are placeholders for data that we wish to read.
 * The data read at the * positions will be pushed into the m vector.
 * The first element in the m vector will always be the empty string "".
 *
 * This function takes a line as an argument, and as a result,
 * it does not advance the file stream,
 */
bool MainParser::parseLine(std::string line, std::string expected){
	m.clear();
	m.push_back(""); //Filler
	std::string::iterator currentChar = line.begin();
	std::string::iterator expectedChar = expected.begin();
	parseWhiteSpace(currentChar);

	parseExpected(line, currentChar, expected, expectedChar);
	while((*expectedChar)=='*'){
		expectedChar++;
		m.push_back(parseParameter(line, currentChar, expectedChar));
		parseExpected(line, currentChar, expected, expectedChar);
	}

	return expectedChar == expected.end();
}

/**
 * The main function to parse a line based on the 'regex' argument.
 *
 * Advances to the next line if the previous line was parsed successfully,
 * or tries to parse the current line if the previous line failed to parse.
 *
 * If 'stopOnFail' is true, this function throws an exception if it fails to parse a line.
 * If 'stopOnFail' is false, this function returns false and makes sure that subsequent calls
 * will try to parse the same line.
 */
bool MainParser::parseLine(std::string regex, bool stopOnFail){
	if(!myfile->good()) throw CeParseException("Unexpected end of file.");
	if(nextLine){
		getline (*myfile,line);
		lineNumber++;
	}
	if(parseLine(line, regex)){
		nextLine=true;
	} else if(!stopOnFail) {
		nextLine=false;
	} else {
		throw CeParseException("Parse error on line: " + util::toString(lineNumber) + ".\nRead: "+ line + "\nExpected: " + regex);
	}

	return nextLine;
}


/**
 * Parse line for to extract data from an xml line and put it into a single string.
 *
 * For example, when the line is: <marking branch="49" id="9"/>
 * And it is parsed with <marking branch="*" id="*"/>
 * The token at index will now be: "49<separetor>9"
 */
void MainParser::parseLine(std::string regex, bool stopOnFail, std::vector<std::string> &tokens, size_t index, std::string defaultValue, std::string separetor, std::string minVersion, std::string maxVersion){
	if (data_version >= minVersion && data_version <=maxVersion){
		std::string result;
		if(parseLine(regex, stopOnFail)){
			for(size_t i=1; i<m.size(); i++){
				result.append(m[i]);
				if(i+1<m.size()) result.append(separetor);
			}
		}else{
			result = defaultValue;
		}

		tokens[index] = result;
	}else{
		if(tokens[index]=="") tokens[index] = defaultValue;
	}
}

void MainParser::copyTo(std::vector<std::string> &tokens, size_t from, size_t to, std::string fromSeparetor, std::string toSeparetor, std::string minVersion, std::string maxVersion){
	if (data_version >= minVersion && data_version <=maxVersion){
		std::string result;
		for(size_t i=0; i<tokens[from].size(); i++){
			if(tokens[from].substr(i, 1) == fromSeparetor){
				result.append(toSeparetor);
			} else{
				result.append(tokens[from].substr(i, 1));
			}
		}
		tokens[to] = result;
	}
}


void MainParser::parseHeader(std::vector<std::string> &tokens){
	data_version = "0.0";
	if(!parseLine(ce_xml::getFirstLine(), false)) parseLine("<?xml*?>");
	parseLine(openXml(ce_xml::data), false, tokens, dataVersion, "0.0");
	data_version = tokens[dataVersion];
}


void MainParser::parse(){
    dbg::out(dbg::info, "parser") << "Parsing file" << std::endl;

	//Read header information
	std::vector<std::string> tokens(fileInformationSize);
	parseHeader(tokens);

	//Read branch information
	std::vector<std::string> branchInformation(branchInformationSize);
	parseLine(openCloseXml(ce_xml::parameters), false, branchInformation, parameters, "unknown");
	parseLine(openXml(ce_xml::series), false, branchInformation, branch, "unknown");
	if(parseLine(readXml(ce_xml::branchFrom), false)){
		parent = util::toInt(m[1]);
//		std::cout << parent << std::endl;
	} else {
		parseLine(readXml(ce_xml::branchFromEmpty), false);
	}

	//Read the generations
//	int max_generation = -1;
//	std::map<int, int> storageMap;
	int generation;
	int storage;
	while(parseLine(readXml(ce_xml::generation_storage), false)){
		generation = util::toInt(m[1]);
		storage = util::toInt(m[2]);
		if(generation > max_generation){
			max_generation = generation;
		}
		storage_map[generation] = storage;
	}
//	parseLine(close(ce_xml::series), true);
//	parseLine(close(ce_xml::data), false);
}


