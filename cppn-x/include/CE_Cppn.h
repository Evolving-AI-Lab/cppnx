/*
 * CE_Cppn.h
 *
 *  Created on: May 30, 2013
 *      Author: joost
 */

#ifndef CE_CPPN_H_
#define CE_CPPN_H_
#include "CE_Defines.h"
#include "CE_Edge.h"
#include "CE_Node.h"
#include "CE_CppnWidget.h"
#include "CE_ActivationFunctions.h"
#include "CE_ColorButton.h"
//#include "NEAT_Defines.h"
//#include "NEAT_STL.h"

class GraphWidget;
class Node;
class Edge;


typedef double (*ActivationFunctionPt)(double);

class Cppn {
public:

	Cppn(GraphWidget* widget):
		activationFunctions(0),
		linkWeights(0),
		lastTargets(0),
		lastSources(0),
		nodeSources(0),
		phenotypeNodes(0),
		numberOfNodes(0),
		numberOfEdges(0),
		nodeChache(0),
		linkChache(0),
		toUpdate(0),
		toUpdateStart(0),
		widget(widget),
		nodes(nr_of_inputs),
		edges(0),
		nodeMap(),
		validPhenotype(false),
		newFile(false)
	{

	}

	~Cppn(){
		deletePhenotype();
	}

	void addNode(std::string branch, std::string id, std::string type, std::string activationFunction, std::string label, QPointF position, QColor color);
	void addConnection(std::string branch, std::string id, std::string source_branch, std::string source_id, std::string target_branch, std::string target_id, double weight, QColor color_q);
	void setWeight(Edge* edge, double weight);
	void updateNodes();

	void buildPhenotype();
	inline void updateNode(const size_t& node, const size_t& xy_index, const double& initialValue = 0);
//	inline void updateLink(const size_t& node, const size_t& link, const size_t& xy_index);

	void positionNodes();
	void positionNodesCircle();

	size_t getNrOfNodes(){ return numberOfNodes;};
	size_t getNrOfEdges(){ return numberOfEdges;};

	Node* getNode(size_t i){return nodes[i];};
	Edge* getEdge(size_t i){return edges[i];};

	std::string getHeaderLine(size_t i){
		return headerLines[i];
	}
	void addHeaderLine(std::string line){
		headerLines.push_back(line);
	}
	size_t getNrOfHeaderLines(){
		return headerLines.size();
	}

	std::string getFooterLine(size_t i){
		return footerLines[i];
	}
	void addFooterLine(std::string line){
		footerLines.push_back(line);
	}
	size_t getNrOfFooterLines(){
		return footerLines.size();
	}

	void setNewFile(bool _newFile){
		newFile = _newFile;
	}

	bool getNewFile(){
		return newFile;
	}

	size_t getNrOfColorButtons();
	CE_ColorButton* getColorButton(size_t i);
	void addColorButton(std::string text, QColor color);


private:
	void updateFromLink(Edge* edge);
	void placeNode(Node* node, size_t index, size_t& lastTarget, size_t& lastSource);
	std::vector< std::vector <Node*> > buildLayers();

	void deletePhenotype(){
		if(activationFunctions) delete[] activationFunctions;
		if(linkWeights) delete[] linkWeights;
		if(lastTargets) delete[] lastTargets;
		if(lastSources) delete[] lastSources;
		if(nodeSources) delete[] nodeSources;
		if(phenotypeNodes) delete[] phenotypeNodes;
		if(nodeChache) delete[] nodeChache;
		if(linkChache) delete[] linkChache;
		if(toUpdate) delete[] toUpdate;
		if(toUpdateStart) delete[] toUpdateStart;
	}

	//Phenotype (maybe make a separate class for it?)
	ActivationFunctionPt* activationFunctions;
	double* linkWeights;
	size_t* lastTargets;
	size_t* lastSources;
	size_t* nodeSources;
	Node** phenotypeNodes;
	double* nodeChache;
	double* linkChache;
	size_t* toUpdate;
	size_t* toUpdateStart;

	size_t numberOfNodes;
	size_t numberOfEdges;
	//End phenotype


	GraphWidget* widget;
    std::vector<Node*> nodes;
    std::vector<Edge*> edges;
    std::map<std::string, Node*> nodeMap;

    bool validPhenotype;

//    static const int width = 100;
//    static const int height = 100;
    static const int width = 256;
    static const int height = 256;

    std::vector<std::string> headerLines;
    std::vector<std::string> footerLines;

    bool newFile;

//    QList<CE_ColorButton*> buttons;
};


#endif /* CE_CPPN_H_ */
