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
#include <fstream>
#include <map>
#include <stdio.h>
#include <string>
#include <sstream>

#include "CE_Parser.hpp"
#include "CE_Cppn.h"
#include "CE_CppnWidget.h"
#include "CE_Label.h"
#include "CX_FileInformation.h"

//class CeParseException : public std::exception
//{
//	std::string text;
//
//public:
//
//	CeParseException(const std::string &_reason){
//		text = _reason;
//	}
//
//	~CeParseException() throw (){
//
//	}
//
//	virtual const char* what() const throw(){
//		return text.c_str();
//	}
//};



typedef std::map<std::string, std::string> io_map_t;


class CppnParser
{
public:
	CppnParser(std::string fileName);
	virtual ~CppnParser();

	void parse(int generation = -1);

//	Cppn* getCppn(){
//		return cppn;
//	}

	/**
	 * Returns a list of pointers to labels.
	 */
	QList<Label*> getLabels(){
		return labels;
	}

    /**
     * Returns a list of pointers to nodes.
     *
     * Does not transfer ownership, as soon as the parser gets deleted,
     * all pointers will be invalidated.
     */
	QList<Node*> getNodes(){
		return nodes;
	}

    /**
     * Returns a list of pointers to edges.
     *
     * Does not transfer ownership, as soon as the parser gets deleted,
     * all pointers will be invalidated.
     */
	QList<Edge*> getEdges(){
		return edges;
	}

    /**
     * Returns a list of pointers to node views.
     *
     * Does not transfer ownership, as soon as the parser gets deleted,
     * all pointers will be invalidated.
     */
	QList<NodeView*> getNodeviews(){
		return nodeviews;
	}

    /**
     * Returns pointer to the file information.
     *
     * Does not transfer ownership, as soon as the parser gets deleted,
     * all pointers will be invalidated.
     */
	FileInformation* getFileInformation(){
		return fileInformation;
	}

    /**
     * Returns a list of pointers to nodes, and clears the list.
     *
     * Transfers ownership of the pointers to the caller,
     * all objects retrieved this way will not be destroyed by the parser.
     */
    QList<Node*> takeNodes(){
        QList<Node*> temp = nodes;
        nodes.clear();
        return temp;
    }

    /**
     * Returns a list of pointers to edges, and clears the list.
     *
     * Transfers ownership of the pointers to the caller,
     * all objects retrieved this way will not be destroyed by the parser.
     */
    QList<Edge*> takeEdges(){
        QList<Edge*> temp = edges;
        edges.clear();
        return temp;
    }

    /**
     * Returns a list of pointers to nodeviews, and clears the list.
     *
     * Transfers ownership of the pointers to the caller,
     * all objects retrieved this way will not be destroyed by the parser.
     */
    QList<NodeView*> takeNodeviews(){
        QList<NodeView*> temp = nodeviews;
        nodeviews.clear();
        return temp;
    }

    /**
     * Returns a pointer to file information and sets the pointer to 0.
     *
     * Transfers ownership of the pointer to the caller,
     * the object retrieved this way will not be destroyed by the parser.
     */
    FileInformation* takeFileInformation(){
        FileInformation* temp = fileInformation;
        fileInformation = 0;
        return temp;
    }


private:
	bool parseLine(std::string line, std::string expected);
	std::string parseParameter(const std::string& line, std::string::iterator& currentChar, std::string::iterator& expectedChar);
	bool parseExpected(const std::string& line, std::string::iterator& currentChar, const std::string& expected, std::string::iterator& expectedChar);
	void parseWhiteSpace(std::string::iterator& it);



//	void parseLine(std::string regex);
	bool parseLine(std::string regex, bool stopOnFail = true);
	bool parseLine(std::vector<std::string> regex, bool stopOnFail = true);
//	bool tryParseLine(std::string regex);
	void parseLine(std::string regex, bool stopOnFail, std::vector<std::string> &tokens, size_t index, std::string defaultValue = "", std::string separetor = " ", std::string minVersion = "0.0", std::string maxVersion = "1.2");
	void copyTo(std::vector<std::string> &tokens, size_t from, size_t to, std::string fromSeparetor = " ", std::string toSeparetor = " ", std::string minVersion = "0.0", std::string maxVersion = "1.2");


	bool parseCount(std::string template_str);
	void parseHeader(std::vector<std::string> &tokens);
	void parseNode(bool store);
	void parseEdge(bool store);

	void parseNodeView(bool store);
	void parseColorButton(bool store);
	void parseGenome(bool store, std::vector<std::string> &tokens);
	void parseParent(bool store);
	void toStream(std::vector<std::string> &tokens, std::iostream &stream);

	void toStream(std::string tokens[], std::iostream &stream);

	enum edge{
		label = 0,
		note,
		marking,
		source,
		target,
		weight,
		originalWeight,
		bookends,
		edgeSize
	};

	enum node{
		nodeLabel = 0,
		nodeNote,
		affinity,
		bias,
		special,
		type,
		nodeIdentifier,
		activationFunction,
		position,
		nodeSize
	};

	enum label{
		labelname= 0,
		rgb,
		labelid,
		labelSize
	};

	enum fileInformation{
		cppnxDataVersion= 0,
		picBreederDataVersion,
		genome,
		genomeIdentifier,
		fileInformationSize
	};

	enum nodeview{
		nodeviewIdentifier = 0,
		nodeviewSize
	};

	std::istream* myfile;
//	Cppn* cppn;
	FileInformation* fileInformation;
	QList<Label*> labels;
	QList<Node*> nodes;
	QList<NodeView*> nodeviews;
	QList<Edge*> edges;

	Node* value;
	Node* hue;
	Node* saturation;

	std::vector<std::string> m;

	std::string data_version;
	std::string line;
	bool nextLine;
	size_t lineNumber;

	int parseCounter;

//	std::map<std::string, Label*> oldLabelMap;
	std::map<std::string, Label*> labelMap;
    std::map<std::string, Node*> nodeMap;

};

#endif /* CPPNPARSER_H_ */
