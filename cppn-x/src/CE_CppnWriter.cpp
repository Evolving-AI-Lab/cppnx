/*
 * CE_CppnWriter.cpp
 *
 *  Created on: Jun 2, 2013
 *      Author: joost
 */

#include "CE_CppnWriter.h"
#include <iostream>
#include <iomanip>

const std::string CppnWriter::version = "1.0";


std::string indent(size_t nr){
	return std::string (nr*4, ' ');
}

void CppnWriter::write(shared_ptr<Cppn> cppn){
	output << cppn->getHeaderLine(0) << "\n";
	output << "<cppn-data version=\"" << version << "\"/>\n";
	for(size_t i=1; i<cppn->getNrOfHeaderLines(); i++){
		output << cppn->getHeaderLine(i) << "\n";
	}

	output << indent(2) << "<buttons count=\"" << cppn->getNrOfColorButtons() << "\">\n";
	for(size_t i=0; i<cppn->getNrOfColorButtons(); i++){
		writeColorButton(cppn->getColorButton(i));
	}
	output << indent(2) << "</buttons>\n";

	output << indent(2) << "<nodes count=\"" << cppn->getNrOfNodes() << "\">\n";
	for(size_t i=0; i<cppn->getNrOfNodes(); i++){
		writeNode(cppn->getNode(i));
	}

	output << indent(2) << "</nodes>\n";

	output << indent(2) << "<links count=\"" << cppn->getNrOfEdges() << "\">\n";
	for(size_t i=0; i<cppn->getNrOfEdges(); i++){
		writeEdge(cppn->getEdge(i));
	}

	output << indent(2) << "</links>\n";

	for(size_t i=1; i<cppn->getNrOfFooterLines(); i++){
		output << cppn->getFooterLine(i) << "\n";
	}
}

void CppnWriter::writeNode(Node* node){
	if(node->getType() == XML_TYPE_HIDDEN){
		output << indent(3) <<  "<node type=\"" << node->getType() << "\">\n";
	} else {
		output << indent(3) << "<node label=\"" << node->getLabel() << "\" type=\"" << node->getType() << "\">\n";
	}


	output << indent(4) << "<marking branch=\"" << node->getBranch() << "\" id=\""<< node->getId() <<"\"/>\n";
	output << indent(4) << "<activation>" << node->getXmlActivationFunction() << "</activation>\n";
	output << indent(4) << "<color>" << node->getColor().red() << " " << node->getColor().green() << " " << node->getColor().blue() << "</color>\n";
	output << indent(4) << "<position>" << std::setprecision(17) << node->pos().x() << " " << node->pos().y() << "</position>\n";
	output << indent(3) << "</node>\n";

}

void CppnWriter::writeEdge(Edge* edge){
	output << indent(3) << "<link>\n";
	output << indent(4) << "<marking branch=\"" << edge->getBranch() << "\" id=\"" << edge->getId() << "\"/>\n";
	output << indent(4) << "<source branch=\"" << edge->sourceNode()->getBranch() << "\" id=\"" << edge->sourceNode()->getId() << "\"/>\n";
	output << indent(4) << "<target branch=\"" << edge->destNode()->getBranch() << "\" id=\"" << edge->destNode()->getId() << "\"/>\n";
	output << indent(4) << "<weight>" << std::setprecision(17) << edge->getWeight() << "</weight>\n";
	output << indent(4) << "<color>" << edge->getColor().red() << " " << edge->getColor().green() << " " << edge->getColor().blue() << "</color>\n";
	output << indent(3) << "</link>\n";
}

void CppnWriter::writeColorButton(CE_ColorButton* colorButton){
	output << indent(3) << "<colorButton>\n";
	output << indent(4) << "<text>" << colorButton->getText() << "</text>\n";
	output << indent(4) << "<color>" << colorButton->getColor().red() << " " << colorButton->getColor().green() << " " << colorButton->getColor().blue() << "</color>\n";
	output << indent(3) << "</colorButton>\n";
}
