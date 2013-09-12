/*
 * CE_Cppn.cpp
 *
 *  Created on: May 30, 2013
 *      Author: joost
 */

#include <queue>
#include <set>
#include <cmath>
#include <algorithm>

#include "CE_Cppn.h"
#include "opt_placement.hpp"


static const double Pi = 3.14159265358979323846264338327950288419717;

void Cppn::addNode(Node* node){
	removedNodes.removeAll(node);

	if(node->getXmlLabel() == INPUT_X){
		nodes[input_x]=node;
		node->setIndex(input_x);
	} else if(node->getXmlLabel() == INPUT_Y){
		nodes[input_y]=node;
		node->setIndex(input_y);
	} else if(node->getXmlLabel() == INPUT_D){
		nodes[input_d]=node;
		node->setIndex(input_d);
	} else if(node->getXmlLabel() == INPUT_BIAS){
		nodes[input_b]=node;
		node->setIndex(input_b);
	} else {
		nodes.push_back(node);
	}

	if(node->getType() == XML_TYPE_INPUT){
		inputs.push_back(node);
	} else if(node->getType() == XML_TYPE_OUTPUT){
		outputs.push_back(node);
	}

	nodeMap[node->getBranch() + "_" + node->getId()]=node;
	numberOfNodes++;
	validPhenotype=false;
}

void Cppn::removeNode(Node* node){
	nodes.removeAll(node);
	removedNodes.append(node);
	numberOfNodes--;
	validPhenotype=false;
}

void Cppn::addConnection(Edge* edge){
//	std::cout << "Adding connection: " << edge << " from: " << edge->sourceNode() << " to " << edge->destNode() << std::endl;
	edge->sourceNode()->addOutgoingEdge(edge);
	edge->destNode()->addIncommingEdge(edge);
	removedEdges.removeAll(edge);
	edges.push_back(edge);
	validPhenotype=false;
	numberOfEdges++;
//	edge->setCppn(this);
}



void Cppn::removeConnection(Edge* edge){
	edge->sourceNode()->removeOutgoingEdge(edge);
	edge->destNode()->removeIncommingEdge(edge);
	edges.removeAll(edge);
	removedEdges.push_back(edge);
	validPhenotype=false;
	numberOfEdges--;
//	buildPhenotype();
}

void Cppn::setWeight(Edge* edge, double weight, bool update){
//	std::cout << "Set weight" <<std::endl;
	if(!validPhenotype) buildPhenotype();
	linkWeights[edge->getIndex()]=weight;
	if(update) updateFromLink(edge);
}



void Cppn::updateNodes(){
	if(!validPhenotype) buildPhenotype();

//	std::cout << "Setting input values... " << std::flush;
	//Set input values
	for(int x=0; x<width; x++){
		for(int y=0; y<height; y++){
			double xv = (double(x)/(double(width)/2) - 1);
			double yv = (double(y)/(double(height)/2) - 1);
			size_t index = x + y*width;

			updateNode(input_x, index, xv);
			updateNode(input_y, index, yv);
			updateNode(input_d, index, double(std::sqrt(float(xv*xv+yv*yv))*1.4));
			updateNode(input_b, index, 1.0);
		}
	}
//	std::cout << "done" << std::endl;

	//Update the rest
//	std::cout << "Updating other nodes... " << std::flush;
	for(size_t currentNode=nr_of_inputs; currentNode<numberOfNodes; currentNode++){
//		std::cout << "Current node: " << currentNode << std::endl;
		for(size_t xy_index=0; xy_index < width*height;  xy_index++){
//			std::cout << "Index: " << xy_index << std::endl;
			updateNode(currentNode, xy_index);
		}
		phenotypeNodes[currentNode]->updateAll();
	}
//	std::cout << "done" << std::endl;

}

std::vector< std::vector <Node*> > Cppn::buildLayers(){
	std::vector< std::vector <Node*> > layers;
	layers.push_back(std::vector <Node*>());
	QList <Node*> notPlaced = nodes;
	QList <Node*> nextNotPlaced;
	std::map <Node*, int> incommingEdges;

	for(int i=0; i<nodes.size(); i++){
		incommingEdges[nodes[i]]= nodes[i]->incomingEdges().size();
		//std::cout << nodes[i]->incomingEdges().size() << std::endl;
	}

	int depth = 0;
	while(notPlaced.size()>0){
		nextNotPlaced.clear();
		for(int i=0; i<notPlaced.size(); i++){
			if(incommingEdges[notPlaced[i]] <= 0){
				layers.back().push_back(notPlaced[i]);
				notPlaced[i]->setDepth(depth);
			} else {
				nextNotPlaced.push_back(notPlaced[i]);
			}
		}

		for(size_t i=0; i<layers.back().size(); i++){
			QList<Edge *> outgoingEdges = layers.back()[i]->outgoingEdges();
			foreach(Edge* outgoing_edge, outgoingEdges){
				incommingEdges.find(outgoing_edge->destNode())->second = incommingEdges[outgoing_edge->destNode()]-1;
			}
		}

		layers.push_back(std::vector <Node*>());
		depth++;
		notPlaced = nextNotPlaced;
	}

	return layers;
}


void Cppn::placeNode(Node* node, size_t index, size_t& lastTarget, size_t& lastSource){
//	std::cout << "Placing node: " << node << " index:" << index << " ... " << std::flush;
	activationFunctions[index]=node->getActivationFunction();

	node->setIndex(index);
	phenotypeNodes[index]=node;

	QList<Edge *> outgoingEdges = node->outgoingEdges();
	foreach(Edge* outgoing_edge, outgoingEdges){
		linkWeights[lastTarget] = outgoing_edge->getWeight();
//		std::cout << "Outgoing edge: " << outgoing_edge << " index:" << lastTarget << " ... " << std::endl;
		outgoing_edge->setIndex(lastTarget);
		lastTarget++;
	}

	QList<Edge *> incommingEdges = node->incomingEdges();
	foreach(Edge* incommingEdge, incommingEdges){
//		std::cout << "Incomming edge: " << incommingEdge << " index:" << lastSource << " ... " << std::endl;
		nodeSources[lastSource] = incommingEdge->getIndex()*width*height;
		lastSource++;
	}

	lastTargets[index+1]=lastTarget;
	lastSources[index+1]=lastSource;
//	std::cout << "done" << std::endl;
}

void Cppn::buildPhenotype(){
//	std::cout << "Building phenotype" <<std::endl;
	deletePhenotype();
//	std::cout << "New nodes size: " << nodes.size() <<std::endl;
//	std::cout << "New edges size: " << edges.size() <<std::endl;
	activationFunctions = new ActivationFunctionPt[nodes.size()];
	lastTargets = new size_t[nodes.size()+1];
	lastSources = new size_t[nodes.size()+1];
	toUpdateStart = new size_t[nodes.size()+1];
	phenotypeNodes = new Node*[nodes.size()];


	linkWeights = new double[edges.size()];
	nodeSources = new size_t[edges.size()];

	nodeChache = new double[nodes.size()*width*height];
	linkChache = new double[edges.size()*width*height];


	std::vector< std::vector <Node*> > layers = buildLayers();

	std::queue<Node*> test1;

	size_t currentNodeIndex=nr_of_inputs;
	lastTargets[0]=0;
	lastSources[0]=0;
	size_t targetEdgeIndex=0;
	size_t sourceEdgeIndex=0;
	//size_t lastTarget=0;
	//size_t lastSource=0;

//	std::cout << "Ordering input nodes... " << std::flush;
	//Put the input nodes in the right order
	Node* inputs[4];
	for(size_t i=0; i<layers[0].size(); i++ ){
		if(layers[0][i]->getType() == XML_TYPE_INPUT){
			inputs[layers[0][i]->getIndex()] = layers[0][i];
		}
	}
//	std::cout << "done " << std::endl;

	std::queue<Node*> test2;

//	std::cout << "Building input nodes... " << std::flush;
	//Process the input nodes
	for(size_t i=0; i<4; i++ ){
		placeNode(inputs[i], i, targetEdgeIndex, sourceEdgeIndex);
	}
//	std::cout << "done " << std::endl;

	std::queue<Node*> test3;

//	std::cout << "Building other nodes... " << std::endl;
	//Process the rest
	for(size_t i=0; i< layers.size(); i++){
		for(size_t j=0; j< layers[i].size(); j++){


			if(layers[i][j]->getType() == XML_TYPE_INPUT){
				//std::cout << "Input set" << std::endl;
				//placeNode(layers[i][j], layers[i][j]->getIndex(), targetEdgeIndex, sourceEdgeIndex);
			} else {
				placeNode(layers[i][j], currentNodeIndex, targetEdgeIndex, sourceEdgeIndex);
				currentNodeIndex++;
			}
		}
	}
//	std::cout << "done " << std::endl;

//	std::cout << "Initializing queue " << std::endl;
	std::queue<Node*> toProcess;
//	std::cout << "Initialized queue " << std::endl;


	std::set<std::pair<size_t, size_t> > ancestor;

//	std::cout << "Building ancestors... " << std::endl;
	for(int i=0; i<nodes.size(); i++){
		toUpdateStart[i]=ancestor.size();
		toProcess.push(phenotypeNodes[i]);
		size_t fromNodeIndex = phenotypeNodes[i]->getIndex();
		ancestor.insert(std::pair<size_t, size_t>(fromNodeIndex, fromNodeIndex));

		while(!toProcess.empty()){
			QList<Edge *> outgoingEdges = toProcess.front()->outgoingEdges();
//			std::cout << "Node: " << toProcess.front() << std::endl;
//			std::cout << "Outgoing edges size: " << toProcess.front()->outgoingEdges().size() << std::endl;
			toProcess.pop();
			foreach(Edge* outgoing_edge, outgoingEdges){
				size_t toNodeIndex = outgoing_edge->destNode()->getIndex();
				if(ancestor.count(std::pair<size_t, size_t>(fromNodeIndex, toNodeIndex))==0){
					ancestor.insert(std::pair<size_t, size_t>(fromNodeIndex, toNodeIndex));
					toProcess.push(outgoing_edge->destNode());
				}
			}
		}
	}
//	std::cout << "done " << std::endl;

//	std::cout << "ancestor.size() " << ancestor.size() << std::endl;
	toUpdateStart[nodes.size()]=ancestor.size();
	toUpdate=new size_t[ancestor.size()];

	size_t index=0;

//	std::cout << "Filling update array... " << std::flush;
	for(int fromNodeIndex=0; fromNodeIndex<nodes.size(); fromNodeIndex++){
		for(int toNodeIndex=0; toNodeIndex<nodes.size(); toNodeIndex++){
			if(ancestor.count(std::pair<size_t, size_t>(fromNodeIndex, toNodeIndex))==1){
				toUpdate[index]=toNodeIndex;
				index++;
			}
		}
	}
//	std::cout << "done " << std::endl;

	calculateModularity();

	validPhenotype=true;
}


qreal getLength(std::vector<Node*> layer, std::vector<QPointF> positions){
	qreal result = 0;
	for(size_t i=0; i< layer.size(); i++){
		foreach(Edge* edge, layer[i]->incomingEdges()){
			result += QLineF(edge->sourceNode()->pos(), positions[i]).length();
		}
	}

	return result;
}

bool compareIds(std::vector<Node*> layer1, std::vector<Node*> layer2){
	for(size_t i=0; i< layer1.size(); i++){
		if(layer1[i]->getId() < layer2[i]->getId()) return true;
		if(layer1[i]->getId() > layer2[i]->getId()) return false;
	}

	return false;
}

void Cppn::positionNodesCircle(){
	std::vector< std::vector <Node*> > layers = buildLayers();


	int k =1;
	QPointF position1;
	QPointF position2;
	Node* edge1 = NULL;
	qreal distanceEdge1 = 0;
	qreal distanceEdge2 = 0;

	for(size_t i=0; i< layers.size(); i++){
		for(size_t j=0; j< layers[i].size(); j++){
			if(k%2 != 0){
				position1 = QPointF(300 * std::cos(float(((Pi/(nodes.size()))*k)+ Pi/2)), 300 * std::sin(float(((Pi/(nodes.size()))*k)+ Pi/2)) );
				position2 = QPointF(-300 * std::cos(float(((Pi/(nodes.size()))*k)+ Pi/2)), 300 * std::sin(float(((Pi/(nodes.size()))*k)+ Pi/2)) );
			}
			qreal distance1 = 0;
			qreal distance2 = 0;
			foreach(Edge* edge, layers[i][j]->incomingEdges()){
				distance1 += QLineF(edge->sourceNode()->pos(), position1).length();
				distance2 += QLineF(edge->sourceNode()->pos(), position2).length();
			}


			if(k%2 == 0){
				if(distanceEdge1 + distance2 < distanceEdge2 +distance1){
					edge1->setPos(position1);
					layers[i][j]->setPos(position2);
				} else {
					edge1->setPos(position2);
					layers[i][j]->setPos(position1);
				}
				edge1 = NULL;
			}else {
				edge1 = layers[i][j];
				distanceEdge1 = distance1;
				distanceEdge2 = distance2;
			}
			k++;
		}
	}

	if(edge1 != NULL){
		if(distanceEdge1 < distanceEdge2){
			edge1->setPos(position1);
		} else {
			edge1->setPos(position2);
		}
	}
}

void Cppn::positionNodes(){
	std::vector< std::vector <Node*> > layers = buildLayers();

	QPointF position;

	int xscale = 200;
	int yscale = 100;

	for(size_t i=0; i< layers.size(); i++){
		size_t layerSize = layers[i].size();
		std::vector<QPointF> positions(layerSize);

		for(size_t j=0; j< layerSize; j++){
			positions[j] = QPointF(int(j)*xscale-(int(layerSize)-1)*(xscale/2), -(int(i)*yscale-(int(layers.size())-1)*(yscale/2)));
		}

		std::vector<Node*> shortestLayout = layers[i];

		for(size_t j=0; j< layerSize; j++){
			shortestLayout[j]->setPos(positions[j]);
		}
	}

}

void Cppn::updateFromLink(Edge* edge){
	if(!validPhenotype) updateNodes();
	size_t source_node_index = edge->sourceNode()->getIndex();
	size_t target_node_index = edge->destNode()->getIndex();
	size_t link_index = edge->getIndex();

	for(size_t xy_index=0; xy_index < width*height;  xy_index++){
		linkChache[xy_index+link_index*width*height]=nodeChache[xy_index+source_node_index*width*height]*linkWeights[link_index];
	}

	for(size_t i=toUpdateStart[target_node_index]; i<toUpdateStart[target_node_index+1]; i++){
		for(size_t xy_index=0; xy_index < width*height;  xy_index++){
			updateNode(toUpdate[i], xy_index);
		}

		phenotypeNodes[toUpdate[i]]->updateAll();
	}

}


inline void Cppn::updateNode(const size_t& node, const size_t& xy_index, const double& initialValue){
	size_t index = xy_index+node*width*height;

	nodeChache[index]=initialValue;

	//Calculate the sum of incoming links

	for(size_t i = lastSources[node]; i<lastSources[node+1]; i++){
		//std::cout << "lastSources[node] " << lastSources[node] << "lastSources[node+1] " << lastSources[node+1] <<std::endl;
		nodeChache[index]+=linkChache[xy_index+nodeSources[i]];
	}

	//Apply the activation function
	nodeChache[index]= (*activationFunctions[node])(nodeChache[index]);
	phenotypeNodes[node]->setPixel(xy_index, nodeChache[index]);

	//Calculate outgoing links
	//std::cout << "lastTargets[node] " << lastTargets[node] << " lastTargets[node+1] " << lastTargets[node+1] <<std::endl;
	for(size_t i = lastTargets[node]; i<lastTargets[node+1]; i++){
		linkChache[xy_index+i*width*height]=nodeChache[index]*linkWeights[i];
	}
}

inline void Cppn::updateNode(const size_t& node){
	if(node <nr_of_inputs){
		for(size_t xy_index=0; xy_index < width*height;  xy_index++){
			updateNode(node, xy_index, nodeChache[xy_index+node*width*height]);
		}
	} else {
		for(size_t xy_index=0; xy_index < width*height;  xy_index++){
			updateNode(node, xy_index);
		}
	}
	phenotypeNodes[node]->updateAll();
}

void Cppn::updateNode(Node* node){
	updateNode(node->getIndex());
}

void Cppn::positionNodesONP(){
//    std::vector<std::vector<Node*> > mods;
//    double qscore =  mod::modules(this, mods);
//
//	std::cout << "Modularity: " << qscore << std::endl;

	QList<Node*> inputs = getInputs();
	QList<Node*> outputs = getOutputs();
	QList<Node*> nodes = getNodes();

	QVector<double> input_x_coords(inputs.size());
	QVector<double> output_x_coords(outputs.size());
	QVector<double> input_y_coords(inputs.size());
	QVector<double> output_y_coords(outputs.size());

	for(int i=0; i<inputs.size(); i++){
		input_x_coords[i] = inputs[i]->pos().x();
		input_y_coords[i] = inputs[i]->pos().y();
	}

	for(int i=0; i<outputs.size(); i++){
		output_x_coords[i] = outputs[i]->pos().x();
		output_y_coords[i] = outputs[i]->pos().y();
	}

	QVector<double> x_coords = opt_placement::compute(this, inputs, input_x_coords, outputs, output_x_coords);
	QVector<double> y_coords = opt_placement::compute(this, inputs, input_y_coords, outputs, output_y_coords);

	for(int i=0; i<nodes.size(); i++){
		nodes[i]->setPos(x_coords[i], y_coords[i]);
	}
}

double Cppn::calculateModularity(){
    std::vector<std::vector<Node*> > mods;
    double qscore =  mod::modules(this, mods);

	for(size_t i=0; i <mods.size(); i++){
		for(size_t j=0; j <mods[i].size(); j++){
			mods[i][j]->setModule(i);
		}
	}

	emit newModularity(qscore);

	return qscore;
}

