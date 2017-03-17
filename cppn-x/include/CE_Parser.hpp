/*
 * CE_Parser.hpp
 *
 *  Created on: Nov 11, 2014
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CE_PARSER_HPP_
#define CPPN_X_INCLUDE_CE_PARSER_HPP_

//Other
#include <zip.h>

//QT
#include <QString>
#include <QFileInfo>

//Local
#include "CE_Xml.h"
#include "CX_Debug.hpp"

#define parseEach(template_str,parser) while(parseCount(template_str)){parser;} parseLine(closeXml(template_str));

std::string openCloseXml(std::string template_str);
std::string openXml(std::string template_str);
std::vector<std::string> openReadXml(std::string template_str);
std::string readXml(std::string template_str);
std::string closeXml(std::string template_str);

class CeParseException : public std::exception
{
	std::string text;

public:

	CeParseException(const std::string &_reason){
		text = _reason;
	}

	~CeParseException() throw (){

	}

	virtual const char* what() const throw(){
		return text.c_str();
	}
};

class Parser{
public:
	Parser(QString fileName): line(""), nextLine(true), lineNumber(0), parseCounter(0){
		QFileInfo fileInfo(fileName);
//		std::cout << "Filename: " << fileName.toStdString() << " exists: " << fileInfo.exists() << std::endl;

		if(!fileInfo.exists()){
			throw CeParseException(QString("File does not exist: " + fileName).toStdString());
		}

		if(fileInfo.suffix() == QString("zip")){
		    dbg::out(dbg::info, "parser") << "CE_Parser.hpp: Unzipping file: " << fileName.toStdString() << std::endl;
			QByteArray ba = fileName.toLocal8Bit();

			//Open the ZIP archive
			int err = 0;
			dbg::out(dbg::info, "parser") << "  Open archive" << std::endl;
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
			dbg::out(dbg::info, "parser") << "  Allocate memory" << std::endl;
			char *contents = new char[st.size+1];
			dbg::out(dbg::info, "parser") << "  Allocate memory done" << std::endl;

			//Read the compressed file
			dbg::out(dbg::info, "parser") << "  Read compressed" << std::endl;
			zip_file *f = zip_fopen(z, "a", 0);
			zip_fread(f, contents, st.size);
			contents[st.size] = '\0';
			zip_fclose(f);
			std::string contentStr(contents);
			dbg::out(dbg::info, "parser") << "  Read compressed done" << std::endl;

			//And close the archive
			zip_close(z);
			myfile = new std::istringstream(contentStr);
			delete [] contents;
			dbg::out(dbg::info, "parser") << "Unzipping file done" << std::endl;
		} else {
			QByteArray ba = fileName.toLocal8Bit();
			myfile = new std::ifstream(ba.data());
		}
	}

	virtual ~Parser(){
		delete myfile;
	}

protected:
	//Pointer to the file we are currently parsing
	std::istream* myfile;

	//String containing the current line that we are trying to parse
	std::string line;

	bool nextLine;
	size_t lineNumber;
	int parseCounter;

	//Vector that will hold the tokens parsed by the parsLine function.
	std::vector<std::string> m;
};


#endif /* CPPN_X_INCLUDE_CE_PARSER_HPP_ */
