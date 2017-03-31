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
#include "CX_Debug.hpp"
#include "opt_placement.hpp"



static const double Pi = 3.14159265358979323846264338327950288419717;

typedef QList<Edge*> path_t;
typedef QSet<path_t> stream_t;

void Cppn::copy(Cppn* other){
	dbg::trace trace("cppn", DBG_HERE);
    dbg::out(dbg::info, "cppn") << "Copying " << other->getNrOfNodes() << " nodes." << std::endl;
    for(size_t i=0; i<other->getNrOfNodes(); ++i){
        Node* otherNode = other->getNode(i);
        Node* node = new Node(
                otherNode->getBranch(),
                otherNode->getId(),
                otherNode->getType(),
                otherNode->getXmlActivationFunction(),
                otherNode->getXmlLabel(),
                otherNode->getAffinity(),
                otherNode->getBias(),
                otherNode->getLabel());
        addNode(node);
    }
    dbg::out(dbg::info, "cppn") << "Added " << getNrOfNodes() << " nodes." << std::endl;

    for(size_t i=0; i<other->getNrOfNodes(); ++i){
        for(size_t j=0; j<other->getNrOfNodes(); ++j){
            dbg::out(dbg::info, "cppn") << "Testing nodes " << i << " and " << j << std::endl;
            if(other->connected(i, j)){
                dbg::out(dbg::info, "cppn") << "Connected" << std::endl;
                Edge* otherEdge = other->getEdge(i,j);
                Edge* thisEdge = new Edge(
                        otherEdge->getBranch(),
                        otherEdge->getId(),
                        getNode(i),
                        getNode(j),
                        otherEdge->getWeight(),
                        otherEdge->getOriginalWeight());
                dbg::out(dbg::info, "cppn") <<
                        "Adding connection: " << thisEdge->sourceNode() <<
                        " " << thisEdge->destNode() << std::endl;
                addConnection(thisEdge);
            }
        }
    }
}

void Cppn::swap(int index1, int index2){
	dbg::trace trace("cppn", DBG_HERE);
    nodes.swap(index1, index2);
    nodes[index1]->setIndex(index1);
    nodes[index2]->setIndex(index2);
}

void Cppn::addNode(Node* node){
	dbg::trace trace("cppn", DBG_HERE);
	removedNodes.removeAll(node);
	node->setIndex(nodes.size());
	nodes.push_back(node);
	node->setCppn(this);
	node->reinitImage();



	//See if node is an input or output
	if(node->getType() == XML_TYPE_INPUT){
		inputs.push_back(node);
	} else if(node->getType() == XML_TYPE_OUTPUT){
		outputs.push_back(node);
	}

	if(inputs.size() == 4 && !_inputsOrdered){
	    foreach(Node* inputNode, inputs){
	        //Determine the type of input, if this node is an input
	        if(inputNode->getXmlLabel() == INPUT_X){
	            swap(inputNode->getIndex(), input_x);
//	            nodes.swap(inputNode->getIndex(), input_x);
//	            nodes[]
//	            nodes[input_x]=node;
//	            node->setIndex(input_x);
	        } else if(inputNode->getXmlLabel() == INPUT_Y){
	            swap(inputNode->getIndex(), input_y);
//	            nodes[input_y]=node;
//	            node->setIndex(input_y);
	        } else if(inputNode->getXmlLabel() == INPUT_D){
	            swap(inputNode->getIndex(), input_d);
//	            nodes[input_d]=node;
//	            node->setIndex(input_d);
	        } else if(inputNode->getXmlLabel() == INPUT_BIAS){
	            swap(inputNode->getIndex(), input_b);
//	            nodes[input_b]=node;
//	            node->setIndex(input_b);
	        }
	    }
	    _inputsOrdered = true;
	}

	//Get id
	id_t id = util::numericCast<id_t>(node->getId());
	if(id > _maxId){
	    _maxId = id;
	}

	nodeMap[node->getBranch() + "_" + node->getId()]=node;
	numberOfNodes++;
	validPhenotype=false;
}

void Cppn::removeNode(Node* node){
	dbg::trace trace("cppn", DBG_HERE);
	nodes.removeAll(node);
	removedNodes.append(node);
	numberOfNodes--;
	validPhenotype=false;
}

void Cppn::addConnection(Edge* edge){
	dbg::trace trace("cppn", DBG_HERE);
//	std::cout << "Adding connection: " << edge << " from: " << edge->sourceNode() << " to " << edge->destNode() << std::endl;
	edge->sourceNode()->addOutgoingEdge(edge);
	edge->destNode()->addIncommingEdge(edge);
	_connectivityMap[QPair<Node*, Node*>(edge->sourceNode(), edge->destNode())] = edge;

    id_t id = util::numericCast<id_t>(edge->getId());
    if(id > _maxId){
        _maxId = id;
    }

	removedEdges.removeAll(edge);
	edges.push_back(edge);
	validPhenotype=false;
	numberOfEdges++;
//	edge->setCppn(this);
}



void Cppn::removeConnection(Edge* edge){
	dbg::trace trace("cppn", DBG_HERE);
	edge->sourceNode()->removeOutgoingEdge(edge);
	edge->destNode()->removeIncommingEdge(edge);
	_connectivityMap.erase(_connectivityMap.find(QPair<Node*, Node*>(edge->sourceNode(), edge->destNode())));
	edges.removeAll(edge);
	removedEdges.push_back(edge);
	validPhenotype=false;
	numberOfEdges--;

//	buildPhenotype();
}

void Cppn::setActivationFunction(Node* node, const std::string& activationFunction){
	dbg::trace trace("cppn", DBG_HERE);
    node->setActivationFunction(activationFunction);
    validPhenotype=false;
}

void Cppn::rewireConnection(Edge* edge, Node* newSource, Node* newTarget){
	dbg::trace trace("cppn", DBG_HERE);
    dbg::check_ptr(dbg::error, edge, DBG_HERE);
    dbg::check_ptr(dbg::error, newSource, DBG_HERE);
    dbg::check_ptr(dbg::error, newTarget, DBG_HERE);
    _connectivityMap.erase(_connectivityMap.find(QPair<Node*, Node*>(edge->sourceNode(), edge->destNode())));
    if(edge->sourceNode() != newSource){
        edge->sourceNode()->removeOutgoingEdge(edge);
        edge->setSourceNode(newSource);
        newSource->addOutgoingEdge(edge);
    }
    if(edge->destNode() != newTarget){
        edge->destNode()->removeIncommingEdge(edge);
        edge->setTargetNode(newTarget);
        newTarget->addIncommingEdge(edge);
    }
    _connectivityMap[QPair<Node*, Node*>(edge->sourceNode(), edge->destNode())] = edge;
    validPhenotype=false;
}

bool Cppn::connected(Node* source, Node* target){
	dbg::trace trace("cppn", DBG_HERE);
    dbg::check_ptr(dbg::error, source, DBG_HERE);
    dbg::check_ptr(dbg::error, target, DBG_HERE);
    return _connectivityMap.count(QPair<Node*, Node*>(source, target)) > 0;
}

bool Cppn::connected(size_t sourceIndex, size_t targetIndex){
	dbg::trace trace("cppn", DBG_HERE);
    dbg::out(dbg::info, "cppn") << "Nodes size: " << nodes.size() << std::endl;
    return connected(getNode(sourceIndex), getNode(targetIndex));
}

bool Cppn::connectionCausesCycle(Edge* thisEdge){
	dbg::trace trace("cppn", DBG_HERE);
	QList<Node*> frontier;
	QSet<Node*> visited;

	frontier.push_back(thisEdge->destNode());
	while(frontier.size() > 0){
		Node* current = frontier.back();
		frontier.pop_back();
		if(visited.contains(current)){
			continue;
		}
		visited.insert(current);
		if(current  == thisEdge->sourceNode()){
			return true;
		}
		foreach(Edge* edge, current->outgoingEdges()){
			if(thisEdge != edge){
				frontier.push_back(edge->destNode());
			}
		}
	}
	return false;
}

bool Cppn::connectionIsOkay(Edge* edge){
	dbg::trace trace("cppn", DBG_HERE);
    if(connectionCausesCycle(edge)) return false;
    if(edge->destNode()->getType() == XML_TYPE_INPUT) return false;
    if(connected(edge->sourceNode(), edge->destNode())) return false;
    return true;
}

QSet<Node*> Cppn::getPredecessors(Node* node){
    dbg::trace trace("cppn", DBG_HERE);
    QList<Node*> frontier;
    QSet<Node*> visited;

    frontier.push_back(node);
    while(frontier.size() > 0){
        Node* current = frontier.back();
        frontier.pop_back();
        if(visited.contains(current)){
            continue;
        }
        visited.insert(current);
        foreach(Edge* edge, current->outgoingEdges()){
            frontier.push_back(edge->sourceNode());
        }
    }
    return visited;
}

Edge* Cppn::getEdge(Node* source, Node* target){
	dbg::trace trace("cppn", DBG_HERE);
    return _connectivityMap[QPair<Node*, Node*>(source, target)];
}

Edge* Cppn::getEdge(size_t sourceIndex, size_t targetIndex){
	dbg::trace trace("cppn", DBG_HERE);
    return getEdge(getNode(sourceIndex), getNode(targetIndex));
}

//bool Cppn::valid(){
//    QList<Node*> frontier;
//    QSet<Node*> visited;
//    foreach(Node* input, inputs){
//        frontier.push_back(input);
//    }
//
//    while(frontier.size() > 0){
//        Node* current = frontier.back();
//        frontier.pop_back();
//        if(visited.contains(current)){
//            return false;
//        }
//        visited.insert(current);
//        foreach(Edge* edge, current->outgoingEdges()){
//            frontier.push_back(edge->destNode());
//        }
//    }
//    return true;
//}

void Cppn::setWeight(Edge* edge, double weight, bool update){
	dbg::trace trace("cppn", DBG_HERE);
//	std::cout << "Set weight" <<std::endl;
	if(!validPhenotype) buildPhenotype();
	linkWeights[edge->getIndex()]=weight;
	if(update) updateFromLink(edge);
}



void Cppn::updateNodes(){
	dbg::trace trace("cppn", DBG_HERE);
	if(!validPhenotype) buildPhenotype();

	dbg::out(dbg::info, "cppn") << "Setting input values... " << std::endl;
	//Set input values
	for(int x=0; x<width; x++){
		for(int y=0; y<height; y++){
			int x_diff = _max_x - _min_x;
			int y_diff = _max_y - _min_y;
			double xv = (double(x)/(double(width)/x_diff) + _min_x);
			double yv = (double(y)/(double(height)/y_diff) + _min_y);
			size_t index = x + y*width;

			updateNode(input_x, index, xv);
			updateNode(input_y, index, yv);
			updateNode(input_d, index, double(std::sqrt(float(xv*xv+yv*yv))*1.4));
			updateNode(input_b, index, 1.0);
		}
	}
	dbg::out(dbg::info, "cppn") << "Setting input values... done" << std::endl;

	//Update the rest
	dbg::out(dbg::info, "cppn") << "Updating other nodes... " << std::endl;
	for(size_t currentNode=nr_of_inputs; currentNode<numberOfNodes; currentNode++){
//		std::cout << "Current node: " << currentNode << std::endl;
		for(int xy_index=0; xy_index < width*height;  xy_index++){
//			std::cout << "Index: " << xy_index << std::endl;
			updateNode(currentNode, xy_index);
		}
		phenotypeNodes[currentNode]->updateAll();
	}
	dbg::out(dbg::info, "cppn") << "Updating other nodes... done" << std::endl;

}

std::vector< std::vector <Node*> > Cppn::buildLayers(){
	dbg::trace trace("cppn", DBG_HERE);
	std::vector< std::vector <Node*> > layers;
	layers.push_back(std::vector <Node*>());
	QList <Node*> notPlaced = nodes;
	QList <Node*> nextNotPlaced;
	std::map <Node*, int> incommingEdges;

	for(int i=0; i<nodes.size(); i++){
		incommingEdges[nodes[i]]= nodes[i]->incomingEdges().size();
		dbg::assertion(DBG_ASSERTION((nodes[i]->getType() != XML_TYPE_INPUT) || incommingEdges[nodes[i]] == 0));
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
	dbg::trace trace("cppn", DBG_HERE);
    dbg::out(dbg::info, "cppn") << "Placing node: " << node << " index: " << index << "... " << std::endl;
    dbg::check_ptr(dbg::error, node, DBG_HERE);
    dbg::check_bounds(dbg::error, 0, index, nodes.size(), DBG_HERE);
	activationFunctions[index]=node->getActivationFunction();

	node->setIndex(index);
	dbg::check_bounds(dbg::error, 0, index, nodes.size(), DBG_HERE);
	phenotypeNodes[index]=node;

	QList<Edge *> outgoingEdges = node->outgoingEdges();
	foreach(Edge* outgoing_edge, outgoingEdges){
	    dbg::check_bounds(dbg::error, 0, lastTarget, edges.size(), DBG_HERE);
		linkWeights[lastTarget] = outgoing_edge->getWeight();
		dbg::out(dbg::info, "cppn") << "Outgoing edge: " << outgoing_edge << " index:" << lastTarget << " ... " << std::endl;
		outgoing_edge->setIndex(lastTarget);
		lastTarget++;
	}

	QList<Edge *> incommingEdges = node->incomingEdges();
	foreach(Edge* incommingEdge, incommingEdges){
	    dbg::out(dbg::info, "cppn") << "Incomming edge: " << incommingEdge << " index:" << lastSource << " ... " << std::endl;
	    dbg::check_bounds(dbg::error, 0, lastSource, edges.size(), DBG_HERE);
		nodeSources[lastSource] = incommingEdge->getIndex()*width*height;
		lastSource++;
	}

//	std::cout << "size: " << dbg::array_size(lastTargets) << std::endl;
//	std::cout << "size: " << sizeof(*lastTargets) << std::endl;
//	std::cout << "size: " << nodes.size() + 1 << std::endl;
	dbg::check_bounds(dbg::error, 0, index+1, nodes.size() + 1, DBG_HERE);
	dbg::check_bounds(dbg::error, 0, index+1, nodes.size() + 1, DBG_HERE);
	lastTargets[index+1]=lastTarget;
	lastSources[index+1]=lastSource;
	dbg::out(dbg::info, "cppn") << "done" << std::endl;
}

void Cppn::buildPhenotype(){
	dbg::trace trace("cppn", DBG_HERE);
	deletePhenotype();
	dbg::out(dbg::info, "cppn") << "Building phenotype" << std::endl;
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
	size_t assigned_inputs = 0;
	std::vector<Node*> inputs(4, (Node*) 0);
	dbg::assertion(DBG_ASSERTION(layers[0].size() >= inputs.size()));
	dbg::out(dbg::info, "cppn") << "Nodes in layer 1: " << layers[0].size() << std::endl;
	for(size_t i=0; i<layers[0].size(); i++ ){
		if(layers[0][i]->getType() == XML_TYPE_INPUT){
		    size_t input_index = layers[0][i]->getIndex();
		    dbg::check_bounds(dbg::error, 0, input_index, inputs.size(), DBG_HERE);
		    dbg::assertion(DBG_ASSERTION(inputs[input_index] == 0));
		    dbg::out(dbg::info, "cppn") << "Found input: " << layers[0][i]->getXmlLabel() << std::endl;
			inputs[input_index] = layers[0][i];
			++assigned_inputs;
		}
	}
	dbg::assertion(DBG_ASSERTION(assigned_inputs == 4));
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

	dbg::out(dbg::info, "cppn") << "Calculating modularity" << std::endl;
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
	dbg::trace trace("cppn", DBG_HERE);
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
	dbg::trace trace("cppn", DBG_HERE);
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
	dbg::trace trace("cppn", DBG_HERE);
	if(!validPhenotype) updateNodes();
	size_t source_node_index = edge->sourceNode()->getIndex();
	size_t target_node_index = edge->destNode()->getIndex();
	size_t link_index = edge->getIndex();

	for(int xy_index=0; xy_index < width*height;  xy_index++){
		linkChache[xy_index+link_index*width*height]=nodeChache[xy_index+source_node_index*width*height]*linkWeights[link_index];
	}

	for(size_t i=toUpdateStart[target_node_index]; i<toUpdateStart[target_node_index+1]; i++){
		for(int xy_index=0; xy_index < width*height;  xy_index++){
			updateNode(toUpdate[i], xy_index);
		}

		phenotypeNodes[toUpdate[i]]->updateAll();
	}

}


inline void Cppn::updateNode(const size_t& node, const size_t& xy_index, const double& initialValue){
	dbg::trace trace("cppn", DBG_HERE);
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
	dbg::trace trace("cppn", DBG_HERE);
	if(node <nr_of_inputs){
		for(int xy_index=0; xy_index < width*height;  xy_index++){
			updateNode(node, xy_index, nodeChache[xy_index+node*width*height]);
		}
	} else {
		for(int xy_index=0; xy_index < width*height;  xy_index++){
			updateNode(node, xy_index);
		}
	}
	phenotypeNodes[node]->updateAll();
}

void Cppn::updateNode(Node* node){
	dbg::trace trace("cppn", DBG_HERE);
	updateNode(node->getIndex());
}

void Cppn::positionNodesONP(){
	dbg::trace trace("cppn", DBG_HERE);
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
	dbg::trace trace("cppn", DBG_HERE);
    std::vector<std::vector<Node*> > mods;
    double qscore =  mod::modules(this, mods);

	for(size_t i=0; i <mods.size(); i++){
		for(size_t j=0; j <mods[i].size(); j++){
			mods[i][j]->setModule(i);
		}
	}

	emit newModularity(qscore);
	_numberOfModules = mods.size();

	return qscore;
}


size_t countOverlap(QList<Edge*> path_1, QList<Edge*> path_2){
    size_t result = 0;
    foreach(Edge* edge_1, path_1){
        foreach(Edge* edge_2, path_2){
            if(edge_1 == edge_2){
                ++result;
                break;
            }
        }
    }
    return result;
}

QSet<path_t> splitPaths(QList<path_t> paths, size_t threshold){
    QSet<path_t> primary_stream_set;
    QList<path_t> primary_stream;
    size_t overlap = 0;

    primary_stream.push_back(paths.front());
    primary_stream_set.insert(paths.front());

    for(int i=0; i<primary_stream.size(); ++i){
        dbg::out(dbg::info, "colorpath") << "    Primary path: " << i << std::endl;
        path_t path1 = primary_stream[i];
        for(int j=0; j<paths.size(); ++j){
            dbg::out(dbg::info, "colorpath") << "        All paths: " << j << std::endl;
            if(primary_stream_set.contains(paths[j])) continue;
            overlap = countOverlap(path1, paths[j]);
            dbg::out(dbg::info, "colorpath") << "        Overlap between: "  << i << " and "<< j  << ":" << overlap << std::endl;
            if(overlap > threshold){
                dbg::out(dbg::info, "colorpath") << "        Path "<< j  << " added."<< std::endl;
                primary_stream.push_back(paths[j]);
                primary_stream_set.insert(paths[j]);
            }
        }
    }
    return primary_stream_set;
}

float countOverlapPercent(QList<Edge*> path_1, QList<Edge*> path_2){
    float result = 0;
    foreach(Edge* edge_1, path_1){
        foreach(Edge* edge_2, path_2){
            if(edge_1 == edge_2){
                result+=1.0;
                break;
            }
        }
    }
    return result / ((float(path_1.size()) + float(path_2.size()))/2.0f);
}

QSet<path_t> splitPathsPercent(QList<path_t> paths, float threshold){
    QSet<path_t> primary_stream_set;
    QList<path_t> primary_stream;
    float overlap = 0;

    primary_stream.push_back(paths.front());
    primary_stream_set.insert(paths.front());

    for(int i=0; i<primary_stream.size(); ++i){
        dbg::out(dbg::info, "colorpath") << "    Primary path: " << i << std::endl;
        path_t path1 = primary_stream[i];
        for(int j=0; j<paths.size(); ++j){
            dbg::out(dbg::info, "colorpath") << "        All paths: " << j << std::endl;
            if(primary_stream_set.contains(paths[j])) continue;
            overlap = countOverlapPercent(path1, paths[j]);
            dbg::out(dbg::info, "colorpath") << "        Overlap between: "  << i << " and "<< j  << ": " << overlap << std::endl;
            if(overlap > threshold){
                dbg::out(dbg::info, "colorpath") << "        Path "<< j  << " added."<< std::endl;
                primary_stream.push_back(paths[j]);
                primary_stream_set.insert(paths[j]);
            }
        }
    }
    return primary_stream_set;
}

//inline bool operator==(const path_t &e1, const path_t &e2)
//{
//    if(e1.size() != e2.size()) return false;
//    for(int i=0; i<e1.size(); ++i){
//        if(e1[i]->getName() != e2[i]->getName()) return false;
//    }
//    return true;
//}

inline uint qHash(const path_t &key)
{
    uint hash = 0;
    for(int i=0; i<key.size(); ++i){
        hash += 10^i + qHash(QString(key[i]->getName().c_str()));
    }
    return hash;
}


void Cppn::colorPaths(int threshold, float thresholdf){
	dbg::trace trace("cppn", DBG_HERE);
    std::cout << "Gathering paths" << std::endl;
    // Gather paths
    QStack<Edge*> stack;
    path_t path;
    QList<path_t> paths;
    foreach(Node* node, inputs){
        // Push all outgoing edges of our input node into the stack
        dbg::out(dbg::info, "colorpath") << "For input: " << node->getName() << std::endl;
        foreach(Edge* edge, node->outgoingEdges()){
            dbg::out(dbg::info, "colorpath") << "    Pushing edge into stack: " << edge->getName() << std::endl;
            stack.push(edge);
        }
        dbg::out(dbg::info, "colorpath") << "    Current stack: " << util::qPointerStackToString(stack) << std::endl;


        // Initialize an empty path
        path.clear();

        // Until the stack is empty, do:
        while(!stack.empty()){
            // Pop the edge from the stack
            Edge* currentEdge  = stack.pop();
            dbg::out(dbg::info, "colorpath") << "    Popped edge: " << currentEdge->getName() << std::endl;

            // Push the edge into the current path
            dbg::out(dbg::info, "colorpath") << "        Added edge to path: " << currentEdge->getName() << std::endl;
            path.push_back(currentEdge);
            dbg::out(dbg::info, "colorpath") << "        Current path: " << util::qPointerListToString(path) << std::endl;

            // If our target node, has no successor, we need to record the current path
            if(currentEdge->destNode()->outgoingEdges().empty()){
                dbg::out(dbg::info, "colorpath") << "        Edge: " << currentEdge->getName() << " is terminal. Adding path." << std::endl;
                paths.push_back(path);

                // If the stack is empty at this point, we have finished our exploration, so break
                if(stack.empty()){
                    dbg::out(dbg::info, "colorpath") << "        Stack is empty, moving on to next input."<< std::endl;
                    break;
                }

                // Backtrack until the last edge in the path is connected to the edge on top of the stack
                // That is, until the target of the last edge in the path is the same as the source of
                // the node on top of the stack, or until the path is empty.
                while(!path.empty() and path.back()->destNode() != stack.top()->sourceNode()){
                    dbg::out(dbg::info, "colorpath") << "        Popping edge: " << path.back()->getName() << std::endl;
                    path.pop_back();
                }

            } else {

                // Push back all edges originating from the target edge.
                foreach(Edge* edge, currentEdge->destNode()->outgoingEdges()){
                    dbg::out(dbg::info, "colorpath") << "        Pushing edge: " << edge->getName() << std::endl;
                    stack.push(edge);
                }
                dbg::out(dbg::info, "colorpath") << "        Current stack: " << util::qPointerStackToString(stack) << std::endl;
            }
        }
    }
    std::cout <<  "paths found: " << paths.size() << std::endl;

    dbg::out(dbg::info, "colorpath") << "DFS done" << std::endl;

//    foreach(QList<Edge*> path, paths){
//        std::cout << "Path: ";
//        foreach(Edge* edge, path){
//            std::cout << edge->getName() << " -> ";
//        }
//        std::cout << "end" << std::endl;
//    }

    std::cout << "Dividing paths into streams..." << std::endl;
    QList<path_t> unassignedPaths = paths;
    QList<stream_t> streams;
    stream_t primary_stream_set;
//    size_t threshold = 1;
//    while(true){
    int i=0;
    while(unassignedPaths.size() > 0){
        if(threshold >= 0){
            primary_stream_set = splitPaths(unassignedPaths, threshold);
        } else {
            primary_stream_set = splitPathsPercent(unassignedPaths, thresholdf);
        }
        dbg::out(dbg::info, "colorpath") << "Stream " << ++i << ": " << primary_stream_set.size() << std::endl;
        foreach(path_t path, primary_stream_set){
            unassignedPaths.removeOne(path);
        }
        streams.push_back(primary_stream_set);
    }
    std::cout << "Streams found: " << streams.size() << std::endl;

    dbg::out(dbg::info, "colorpath") << "Number of streams split: " << streams.size() << std::endl;


    std::cout << "Coloring edges..." << std::endl;
    QList<QColor> streamColors;
    for(int i=0; i<streams.size(); ++i){
        QColor color;
        color.setHsv((255.0/float(streams.size())) * float(i), 255, 255);
        streamColors.push_back(color);
        dbg::out(dbg::info, "colorpath") << "Stream: " << i << " color: " << color.red() << ", " << color.green() << ", " << color.blue() << std::endl;
    }
//        if(primary_stream_set.size() < paths.size()) break;
//        ++threshold;
//    }
    //std::cout << "Final threshold: " << threshold << std::endl;

//    // Classify edges
//    QSet<Edge*> edge_primary_set;
//    QSet<Edge*> edge_remainder_set;
//    for(int i=0; i<paths.size(); ++i){
//        if(primary_stream_set.contains(i)){
//            foreach(Edge* edge, paths[i]){
//                edge_primary_set.insert(edge);
//            }
//        } else {
//            foreach(Edge* edge, paths[i]){
//                edge_remainder_set.insert(edge);
//            }
//        }
//    }
//
//    // Split the remainder with the same threshold
////    QList<path_t> remainder_paths = edge_remainder_set.toList();
//
//    // Paint edges according to class
//    bool primary;
//    bool remainder;
    foreach(Edge* edge, edges){
        QColor color(255,255,255);
        bool colorAssigned = false;
        for(int i=0; i<streams.size(); ++i){
            bool contains = false;

            // Is the edge a member of stream i
            foreach(path_t path, streams[i]){
                foreach(Edge* stream_edge, path){
                    if(edge == stream_edge) {
                        contains = true;
                        break;
                    }
                }
                if(contains) break;
            }

            // If so, color it according to stream i.
            if(contains){
                if (!colorAssigned){
                    color = streamColors[i];
                    colorAssigned=true;
                } else {
                    dbg::out(dbg::info, "colorpath") << "Edge: " << edge->getName() << " assigned to stream: " << i << std::endl;
                    int r = (color.red() + streamColors[i].red())/3;
                    if(r<0) r=0;
                    int g = (color.green() + streamColors[i].green())/3;
                    if(g<0) g=0;
                    int b = (color.blue() + streamColors[i].blue())/3;
                    if(b<0) b=0;
                    dbg::out(dbg::info, "colorpath") << "Edge color: " << r << ", " << g << ", " << b << std::endl;
                    color.setRgb(r,g,b);
                }
            }
        }
        edge->setCustomColor(color);
    }


    foreach(Node* node, nodes){
        QColor color(255,255,255);
        bool colorAssigned = false;
        for(int i=0; i<streams.size(); ++i){
            bool contains = false;

            // Is the edge a member of stream i
            foreach(path_t path, streams[i]){
                foreach(Edge* stream_edge, path){
                    foreach(Edge* in_edge, node->incomingEdges()){
                        if(in_edge == stream_edge) {
                            contains = true;
                            break;
                        }
                    }
                    if(contains) break;
                    foreach(Edge* in_edge, node->outgoingEdges()){
                        if(in_edge == stream_edge) {
                            contains = true;
                            break;
                        }
                    }
                    if(contains) break;
                }
                if(contains) break;
            }

            // If so, color it according to stream i.
            if(contains){
                if (!colorAssigned){
                    color = streamColors[i];
                    colorAssigned=true;
                } else {
//                    dbg::out(dbg::info, "colorpath") << "Edge: " << edge->getName() << " assigned to stream: " << i << std::endl;
                    int r = (color.red() + streamColors[i].red())/3;
                    if(r<0) r=0;
                    int g = (color.green() + streamColors[i].green())/3;
                    if(g<0) g=0;
                    int b = (color.blue() + streamColors[i].blue())/3;
                    if(b<0) b=0;
//                    dbg::out(dbg::info, "colorpath") << "Edge color: " << r << ", " << g << ", " << b << std::endl;
                    color.setRgb(r,g,b);
                }
            }
        }
        node->setCustomColor(color);
    }

    std::cout << "Coloring edges: done." << std::endl;
}

