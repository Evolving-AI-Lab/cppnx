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
#include "CE_ColorButton.h"
#include <iostream>
#include <iomanip>

class Cppn;
class Node;
class Edge;
class CE_ColorButton;

class CppnWriter {
public:
	CppnWriter(std::string filename){
//		QFile file("file.xxx");
//		file.open(QIODevice::WriteOnly);
//		output.setDevice(&file);

		output.open(filename.c_str());
	}

	void write(shared_ptr<Cppn> cppn);

private:
	void writeNode(Node* node);
	void writeEdge(Edge* node);
	void writeColorButton(CE_ColorButton* colorButton);

	std::ofstream output;
//	 QDataStream output;

	static const std::string version;
};


#endif /* CE_CPPNWRITER_H_ */
