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
//#include "CE_CppnWidget.h"
#include "CE_ActivationFunctions.h"
#include "CE_Label.h"
#include "CE_FinalNodeView.h"
//#include "NEAT_Defines.h"
//#include "NEAT_STL.h"

//class CppnWidget;
class Node;
class Edge;




class Cppn : public QObject{
	Q_OBJECT
public:

	Cppn():
		activationFunctions(0),
		linkWeights(0),
		lastTargets(0),
		lastSources(0),
		nodeSources(0),
		phenotypeNodes(0),
		nodeChache(0),
		linkChache(0),
		toUpdate(0),
		toUpdateStart(0),
		numberOfNodes(0),
		numberOfEdges(0),
//		widget(widget),
//		nodes(nr_of_inputs),
//		edges(),
		nodeMap(),
		validPhenotype(false)
//		newFile(false)

	{
		for(int i =0; i<nr_of_inputs; i++) nodes.append(0);
//		finalNodeView = new FinalNodeView(width, height);
//		finalNodeView = new FinalNodeView();
	}

	~Cppn(){
		deletePhenotype();
		foreach(Node* node, removedNodes){
			delete node;
		}

		foreach(Edge* edge, removedEdges){
			delete edge;
		}
	}


	void addNode(Node* node);
	void addConnection(Edge* edge);

	void removeNode(Node* node);
	void removeConnection(Edge* edge);


	void buildPhenotype();
	inline void updateNode(const size_t& node, const size_t& xy_index, const double& initialValue = 0);
	inline void updateNode(const size_t& node);
//	inline void updateLink(const size_t& node, const size_t& link, const size_t& xy_index);

	void positionNodes();
	void positionNodesCircle();
	void positionNodesONP();

	double calculateModularity();

	size_t getNrOfNodes() const{ return numberOfNodes;};
	size_t getNrOfEdges() const{ return numberOfEdges;};

	Node* getNode(size_t i){return nodes[i];};
	Node* getNode(std::string name){return nodeMap[name];};
	Edge* getEdge(size_t i){return edges[i];};

	QList<Node*> getNodes() const{return nodes;};
	QList<Edge*> getEdges() const{return edges;};

	QList<Node*> getInputs() const{return inputs;};
	QList<Node*> getOutputs() const{return outputs;};

    static const int width = 256;
    static const int height = 256;

public slots:
	void updateNode(Node* node);
	void setWeight(Edge* edge, double weight, bool update = true);
	void updateNodes();

signals:
	void newModularity(double);

private:
	void updateFromLink(Edge* edge);
	void placeNode(Node* node, size_t index, size_t& lastTarget, size_t& lastSource);
	std::vector< std::vector <Node*> > buildLayers();

	void deletePhenotype(){
//		std::cout << "Deleting phenotype..." << std::flush;
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
//		std::cout << "done" << std::endl;
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


//	CppnWidget* widget;
    QList<Node*> nodes;
    QList<Node*> inputs;
    QList<Node*> outputs;
    QList<Edge*> edges;
    QList<Node*> removedNodes;
    QList<Edge*> removedEdges;
    std::map<std::string, Node*> nodeMap;

    bool validPhenotype;

//    static const int width = 100;
//    static const int height = 100;


//    std::vector<std::string> headerLines;
//    std::vector<std::string> footerLines;

//    bool newFile;
//    FinalNodeView* finalNodeView;

    //File only data
//    std::string age;
//    std::string phenotype;
//    std::string branch;
//    std::string id;
//    std::string dataVersion;
//    std::vector<std::string> parent_branches;
//    std::vector<std::string> parent_ids;


//    QList<CE_ColorButton*> buttons;
};


#endif /* CE_CPPN_H_ */
