/*
 * CE_CppnWriter.h
 *
 *  Created on: May 30, 2013
 *      Author: joost
 */

#ifndef CE_CPPNWRITER_H_
#define CE_CPPNWRITER_H_

#include "CE_Cppn.h"
#include "CE_Defines.h"
#include "CE_Label.h"
#include "CX_FileInformation.h"

#include <iostream>
#include <iomanip>

class Cppn;
class Node;
class Edge;
class Label;
class NodeView;

class CppnWriter {
public:
	CppnWriter(std::string filename){
//		QFile file("file.xxx");
//		file.open(QIODevice::WriteOnly);
//		output.setDevice(&file);
		indentCt = 0;
		output.open(filename.c_str());
	}

	void write(Cppn* cppn, QList<Label*> labels, FileInformation* fileInformation, QList<NodeView*> nodeviews);

private:

	void open(std::string template_str){
		open(template_str, "", "", "", "");
	}

	template <typename Param1>
	void open(std::string template_str, Param1 param1){
		open(template_str, param1, "", "", "");
	}

	template <typename Param1, typename Param2>
	void open(std::string template_str, Param1 param1, Param2 param2){
		open(template_str, param1, param2, "", "");
	}

	template <typename Param1, typename Param2, typename Param3>
	void open(std::string template_str, Param1 param1, Param2 param2, Param3 param3){
		open(template_str, param1, param2, param3, "");
	}

	template <typename Param1, typename Param2, typename Param3, typename Param4>
	void open(std::string template_str, Param1 param1, Param2 param2, Param3 param3, Param4 param4);
//	void open(std::string template_str, std::string param1 = "", std::string param2= "", std::string param3="", std::string param4="");
//	void open(std::string template_str, size_t param1);
	void close(std::string template_str);


	void write(std::string template_str){
		write(template_str, "", "", "", "");
	}
	template <typename Param1>
	void write(std::string template_str, Param1 param1){
		write(template_str, param1, "", "", "");
	}
	template <typename Param1, typename Param2>
	void write(std::string template_str, Param1 param1, Param2 param2){
		write(template_str, param1, param2, "", "");
	}
	template <typename Param1, typename Param2, typename Param3>
	void write(std::string template_str, Param1 param1, Param2 param2, Param3 param3){
		write(template_str, param1, param2, param3, "");
	}
	template <typename Param1, typename Param2, typename Param3, typename Param4>
	void write(std::string template_str, Param1 param1, Param2 param2, Param3 param3, Param4 param4);

//	void write(std::string template_str, std::string param1 = "", std::string param2= "", std::string param3="", std::string param4="");
//	void openClose(std::string template_str, std::string param);
	template <typename ParamType>
	void openClose(std::string template_str, ParamType param);
//	void writeCppn(Cppn* cppn);
	void writeNode(Node* node);
	void writeEdge(Edge* node);
	void writeColorButton(Label* colorButton);
	void writeNodeview(NodeView* nodeview);

	std::ofstream output;
//	 QDataStream output;

	static const std::string version;

	size_t indentCt;
};


#endif /* CE_CPPNWRITER_H_ */
