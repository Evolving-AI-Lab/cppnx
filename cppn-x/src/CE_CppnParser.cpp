/*
 * CE_CppnParser.cpp
 *
 *  Created on: May 31, 2013
 *      Author: joost
 */

#include "CE_CppnParser.h"
#include "CE_Xml.h"
#include "CE_Util.h"
#include <assert.h>
#include <limits>
#include <zip.h>




//#define parseEach(template_str,parser) while(parseCount(template_str)){parser;} parseLine(closeXml(template_str));

CppnParser::CppnParser(std::string stdFileName):data_version(""), line(""), nextLine(true), lineNumber(0), parseCounter(0){
    dbg::trace trace("parser", DBG_HERE);
    QString qFileName(stdFileName.c_str());
    QFileInfo fileInfo(qFileName);

    fileInformation = new FileInformation();
    value = 0;
    hue = 0;
    saturation = 0;

    if(!fileInfo.exists()){
        throw CeParseException(QString("File does not exist: " + qFileName).toStdString());
    }

	if(fileInfo.suffix() == QString("zip")){
	    dbg::out(dbg::info, "parser") << "CE_CppnParser.hpp: Unzipping file: " << qFileName.toStdString() << std::endl;
	    QByteArray ba = qFileName.toLocal8Bit();
		//Open the ZIP archive
	    dbg::out(dbg::info, "parser") << "  Open archive" << std::endl;
		int err = 0;
		zip *z = zip_open(ba.data(), 0, &err);
		dbg::out(dbg::info, "parser") << "  Open archive done" << std::endl;

		//Search for the file of given name
		dbg::out(dbg::info, "parser") << "  Zip stats" << std::endl;
		const char *name = "a";
		struct zip_stat st;
		zip_stat_init(&st);
		zip_stat(z, name, 0, &st);
		dbg::out(dbg::info, "parser") << "  Zip stats done" << std::endl;

		//Alloc memory for its uncompressed contents
		dbg::out(dbg::info, "parser") << "  Allocate memory: " << st.size << " characters" << std::endl;
		char *contents = new char[st.size+1];
		dbg::out(dbg::info, "parser") << "  Allocate memory done" << std::endl;

		//Read the compressed file
		dbg::out(dbg::info, "parser") << "  Read compressed" << std::endl;
		dbg::out(dbg::info, "parser") << "    Open" <<std::endl;
		zip_file *f = zip_fopen(z, "a", 0);
		dbg::out(dbg::info, "parser") << "    Open done. Pointer: " << f << std::endl;
		dbg::out(dbg::info, "parser") << "    Read: " << st.size << " characters" <<std::endl;
		int read_err = zip_fread(f, contents, st.size);
		contents[st.size] = '\0';
		dbg::out(dbg::info, "parser") << "    Read done. Exit code: " << read_err <<std::endl;
		dbg::out(dbg::info, "parser") << "    Close" <<std::endl;
		int close_err = zip_fclose(f);
		dbg::out(dbg::info, "parser") << "    Close done. Exit code: " << close_err << std::endl;
		uint i;
		for(i=0; i<st.size; ++i){
		    if(contents[i] == '\0'){
		        dbg::out(dbg::info, "parser") << "Null found at: " << i <<std::endl;
		    }
		}
        if(contents[i] == '\0'){
            dbg::out(dbg::info, "parser") << "Null found at: " << i <<std::endl;
        }
        dbg::out(dbg::info, "parser") << "    Create string" <<std::endl;
		std::string contentStr(contents);
		dbg::out(dbg::info, "parser") << "    Create string done. Length: " << contentStr.size() << std::endl;
		dbg::out(dbg::info, "parser") << "  Read compressed done" << std::endl;

		//And close the archive
		zip_close(z);
		myfile = new std::istringstream(contentStr);
		delete [] contents;
		dbg::out(dbg::info, "parser")<< "Unzipping file done" << std::endl;
	} else {
        QByteArray ba = qFileName.toLocal8Bit();
        myfile = new std::ifstream(ba.data());
	}
}

CppnParser::~CppnParser(){
    dbg::trace trace("parser", DBG_HERE);
    foreach(Label* label, labels){
        label->unregisterObject();
    }

    foreach(Node* node, nodes){
        delete node;
    }
    nodes.clear();

    foreach(Edge* edge, edges){
        delete edge;
    }
    edges.clear();

    foreach(NodeView* nodeview, nodeviews){
        delete nodeview;
    }
    nodeviews.clear();

	if(myfile) delete myfile;
	myfile = 0;

    if(fileInformation) delete fileInformation;
    fileInformation = 0;
}


//std::string openClose(std::string template_str){
//	return std::string(ce_xml::getOpenXmlString(template_str) + "*" + ce_xml::getCloseXmlString(template_str));
//}
//
//std::string open(std::string template_str){
//	return std::string(ce_xml::getOpenXmlString(template_str, "*"));
//}
//
//std::string read(std::string template_str){
//	return std::string(ce_xml::getOneLineXmlString(template_str, "*"));
//}
//
//std::string close(std::string template_str){
//	return std::string(ce_xml::getCloseXmlString(template_str));
//}


bool CppnParser::parseCount(std::string template_str){
    dbg::trace trace("parser", DBG_HERE);
	std::string regex  = openXml(template_str);
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

void CppnParser::parseWhiteSpace(std::string::iterator& it){
    dbg::trace trace("parser", DBG_HERE);
	while((*it) == ' ') it++;
}

bool CppnParser::parseExpected(const std::string& line, std::string::iterator& currentChar,const std::string& expected, std::string::iterator& expectedChar){
    dbg::trace trace("parser", DBG_HERE);
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
    dbg::trace trace("parser", DBG_HERE);
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

bool CppnParser::parseLine(std::string line, std::string expected){
    dbg::trace trace("parser", DBG_HERE);
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

bool CppnParser::parseLine(std::string regex, bool stopOnFail){
    dbg::trace trace("parser", DBG_HERE);
//	std::cout << "Parsing line: " << util::toString(lineNumber) << " regex: " << regex <<std::endl;
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


bool CppnParser::parseLine(std::vector<std::string> regex, bool stopOnFail){
    dbg::trace trace("parser", DBG_HERE);
    bool success = false;
    for(size_t i=0; i< regex.size(); ++i){
        if(parseLine(regex[i], false)){
            success = true;
            break;
        }
    }
    if(stopOnFail && !success) {
        std::string expected;
        for(size_t i=0; i< regex.size(); ++i){
            expected+=regex[i];
            expected+="\n";
        }
        throw CeParseException("Parse error on line: " + util::toString(lineNumber) + ".\nRead: "+ line + "\nExpected: " + expected);
    }
    return success;
}

//void CppnParser::parseLine(std::string regex){
////	std::cout << "Parsing line: " << util::toString(lineNumber) << " regex: " << regex <<std::endl;
//	if(!myfile->good()) throw CeParseException("Unexpected end of file.");
//	if(nextLine){
//		getline (*myfile,line);
//		lineNumber++;
//	}
//	if(parseLine(line, regex)){
//		nextLine=true;
//	} else {
//		throw CeParseException("Parse error on line: " + util::toString(lineNumber) + ".\nRead: "+ line + "\nExpected: " + regex);
//	}
//}

void CppnParser::parseLine(std::string regex, bool stopOnFail, std::vector<std::string> &tokens, size_t index, std::string defaultValue, std::string separetor, std::string minVersion, std::string maxVersion){
    dbg::trace trace("parser", DBG_HERE);
//	std::cout << "Parsing line: " << util::toString(lineNumber) << " regex: " << regex << " index: " << index << " of: " << tokens.size() << " default: \"" <<  defaultValue << "\" curVersion: " << data_version << " minVersion: " << minVersion << " maxVersion: " << maxVersion <<std::endl;

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

//		if(result == "") result = "\"\"";
		tokens[index] = result;
	}else{
		if(tokens[index]=="") tokens[index] = defaultValue;
	}
}

void CppnParser::copyTo(std::vector<std::string> &tokens, size_t from, size_t to, std::string fromSeparetor, std::string toSeparetor, std::string minVersion, std::string maxVersion){
    dbg::trace trace("parser", DBG_HERE);
//	std::cout << "Parsing line: " << util::toString(lineNumber) << " regex: " << regex << " index: " << index << " of: " << tokens.size() << " default: " <<  defaultValue << " minVersion: " << minVersion << " maxVersion: " << maxVersion <<std::endl;


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

void CppnParser::toStream(std::vector<std::string> &tokens, std::iostream &stream){
    dbg::trace trace("parser", DBG_HERE);
//	std::cout << "To stream. Tokens: " << tokens.size() << std::endl;
	stream << std::setprecision(17);
	for(size_t i=0; i<tokens.size(); i++){
		dbg::out(dbg::info, "parser") << "Token " << i << ": " << tokens[i] << std::endl;
		stream <<tokens[i] << " ";
	}

//	std::cout << "To stream end." <<std::endl;
}


void CppnParser::parseHeader(std::vector<std::string> &tokens){
    dbg::trace trace("parser", DBG_HERE);
	data_version = "0.0";
	if(!parseLine(ce_xml::getFirstLine(), false)) parseLine("<?xml*?>");
	parseLine(readXml(ce_xml::cppn_data), false, tokens, cppnxDataVersion, "0.0");
	parseLine(openXml(ce_xml::data), false, tokens, picBreederDataVersion, "1.0");
	data_version = tokens[cppnxDataVersion];
}

void CppnParser::parseNodeView(bool store){
    dbg::trace trace("parser", DBG_HERE);
	std::stringstream stream;
	std::vector<std::string> tokens(nodeviewSize);

	parseLine(openXml(ce_xml::nodeview));
	parseLine(readXml(ce_xml::identifier), true,tokens, nodeviewIdentifier);
	parseLine(closeXml(ce_xml::nodeview));
	toStream(tokens, stream);

	if(store){
	    dbg::out(dbg::info, "parser") << "Storing node view" << std::endl;
		if(tokens[nodeviewIdentifier] == " final"){
			FinalNodeView* finalNodeview = new FinalNodeView();
			finalNodeview->setValueNode(value);
			if(hue) finalNodeview->setHueNode(hue);
			if(saturation) finalNodeview->setSaturationNode(saturation);
			nodeviews.append(finalNodeview);
		} else {
			nodeviews.append(new NodeView(stream, nodeMap));
		}
	}
}

void CppnParser::parseColorButton(bool store){
    dbg::trace trace("parser", DBG_HERE);
	std::stringstream stream;
	std::vector<std::string> tokens(labelSize);

	parseLine(openXml(ce_xml::color_button));
	parseLine(readXml(ce_xml::color_label), true,tokens, labelid, "", " ", "1.1");
	parseLine(openCloseXml(ce_xml::text), true,tokens, labelname, "\"\"");
	parseLine(readXml(ce_xml::color), true,tokens, rgb, "255 255 255");
	parseLine(closeXml(ce_xml::color_button));
	copyTo(tokens, rgb, labelid, " ", "_", "1.0", "1.0");
	tokens[labelname] = "\"" + tokens[labelname] + "\"";

	toStream(tokens, stream);

	if(store){
	    dbg::out(dbg::info, "parser") << "Storing label" << std::endl;
		Label* labelWidget = new Label(stream);
		labelMap[tokens[labelid]] = labelWidget;
		labels.append(labelWidget);
		labelWidget->registerObject();
	}
}


void CppnParser::parseNode(bool store){
    dbg::trace trace("parser", DBG_HERE);
	std::stringstream stream;
	std::vector<std::string> tokens(nodeSize);

	if(parseLine(openXml(ce_xml::node), false)){
		tokens[affinity] = "grey";
		tokens[bias] = "0.0";
		tokens[special] = "";
		tokens[type] = m[1];
	} else if(parseLine(openXml(ce_xml::ionode), false)){
		tokens[affinity] = "grey";
		tokens[bias] = "0.0";
		tokens[special] = m[1];
		tokens[type] = m[2];
	} else if(parseLine(openXml(ce_xml::colornode), false)){
		tokens[affinity] = m[1];
		tokens[bias] = m[2];
		tokens[special] = "";
		tokens[type] = m[3];
	} else{
		parseLine(openXml(ce_xml::iocolornode));
		tokens[affinity]  = m[1];
		tokens[bias] = m[2];
		tokens[special] = m[3];
		tokens[type] = m[4];
	}

	if(tokens[affinity] == "") tokens[affinity] = "grey";
	if(tokens[bias] == "") tokens[bias] = "0.0";
	if(tokens[type] == "") tokens[type] = "hidden";

	parseLine(readXml(ce_xml::marking), true, tokens, nodeIdentifier);
	parseLine(openCloseXml(ce_xml::activation), true, tokens, activationFunction);
	parseLine(readXml(ce_xml::color), true, tokens, nodeLabel,"255_255_255", "_", "1.0", "1.0");
	parseLine(readXml(ce_xml::color_label), true, tokens, nodeLabel,"","", "1.1");
	parseLine(readXml(ce_xml::position), true, tokens, position,"0.0 0.0", " ", "1.0");
	parseLine(openCloseXml(ce_xml::text), true, tokens, nodeNote,"", " ", "1.1");
	parseLine(closeXml(ce_xml::node));

	tokens[special] = "\"" + tokens[special] + "\"";
	tokens[nodeNote] = "\"" + tokens[nodeNote] + "\"";

	toStream(tokens, stream);
//	std::cout << stream.str() << std::endl;

	if(store){
	    dbg::out(dbg::info, "parser") << "Storing node" << std::endl;
		Node* node = new Node(stream, labelMap);
		nodes.append(node);
		nodeMap[node->getBranch() + "_" + node->getId()] = node;

		if(node->getXmlLabel() == OUTPUT_INK) value = node;
		if(node->getXmlLabel() == OUTPUT_BRIGTHNESS) value = node;
		if(node->getXmlLabel() == OUTPUT_SATURATION) saturation = node;
		if(node->getXmlLabel() == OUTPUT_HUE) hue = node;

	}
}

void CppnParser::parseEdge(bool store){
    dbg::trace trace("parser", DBG_HERE);
	std::stringstream stream;
	std::vector<std::string> tokens(edgeSize);

	//Parse first line
	parseLine(openXml(ce_xml::link));
	parseLine(readXml(ce_xml::marking), true,tokens, marking);
	parseLine(readXml(ce_xml::source), true,tokens, source, "", "_");
	parseLine(readXml(ce_xml::target), true,tokens, target, "", "_");
	parseLine(openCloseXml(ce_xml::weight), true,tokens, weight);
	parseLine(readXml(ce_xml::color), true,tokens, label, "empty", "_", "1.0", "1.0");
	parseLine(openCloseXml(ce_xml::original_weight), true,tokens, originalWeight, tokens[weight], " ", "1.1");
	parseLine(readXml(ce_xml::color_label), true,tokens, label, "", "", "1.1");
	parseLine(openCloseXml(ce_xml::text), true,tokens, note, "\"\"", "", "1.1");
	parseLine(readXml(ce_xml::bookends), true,tokens, bookends, "-3.0 3.0 0.1", " ", "1.2");
	parseLine(closeXml(ce_xml::link));

	tokens[note] = "\"" + tokens[note] + "\"";

	toStream(tokens, stream);
	if(store){
	    dbg::out(dbg::info, "parser") << "Storing edge" << std::endl;
	    edges.append(new Edge(stream, nodeMap, labelMap));
	}
}

void CppnParser::parseParent(bool store){
    dbg::trace trace("parser", DBG_HERE);
	parseLine(readXml(ce_xml::identifier));
	if(store) fileInformation->addParent(m[1], m[2]);
}

void CppnParser::parseGenome(bool store, std::vector<std::string> &tokens){
    dbg::trace trace("parser", DBG_HERE);
	parseLine(openXml(ce_xml::genome), false, tokens, genome, "unknown");
	tokens[genome] = tokens[genome] + " grey";
	parseLine(openXml(ce_xml::genomePhen), false, tokens, genome, "unknown grey");
	parseLine(readXml(ce_xml::identifier), true, tokens, genomeIdentifier, "unknown unknown");
	//Set parents
	if(!parseLine(readXml(ce_xml::parent_count), false)){
		parseEach(ce_xml::parent_count, parseParent(store));
	}

	if(data_version >= "1.0"){
		parseEach(ce_xml::buttons_count, parseColorButton(store));
	}

	parseEach(ce_xml::nodes_count, parseNode(store));
	parseEach(ce_xml::link_count, parseEdge(store));

	if(data_version >= "1.2"){
		parseEach(ce_xml::nodeviews_count, parseNodeView(store));
	}

	parseLine(closeXml(ce_xml::genome));
}


void CppnParser::parse(int generation){
    dbg::trace trace("parser", DBG_HERE);
    dbg::out(dbg::info, "parser") << "Parsing file" << std::endl;

	std::stringstream stream;
	std::vector<std::string> tokens(fileInformationSize);

//	labelMap["empty"] = new Label();

	parseHeader(tokens);

	if(parseLine(openXml(ce_xml::storage), false)){
//		int read_min = util::toInt(m[2]);
//		int read_max = util::toInt(m[1]);

		std::streampos beforeCount = myfile->tellg();

		int min = std::numeric_limits<int>::max();
		int max = std::numeric_limits<int>::min();
		std::vector<std::string> dummytokens(fileInformationSize);

		while(parseLine(openReadXml(ce_xml::generation), false)){
			int nr = util::toInt(m[1]);
            int size = util::toInt(m[2]);
            if(size == 0) continue;
			if(nr < min) min = nr;
			if(nr > max) max = nr;
			parseGenome(false, dummytokens);
			parseLine(closeXml(ce_xml::generation), false);
		}

		/*
		if(read_min != min){
			QMessageBox msgBox(QMessageBox::Warning, QMessageBox::tr("Warning"), QMessageBox::tr("There exist generations smaller than the minimum generation in this file. File might be corrupted."), QMessageBox::Ok, 0);
			msgBox.exec();
		}
		if(read_max != max){
			QMessageBox msgBox(QMessageBox::Warning, QMessageBox::tr("Warning"), QMessageBox::tr("There exist generations greater than the maximum generation in this file. File might be corrupted."), QMessageBox::Ok, 0);
			msgBox.exec();
		}
		 */

		myfile->seekg(beforeCount);
		nextLine=true;

		if(generation == -1){
			bool ok = true;
			std::string text = "This files contains multiple generations from " + util::toString(min) + " to " + util::toString(max) + "\nPlease select a generation to display.";
			generation =  QInputDialog::getInt(0, QInputDialog::tr("Select Generation"), QInputDialog::tr(text.c_str()), 0, min, max, 1, &ok);
			if(!ok) throw CeParseException("Parsing canceled.");
		}

		while(parseLine(openReadXml(ce_xml::generation), false)){
			int nr = util::toInt(m[1]);
			int size = util::toInt(m[2]);
			if(size == 0) continue;
			if(nr == generation) break;
			parseGenome(false, dummytokens);
			parseLine(closeXml(ce_xml::generation), false);
		}
	}

	parseGenome(true, tokens);
	toStream(tokens, stream);

	dbg::out(dbg::info, "parser") << "Initializing file information from stream..." << std::endl;
	fileInformation->init(stream);

//	std::cout << value << std::endl;

	if(data_version <= "1.1"){
	    dbg::out(dbg::info, "parser") << "Working with data prior to version 1.2" << std::endl;
		FinalNodeView* finalNodeview = new FinalNodeView();
		finalNodeview->setValueNode(value);
		if(hue) finalNodeview->setHueNode(hue);
		if(saturation) finalNodeview->setSaturationNode(saturation);
		nodeviews.append(finalNodeview);
	}

	dbg::out(dbg::info, "parser") << "Finished parsing." << std::endl;

//	return cppn;
}


