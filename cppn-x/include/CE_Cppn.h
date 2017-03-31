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
#include "CX_Debug.hpp"
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
		nodeMap(),
		validPhenotype(false),
		_numberOfModules(0),
		_maxId(0),
	    _inputsOrdered(false),
		_min_x(-1),
		_max_x(1),
		_min_y(-1),
		_max_y(1),
		width(IMAGE_WIDTH),
		height(IMAGE_HEIGHT)
	{
	    //Reserve space for the inputs
	    //Exactly four inputs have to be provided through the addNode function
	    //That have the XML labels:
	    // INPUT_X
	    // INPUT_Y
	    // INPUT_D
	    // INPUT_BIAS
	    //Before the network can be run
	    //TODO: We should probably look for a better solution
//		for(int i =0; i<nr_of_inputs; i++) nodes.append(0);
	}

	~Cppn(){
		deletePhenotype();
		dbg::out(dbg::info, "cppn") << "Deleting: " << removedNodes.size() + nodes.size() << " nodes" << std::endl;
		foreach(Node* node, removedNodes){
			delete node;
		}
        foreach(Node* node, nodes){
            delete node;
        }

		dbg::out(dbg::info, "cppn") << "Deleting: " << removedEdges.size() + edges.size() << " edges" << std::endl;
		foreach(Edge* edge, removedEdges){
			delete edge;
		}
        foreach(Edge* edge, edges){
            delete edge;
        }
	}

	void copy(Cppn* other);

	void addNode(Node* node);
	void addConnection(Edge* edge);

	void removeNode(Node* node);
	void removeConnection(Edge* edge);

	void setMinX(int min_x){_min_x = min_x;}
	int getMinX(){ return _min_x;}
	void setMaxX(int max_x){_max_x = max_x;}
	int getMaxX(){ return _max_x;}
	void setMinY(int min_y){_min_y = min_y;}
	int getMinY(){ return _min_y;}
	void setMaxY(int max_y){_max_y = max_y;}
	int getMaxY(){ return _max_y;}
	void setResX(int res_x){width = res_x;}
	int getResX(){ return width;}
	void setResY(int res_y){height = res_y;}
	int getResY(){ return height;}

	void rewireConnection(Edge* edge, Node* newSource, Node* newTarget);
	bool connected(Node* source, Node* target);
	bool connected(size_t sourceIndex, size_t targetIndex);
	bool connectionCausesCycle(Edge* edge);
	bool connectionIsOkay(Edge* edge);

	void setActivationFunction(Node* node, const std::string& activationFunction);

	/**
	 * Returns a set of all predecessors of the supplied node, including itself.
	 *
	 * @param node The node from which to get the predecessors.
	 *             It is assumed that the node is part of this cppn,
	 *             but it probably doesn't have to be.
	 * @return     A set of all nodes that are predecessors of the supplied node.
	 */
	QSet<Node*> getPredecessors(Node* node);

	void buildPhenotype();
	inline void updateNode(const size_t& node, const size_t& xy_index, const double& initialValue = 0);
	inline void updateNode(const size_t& node);
//	inline void updateLink(const size_t& node, const size_t& link, const size_t& xy_index);

	void positionNodes();
	void positionNodesCircle();
	void positionNodesONP();

	double calculateModularity();
	void colorPaths(int threshold, float thresholdf);

	size_t getNrOfNodes() const{ return numberOfNodes;};
	size_t getNrOfEdges() const{ return numberOfEdges;};

	Node* getNode(size_t i){dbg::check_bounds(dbg::error, 0, i, nodes.size(), DBG_HERE); return nodes[i];};
	Node* getNode(std::string name){return nodeMap[name];};
	Edge* getEdge(size_t i){dbg::check_bounds(dbg::error, 0, i, edges.size(), DBG_HERE); return edges[i];};
	Edge* getEdge(Node* source, Node* target);
	Edge* getEdge(size_t sourceIndex, size_t targetIndex);

	QList<Node*> getNodes() const{return nodes;};
	QList<Edge*> getEdges() const{return edges;};

	QList<Node*> getInputs() const{return inputs;};
	QList<Node*> getOutputs() const{return outputs;};

	size_t getNrOfModules() const{return _numberOfModules;};

	id_t getMaxId() const{return _maxId;}
	std::string getNextId() const{return util::toString(getMaxId()+1);}

	// Picbreeder settings
    //static const int width = IMAGE_WIDTH;
    //static const int height = IMAGE_HEIGHT;

    // Anhs Settings




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
	    dbg::out(dbg::info, "cppn") << "Deleting phenotype..." << std::endl;
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
		dbg::out(dbg::info, "cppn") << "Deleting phenotype... done" << std::endl;
	}

	void swap(int index1, int index2);

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
    QMap<QPair<Node*, Node*>, Edge*> _connectivityMap;
    bool validPhenotype;

    size_t _numberOfModules;

    id_t _maxId;
    bool _inputsOrdered;

    int _min_x;
    int _max_x;
    int _min_y;
    int _max_y;
    int width;
    int height;
};


#endif /* CE_CPPN_H_ */
