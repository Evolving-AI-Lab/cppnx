/*
 * CE_CppnParser.cpp
 *
 *  Created on: May 31, 2013
 *      Author: joost
 */

#include "CE_CppnParser.h"
#include "CE_Xml.h"
#include "CE_Util.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits>
#include <zip.h>


#define parseEach(template_str,parser) while(parseCount(template_str)){parser;} parseLine(close(template_str));

CppnParser::CppnParser(std::string fileName, GraphWidget* widget):cppn(0), widget(widget), data_version(""), line(""), nextLine(true), lineNumber(0), parseCounter(0){
	cppn = new Cppn(widget);

	unsigned found = fileName.find_last_of(".");
	if(fileName.substr(found+1) == "zip"){
		//Open the ZIP archive
		int err = 0;
		zip *z = zip_open(fileName.c_str(), 0, &err);

		//Search for the file of given name
		const char *name = "a";
		struct zip_stat st;
		zip_stat_init(&st);
		zip_stat(z, name, 0, &st);

		//Alloc memory for its uncompressed contents
		char *contents = new char[st.size];

		//Read the compressed file
		zip_file *f = zip_fopen(z, "a", 0);
		zip_fread(f, contents, st.size);
		zip_fclose(f);
		std::string contentStr(contents);

		//And close the archive
		zip_close(z);
		myfile = new std::istringstream(contentStr);
		delete [] contents;
	} else {
		myfile = new std::ifstream(fileName.c_str());
	}
}

CppnParser::~CppnParser(){
	delete myfile;
}


std::string openClose(std::string template_str){
	return std::string(ce_xml::getOpenXmlString(template_str) + "*" + ce_xml::getCloseXmlString(template_str));
}

std::string open(std::string template_str){
	return std::string(ce_xml::getOpenXmlString(template_str, "*"));
}

std::string read(std::string template_str){
	return std::string(ce_xml::getOneLineXmlString(template_str, "*"));
}

std::string close(std::string template_str){
	return std::string(ce_xml::getCloseXmlString(template_str));
}


bool CppnParser::parseCount(std::string template_str){
	std::string regex  = open(template_str);
	if(parseCounter>1){
		parseCounter--;
		return true;
	} else if(parseCounter==1){
		parseCounter=0;
		return false;
	} else{
		parseLine(regex);
//		parseCounter = boost::lexical_cast<int>(m[1]);
		parseCounter = util::toInt(m[1]);
		return parseCounter>0;
	}
}
//
//void CppnParser::parseLine(boost::regex regex){
//	if(!myfile.good()) throw JGTL::LocatedException("Unexpected end of file.");
//	if(nextLine){
//		getline (myfile,line);
//		lineNumber++;
//	}
//	if(regex_search(line, m, regex)){
//		nextLine=true;
//	} else {
//		throw JGTL::LocatedException("Parse error on line: " + boost::lexical_cast<std::string>(lineNumber) + ".\nRead: "+ line + "\nExpected: " + regex.str());
//	}
//}
//
bool CppnParser::tryParseLine(std::string regex){
	if(!myfile->good()) throw JGTL::LocatedException("Unexpected end of file.");
	if(nextLine){
		getline (*myfile,line);
		lineNumber++;
	}
	if(parseLine(line, regex)){
		nextLine=true;
		return true;
	} else {
		nextLine=false;
		return false;
	}
}

void CppnParser::parseWhiteSpace(std::string::iterator& it){
	while((*it) == ' ') it++;
}

bool CppnParser::parseExpected(const std::string& line, std::string::iterator& currentChar,const std::string& expected, std::string::iterator& expectedChar){
	while(currentChar != line.end() && expectedChar != expected.end()){
//		std::cout << "cur: " << int (*currentChar) << " exp: " << int (*expectedChar) <<std::endl;
		if((*currentChar)!=(*expectedChar)) return false;
		currentChar++;
		expectedChar++;
	}

//	std::cout << (expectedChar == expected.end()) << std::endl;
	return expectedChar == expected.end();
}

std::string CppnParser::parseParameter(const std::string& line, std::string::iterator& currentChar, std::string::iterator& expectedChar){
	std::string result = "";
	while(currentChar != line.end()){
		if( (*currentChar) == (*expectedChar) ) return result;
		result.append(1, (*currentChar));
		currentChar++;
	}

	return result;
}

bool CppnParser::parseLine(std::string line, std::string expected){
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


void CppnParser::parseLine(std::string regex){
	if(!myfile->good()) throw JGTL::LocatedException("Unexpected end of file.");
	if(nextLine){
		getline (*myfile,line);
		lineNumber++;
	}
	if(parseLine(line, regex)){
		nextLine=true;
	} else {
		throw JGTL::LocatedException("Parse error on line: " + util::toString(lineNumber) + ".\nRead: "+ line + "\nExpected: " + regex);
	}
}


void CppnParser::parseHeader(bool store){
	//Parse first line
//	parseLine(first_line);
	if(!tryParseLine(ce_xml::getFirstLine())) parseLine("<?xml*?>");

	if(tryParseLine(read(ce_xml::cppn_data))){
//		data_version = m[1].str();
		data_version = m[1];
		if(store) cppn->setNewFile(false);
	} else {
		data_version = "0.0";
		if(store) cppn->setNewFile(true);
	}

	if(tryParseLine(open(ce_xml::data))){
//		if(store) cppn->setDataVersion(m[1].str());
		if(store) cppn->setDataVersion(m[1]);
	} else {
		if(store) cppn->setDataVersion("1.0");
	}
}

void CppnParser::parseColorButton(bool store){
//	std::cout << "Parsing label" << std::endl;
	std::string text_str;
	int r;
	int g;
	int b;
	QColor color_q(0, 0, 0);
	std::string color_str = "";
	LabelWidget* labelWidget;
	id_t id;


	parseLine(open(ce_xml::color_button));

	if (data_version >= "1.1"){
		parseLine(read(ce_xml::color_label));
		id = util::toInt(m[1]);
	} else {
		id = oldLabelMap.size()+1;
	}

	parseLine(openClose(ce_xml::text));
	//	text_str = m[1].str();
	text_str = m[1];

	parseLine(read(ce_xml::color));
	r = util::toInt(m[1]);
	g = util::toInt(m[2]);
	b = util::toInt(m[3]);
	color_q = QColor (r, g, b);
	color_str = m[1] + m[2] + m[3];

	parseLine(close(ce_xml::color_button));

	if(store){
		labelWidget = new LabelWidget(text_str.c_str(), color_q, false);
		labelWidget->setId(id);

		if (data_version >= "1.1"){
			labelMap[id] = labelWidget;
		} else {
//			std::cout << "Parsed address: " << (void*) labelWidget << std::endl;
			oldLabelMap[color_str] = labelWidget;
		}
		cppn->addLabelWidget(labelWidget);

	}

}


void CppnParser::parseNode(bool store){
//	std::cout << "Parsing node" << std::endl;
	std::string type;
	std::string label="";
	std::string branch;
	std::string bias = "0.0";
	std::string affinity = "grey";
	std::string id;
	std::string activation_function;
	QColor color_q(255, 255, 255);
	QPointF position_q(0, 0);

	//Version 1.1
	LabelWidget* labelWidget = 0;
	id_t labelId =0;
	std::string text = "";
	std::string color_str = "";


	int r;
	int g;
	int b;
	double x;
	double y;

	if(tryParseLine(open(ce_xml::node))){
		type = m[1];
	} else if(tryParseLine(open(ce_xml::ionode))){
		label = m[1];
		type = m[2];
	} else if(tryParseLine(open(ce_xml::colornode))){
		affinity = m[1];
		bias = m[2];
		type = m[3];
	} else{
		parseLine(open(ce_xml::iocolornode));
		affinity = m[1];
		bias = m[2];
		label = m[3];
		type = m[4];
	}

	parseLine(read(ce_xml::marking));
	branch = m[1];
	id = m[2];

	parseLine(openClose(ce_xml::activation));
	activation_function = m[1];


	if(data_version >= "1.1"){
		parseLine(read(ce_xml::color_label));
		labelId = util::toInt(m[1]);
	} else if(data_version >= "1.0"){
		parseLine(read(ce_xml::color));
		r = util::toInt(m[1]);
		g = util::toInt(m[2]);
		b = util::toInt(m[3]);
		color_q = QColor (r, g, b);
		color_str = m[1] + m[2] + m[3];
	}

	if(data_version >= "1.0"){
		parseLine(read(ce_xml::position));
		x = util::toDouble(m[1]);
		y = util::toDouble(m[2]);
		position_q = QPointF (x, y);
	}

	if(data_version >= "1.1"){
		parseLine(openClose(ce_xml::text));
		text = m[1];
	}

	parseLine(close(ce_xml::node));

//	std::cout << "Parsed line: " << lineNumber << " label: " << label <<std::endl;
	if(store){
		Node* node;
		if (data_version >= "1.1"){
			labelWidget = labelMap[labelId];
			node = new Node(widget, branch, id, type, activation_function, label, affinity, bias, cppn->width, cppn->height, labelWidget, text);
		} else if (data_version >= "1.0"){
			labelWidget = oldLabelMap[color_str];
//			std::cout << "Retrieved address: " << (void*) labelWidget << std::endl;
			node = new Node(widget, branch, id, type, activation_function, label, affinity, bias, cppn->width, cppn->height, labelWidget);
		} else {
			node = new Node(widget, branch, id, type, activation_function, label, affinity, bias, cppn->width, cppn->height);
		}
		node->setPos(position_q);
		cppn->addNode(node);
	}
}

void CppnParser::parseEdge(bool store){
//	std::cout << "Parsing edge" << std::endl;
	std::string branch;
	std::string id;
	std::string source_branch;
	std::string source_id;
	std::string target_branch;
	std::string target_id;
	int r;
	int g;
	int b;
	double weight_f=0;
	QColor color_q(255, 255, 255);

	//Version 1.1
	LabelWidget* labelWidget = 0;
	double original_weight_f=0;
	id_t labelId =0;
	std::string text = "";
	std::string color_str = "";

	//Parse first line
	parseLine(open(ce_xml::link));

	parseLine(read(ce_xml::marking));
//	branch = m[1].str();
//	id = m[2].str();
	branch = m[1];
	id = m[2];

	parseLine(read(ce_xml::source));
//	source_branch = m[1].str();
//	source_id = m[2].str();
	source_branch = m[1];
	source_id = m[2];

	parseLine(read(ce_xml::target));
//	target_branch = m[1].str();
//	target_id = m[2].str();
	target_branch = m[1];
	target_id = m[2];

	parseLine(openClose(ce_xml::weight));
//	weight_f = boost::lexical_cast<double>(m[1].str());
	weight_f = util::toDouble(m[1]);

	if(data_version >= "1.1"){
		parseLine(openClose(ce_xml::original_weight));
		original_weight_f = util::toDouble(m[1]);

		parseLine(read(ce_xml::color_label));
		labelId = util::toInt(m[1]);

		parseLine(openClose(ce_xml::text));
		text = m[1];
	}else if(data_version >= "1.0"){
		parseLine(read(ce_xml::color));
		r = util::toInt(m[1]);
		g = util::toInt(m[2]);
		b = util::toInt(m[3]);
		color_q = QColor (r, g, b);
		color_str = m[1] + m[2] + m[3];
	}

	parseLine(close(ce_xml::link));

//	std::cout << "Parsed line: " << lineNumber <<std::endl;
	if(store){
		Edge* edge;
		if (data_version >= "1.1"){
			labelWidget = labelMap[labelId];
			edge = new Edge(widget, branch, id, cppn->getNode(source_branch+"_"+source_id), cppn->getNode(target_branch+"_"+target_id), weight_f, original_weight_f, labelWidget, text);
		} else if (data_version >= "1.0"){
			labelWidget = oldLabelMap[color_str];
			edge = new Edge(widget, branch, id, cppn->getNode(source_branch+"_"+source_id), cppn->getNode(target_branch+"_"+target_id), weight_f, weight_f, labelWidget);
		} else {
			edge = new Edge(widget, branch, id, cppn->getNode(source_branch+"_"+source_id), cppn->getNode(target_branch+"_"+target_id), weight_f, weight_f);
		}
		cppn->addConnection(edge);
	}
}


//void CppnParser::parseFooter(bool store){
//	while(myfile.good()){
//		parseLine(any);
//		if(store) cppn->addFooterLine(line);
//	}
//}

void CppnParser::parseParent(bool store){
	parseLine(read(ce_xml::identifier));
//	if(store) cppn->addParent(m[1].str(), m[2].str());
	if(store) cppn->addParent(m[1], m[2]);
}

void CppnParser::parseGenome(bool store){
	//Set genome
	if(tryParseLine(open(ce_xml::genome))){
//		if(store) cppn->setGenome(m[1].str());
		if(store) cppn->setGenome(m[1]);
	} else {
		parseLine(open(ce_xml::genomePhen));
//		if(store) cppn->setGenome(m[1].str(), m[2].str());
		if(store) cppn->setGenome(m[1], m[2]);
	}

	//Set identifier
	parseLine(read(ce_xml::identifier));
	if(store) cppn->setIdentifier(m[1], m[2]);

	//Set parents
	parseEach(ce_xml::parent_count, parseParent(store));

	//Parse buttons
	if(data_version >= "1.0"){
		parseEach(ce_xml::buttons_count, parseColorButton(store));
	}

	//Parse nodes
	parseEach(ce_xml::nodes_count, parseNode(store));
	//Parse Links
	parseEach(ce_xml::link_count, parseEdge(store));
	parseLine(close(ce_xml::genome));
}


Cppn* CppnParser::parse(){
	parseHeader(true);

	if(tryParseLine(open(ce_xml::storage))){
		int read_min = util::toInt(m[2]);
		int read_max = util::toInt(m[1]);

		std::streampos beforeCount = myfile->tellg();

		int min = std::numeric_limits<int>::max();
		int max = std::numeric_limits<int>::min();
		while(tryParseLine(open(ce_xml::generation))){
			int nr = util::toInt(m[1]);
			if(nr < min) min = nr;
			if(nr > max) max = nr;
			parseGenome(false);
			parseLine(close(ce_xml::generation));
		}

		if(read_min != min) widget->warning("There exist generations smaller than the minimum generation in this file. File might be corrupted.");
		if(read_max != max) widget->warning("There exist generations greater than the maximum generation in this file. File might be corrupted.");


		myfile->seekg(beforeCount);
		nextLine=true;


		int generationNr = widget->getGeneration(min, max);
		while(tryParseLine(open(ce_xml::generation))){
//			int nr = boost::lexical_cast<int>(m[1].str());
//			int nr = boost::lexical_cast<int>(m[1]);
			int nr = util::toInt(m[1]);
			if(nr == generationNr) break;
			parseGenome(false);
			parseLine(close(ce_xml::generation));
		}
	}

	parseGenome(true);

	return cppn;
}


