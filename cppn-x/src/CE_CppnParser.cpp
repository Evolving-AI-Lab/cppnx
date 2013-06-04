/*
 * CE_CppnParser.cpp
 *
 *  Created on: May 31, 2013
 *      Author: joost
 */

#include "CE_CppnParser.h"

const boost::regex	CppnParser::any= boost::regex(".*");

const boost::regex	CppnParser::first_line = boost::regex("<\?xml.*\?>");
const boost::regex	CppnParser::ionode= boost::regex("\\s*<node label=\"(.*?)\" type=\"(.*?)\">");
const boost::regex	CppnParser::node= boost::regex("\\s*<node type=\"(.*?)\">");
const boost::regex	CppnParser::link= boost::regex("\\s*<link>");

const boost::regex	CppnParser::marking= boost::regex("\\s*<marking branch=\"(\\d*)\" id=\"(\\d*)\"/>");
const boost::regex	CppnParser::activation= boost::regex("\\s*<activation>(.*)</activation>");
const boost::regex	CppnParser::color= boost::regex("\\s*<color>(\\d*) (\\d*) (\\d*)</color>");
const boost::regex	CppnParser::position= boost::regex("\\s*<position>(.*) (.*)</position>");

const boost::regex	CppnParser::source= boost::regex("\\s*<source branch=\"(\\d*)\" id=\"(\\d*)\"/>");
const boost::regex	CppnParser::target= boost::regex("\\s*<target branch=\"(\\d*)\" id=\"(\\d*)\"/>");
const boost::regex	CppnParser::weight= boost::regex("\\s*<weight>(.*)</weight>");


const boost::regex	CppnParser::cppn_data= boost::regex("\\s*<cppn-data version=\"(.*)\"/>");
const boost::regex	CppnParser::nodes_count= boost::regex("\\s*<nodes count=\"(.*)\">");
const boost::regex	CppnParser::link_count= boost::regex("\\s*<links count=\"(.*)\">");

const boost::regex	CppnParser::buttons_count= boost::regex("\\s*<buttons count=\"(.*)\">");
const boost::regex	CppnParser::text= boost::regex("\\s*<text>(.*)</text>");

const boost::regex	CppnParser::count= boost::regex(".*count=\"(.*)\".*");

const boost::regex	CppnParser::data= boost::regex("\\s*<data version=\"(.*)\">");


bool CppnParser::parseCount(boost::regex regex){
	if(parseCounter>1){
		parseCounter--;
		return true;
	} else if(parseCounter==1){
		parseCounter=0;
		return false;
	} else{
		parseLine(regex);
		parseCounter = lexical_cast<int>(m[1].str());
		return parseCounter>0;
	}
}

void CppnParser::parseLine(boost::regex regex){
	if(!myfile.good()) throw JGTL::LocatedException("Unexpected end of file.");
	if(nextLine){
		getline (myfile,line);
		lineNumber++;
	}
	if(regex_search(line, m, regex)){
		nextLine=true;
	} else {
		throw JGTL::LocatedException("Parse error on line: " + boost::lexical_cast<std::string>(lineNumber) + " :"+ line);
	}
}

bool CppnParser::tryParseLine(boost::regex regex){
	if(!myfile.good()) throw JGTL::LocatedException("Unexpected end of file.");
	if(nextLine){
		getline (myfile,line);
		lineNumber++;
	}
	if(regex_search(line, m, regex)){
		nextLine=true;
		return true;
	} else {
		nextLine=false;
		return false;
	}
}


void CppnParser::parseHeader(){
	//Parse first line
	parseLine(first_line);
	cppn->addHeaderLine(line);

	if(tryParseLine(cppn_data)){
		data_version = m[1].str();
		cppn->setNewFile(false);
	} else {
		data_version = "raw";
		cppn->setNewFile(true);
	}

	if(tryParseLine(data)){
		cppn->addHeaderLine(line);
	}

	parseLine(any);
	cppn->addHeaderLine(line);

	parseLine(any);
	cppn->addHeaderLine(line);

	while(parseCount()){
		cppn->addHeaderLine(line);
		parseLine(any);
	}

	cppn->addHeaderLine(line);

	parseLine(any);
	cppn->addHeaderLine(line);
}

void CppnParser::parseColorButton(){
	std::string text_str;
	int r;
	int g;
	int b;
	QColor color_q(0, 0, 0);

	parseLine(any);
	parseLine(text);
	text_str = m[1].str();

	parseLine(color);
	r = lexical_cast<int>(m[1].str());
	g = lexical_cast<int>(m[2].str());
	b = lexical_cast<int>(m[3].str());
	color_q = QColor (r, g, b);

	parseLine(any);

	cppn->addColorButton(text_str, color_q);
}

void CppnParser::parseNode(){
	std::string type;
	std::string label;
	std::string branch;
	std::string id;
	std::string activation_function;
	QColor color_q(0, 0, 0);
	QPointF position_q(0, 0);
	int r;
	int g;
	int b;
	double x;
	double y;

	if(tryParseLine(node)){
		type = m[1].str();
	} else {
		parseLine(ionode);
		label = m[1].str();
		type = m[2].str();
	}

	parseLine(marking);
	branch = m[1].str();
	id = m[2].str();

	parseLine(activation);
	activation_function = m[1].str();

	if(data_version == "1.0"){
		parseLine(color);
		r = lexical_cast<int>(m[1].str());
		g = lexical_cast<int>(m[2].str());
		b = lexical_cast<int>(m[3].str());
		color_q = QColor (r, g, b);

		parseLine(position);
		x = lexical_cast<double>(m[1].str());
		y = lexical_cast<double>(m[2].str());
		position_q = QPointF (x, y);

	}

	parseLine(any);

	cppn->addNode(branch, id, type, activation_function, label, position_q, color_q);
}

void CppnParser::parseEdge(){
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

	//Parse first line
	parseLine(any);

	parseLine(marking);
	branch = m[1].str();
	id = m[2].str();

	parseLine(source);
	source_branch = m[1].str();
	source_id = m[2].str();

	parseLine(target);
	target_branch = m[1].str();
	target_id = m[2].str();

	parseLine(weight);
	weight_f = lexical_cast<double>(m[1].str());

	if(data_version == "1.0"){
		parseLine(color);
		r = lexical_cast<int>(m[1].str());
		g = lexical_cast<int>(m[2].str());
		b = lexical_cast<int>(m[3].str());
		color_q = QColor (r, g, b);
	}

	parseLine(any);

	cppn->addConnection(branch, id, source_branch, source_id, target_branch, target_id, weight_f, color_q);
}


void CppnParser::parseFooter(){
	while(myfile.good()){
		parseLine(any);
		cppn->addFooterLine(line);
	}
}


shared_ptr<Cppn> CppnParser::parse(std::string fileName, GraphWidget* widget){
	cppn = shared_ptr<Cppn>(new Cppn(widget));
	myfile.open(fileName.c_str());

	parseHeader();


	if(data_version == "1.0"){
		parseLine(buttons_count);
		int nr_of_buttons = lexical_cast<int>(m[1].str());
		for(int i=0; i<nr_of_buttons; i++){
			parseColorButton();
		}
		parseLine(any);
	}


	parseLine(nodes_count);
	int nr_of_nodes = lexical_cast<int>(m[1].str());
	for(int i=0; i<nr_of_nodes; i++){
		parseNode();
	}
	parseLine(any);

	parseLine(link_count);
	int nr_of_edges = lexical_cast<int>(m[1].str());

	for(int i=0; i<nr_of_edges; i++){
		parseEdge();
	}
	parseFooter();

	return cppn;
}


