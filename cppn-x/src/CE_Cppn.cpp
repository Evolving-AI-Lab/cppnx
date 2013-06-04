/*
 * CE_Cppn.cpp
 *
 *  Created on: May 30, 2013
 *      Author: joost
 */

#include "CE_Cppn.h"
#include <queue>


static const double Pi = 3.14159265358979323846264338327950288419717;

void Cppn::addNode(std::string branch, std::string id, std::string type, std::string activationFunction, std::string label, QPointF position, QColor color){
	Node* QTnode = new Node(widget, branch, id, type, activationFunction, label, width, height, color);
	if(label == INPUT_X){
		nodes[input_x]=QTnode;
		QTnode->setIndex(input_x);
	} else if(label == INPUT_Y){
		nodes[input_y]=QTnode;
		QTnode->setIndex(input_y);
	} else if(label == INPUT_D){
		nodes[input_d]=QTnode;
		QTnode->setIndex(input_d);
	} else if(label == INPUT_BIAS){
		nodes[input_b]=QTnode;
		QTnode->setIndex(input_b);
	} else {
		nodes.push_back(QTnode);
	}
	QTnode->setPos(position);
	nodeMap[branch + "_" + id]=QTnode;
	numberOfNodes++;
	validPhenotype=false;

}


void Cppn::addConnection(std::string branch, std::string id, std::string source_branch, std::string source_id, std::string target_branch, std::string target_id, double weight, QColor color){
	edges.push_back(new Edge(widget, branch, id, nodeMap[source_branch+"_"+source_id], nodeMap[target_branch+"_"+target_id], weight, color));
	validPhenotype=false;
	numberOfEdges++;
}

void Cppn::setWeight(Edge* edge, double weight){
	if(edge->getWeight() == weight) return;
	edge->setWeight(weight);
	edge->update();
	if(validPhenotype){
		linkWeights[edge->getIndex()]=weight;
		//updateNodes();
	}
	//updateNodes(edge->destNode());
	updateFromLink(edge);
}



void Cppn::updateNodes(){
	if(!validPhenotype) buildPhenotype();

	//Set input values
	for(unsigned long x=0; x<width; x++){
		for(unsigned long y=0; y<height; y++){
			double xv = (double(x)/(double(width)/2) - 1);
			double yv = (double(y)/(double(height)/2) - 1);
			size_t index = x + y*width;

			updateNode(input_x, index, xv);
			updateNode(input_y, index, yv);
			updateNode(input_d, index, double(std::sqrt(float(xv*xv+yv*yv))*1.4));
			updateNode(input_b, index, 1.0);
		}
	}

	//Update the rest
	for(size_t currentNode=nr_of_inputs; currentNode<numberOfNodes; currentNode++){
//		std::cout << "Current node: " << currentNode << std::endl;
		for(size_t xy_index=0; xy_index < width*height;  xy_index++){
//			std::cout << "Index: " << xy_index << std::endl;
			updateNode(currentNode, xy_index);
		}
	}
}

std::vector< std::vector <Node*> > Cppn::buildLayers(){
	std::vector< std::vector <Node*> > layers;
	layers.push_back(std::vector <Node*>());
	std::vector <Node*> notPlaced = nodes;
	std::vector <Node*> nextNotPlaced;
	std::map <Node*, int> incommingEdges;

	for(int i=0; i<nodes.size(); i++){
		incommingEdges[nodes[i]]= nodes[i]->incomingEdges().size();
		//std::cout << nodes[i]->incomingEdges().size() << std::endl;
	}

	while(notPlaced.size()>0){
		nextNotPlaced.clear();
		for(int i=0; i<notPlaced.size(); i++){
			if(incommingEdges[notPlaced[i]] <= 0){
				layers.back().push_back(notPlaced[i]);
			} else {
				nextNotPlaced.push_back(notPlaced[i]);
			}
		}

		for(int i=0; i<layers.back().size(); i++){
			QList<Edge *> outgoingEdges = layers.back()[i]->outgoingEdges();
			foreach(Edge* outgoing_edge, outgoingEdges){
				incommingEdges.find(outgoing_edge->destNode())->second = incommingEdges[outgoing_edge->destNode()]-1;
			}
		}

		layers.push_back(std::vector <Node*>());
		notPlaced = nextNotPlaced;
	}

	return layers;
}


void Cppn::placeNode(Node* node, size_t index, size_t& lastTarget, size_t& lastSource){
	switch(node->getActivationFunction()){
	case ACTIVATION_FUNCTION_SIGMOID:
		activationFunctions[index]=act_functions::sigmoid;
		break;
	case ACTIVATION_FUNCTION_LINEAR:
		activationFunctions[index]=act_functions::identity;
		break;
	case ACTIVATION_FUNCTION_SIN:
		activationFunctions[index]=act_functions::sin;
		break;
	case ACTIVATION_FUNCTION_COS:
		activationFunctions[index]=act_functions::cos;
		break;
	case ACTIVATION_FUNCTION_GAUSSIAN:
		activationFunctions[index]=act_functions::gaussian;
		break;
	default:
		throw JGTL::LocatedException("Unknow activation function: '" + boost::lexical_cast<std::string>(node->getActivationFunction()) + "'");
		break;
	}
	node->setIndex(index);
	phenotypeNodes[index]=node;

	QList<Edge *> outgoingEdges = node->outgoingEdges();
	foreach(Edge* outgoing_edge, outgoingEdges){
		linkWeights[lastTarget] = outgoing_edge->getWeight();
		outgoing_edge->setIndex(lastTarget);
		lastTarget++;
	}

	QList<Edge *> incommingEdges = node->incomingEdges();
	foreach(Edge* incommingEdge, incommingEdges){
		nodeSources[lastSource] = incommingEdge->getIndex()*width*height;
		lastSource++;
	}

	lastTargets[index+1]=lastTarget;
	lastSources[index+1]=lastSource;
}

void Cppn::buildPhenotype(){
	deletePhenotype();
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



	size_t currentNodeIndex=nr_of_inputs;
	lastTargets[0]=0;
	lastSources[0]=0;
	size_t targetEdgeIndex=0;
	size_t sourceEdgeIndex=0;
	//size_t lastTarget=0;
	//size_t lastSource=0;

	//Put the input nodes in the right order
	Node* inputs[4];
	for(int i=0; i<layers[0].size(); i++ ){
		if(layers[0][i]->getType() == XML_TYPE_INPUT){
			inputs[layers[0][i]->getIndex()] = layers[0][i];
		}
	}

	//Process the input nodes
	for(size_t i=0; i<4; i++ ){
		placeNode(inputs[i], i, targetEdgeIndex, sourceEdgeIndex);
	}

	//Process the rest
	for(int i=0; i< layers.size(); i++){
		for(int j=0; j< layers[i].size(); j++){


			if(layers[i][j]->getType() == XML_TYPE_INPUT){
				//std::cout << "Input set" << std::endl;
				//placeNode(layers[i][j], layers[i][j]->getIndex(), targetEdgeIndex, sourceEdgeIndex);
			} else {
				placeNode(layers[i][j], currentNodeIndex, targetEdgeIndex, sourceEdgeIndex);
				currentNodeIndex++;
			}
		}
	}


	std::queue<Node*> toProcess;
	std::set<std::pair<size_t, size_t> > ancestor;

	for(size_t i=0; i<nodes.size(); i++){
		toUpdateStart[i]=ancestor.size();
		toProcess.push(phenotypeNodes[i]);
		size_t fromNodeIndex = phenotypeNodes[i]->getIndex();
		ancestor.insert(std::pair<size_t, size_t>(fromNodeIndex, fromNodeIndex));

		while(!toProcess.empty()){
			QList<Edge *> outgoingEdges = toProcess.front()->outgoingEdges();
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
//	std::cout << "ancestor.size() " << ancestor.size() << std::endl;
	toUpdateStart[nodes.size()]=ancestor.size();
	toUpdate=new size_t[ancestor.size()];

	size_t index=0;
	for(size_t fromNodeIndex=0; fromNodeIndex<nodes.size(); fromNodeIndex++){
		for(size_t toNodeIndex=0; toNodeIndex<nodes.size(); toNodeIndex++){
			if(ancestor.count(std::pair<size_t, size_t>(fromNodeIndex, toNodeIndex))==1){
				toUpdate[index]=toNodeIndex;
				index++;
			}
		}
	}

	validPhenotype=true;
}

void Cppn::positionNodesCircle(){
	std::vector< std::vector <Node*> > layers = buildLayers();


	int k =1;
	QPointF position1;
	QPointF position2;
	Node* edge1;
	qreal distanceEdge1;
	qreal distanceEdge2;

	for(int i=0; i< layers.size(); i++){
		for(int j=0; j< layers[i].size(); j++){
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

	int scale = 70;

	for(int i=0; i< layers.size(); i++){
		for(int j=0; j< layers[i].size(); j++){
			position = QPointF(j*scale-(int(layers[i].size())-1)*(scale/2), -(i*scale-(int(layers.size())-1)*(scale/2)));
			layers[i][j]->setPos(position);
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

		phenotypeNodes[toUpdate[i]]->update();
	}
}


//inline void Cppn::updateLink(const size_t& node, const size_t& link, const size_t& xy_index){
//	//Calculate outgoing links
//	linkChache[xy_index+link*width*height]=nodeChache[xy_index+node*width*height]*linkWeights[link];
//}



inline void Cppn::updateNode(const size_t& node, const size_t& xy_index, const double& initialValue){
	size_t index = xy_index+node*width*height;

	nodeChache[index]=initialValue;

	//Calculate the sum of incoming links

	for(size_t i = lastSources[node]; i<lastSources[node+1]; i++){
		//std::cout << "lastSources[node] " << lastSources[node] << "lastSources[node+1] " << lastSources[node+1] <<std::endl;
		nodeChache[index]+=linkChache[xy_index+nodeSources[i]];
	}

	//Apply the activation function
//	std::cout << "Activate function: " << (void *)activationFunctions[node] << std::endl;
//	std::cout << "sigmoid function: " << (void *)act_functions::sigmoid << std::endl;
//	std::cout << "identity function: " << (void *)act_functions::identity << std::endl;
//	std::cout << "sin function: " << (void *)act_functions::sin << std::endl;
//	std::cout << "cos function: " << (void *)act_functions::cos << std::endl;
//	std::cout << "gaussian function: " << (void *)act_functions::gaussian << std::endl;

	nodeChache[index]= (*activationFunctions[node])(nodeChache[index]);
//	std::cout << "Done" << std::endl;
	phenotypeNodes[node]->setPixel(xy_index, char(std::min(std::abs(nodeChache[index]), 1.0)*255));

	//Calculate outgoing links
	//std::cout << "lastTargets[node] " << lastTargets[node] << " lastTargets[node+1] " << lastTargets[node+1] <<std::endl;
	for(size_t i = lastTargets[node]; i<lastTargets[node+1]; i++){
		linkChache[xy_index+i*width*height]=nodeChache[index]*linkWeights[i];
	}
}


size_t Cppn::getNrOfColorButtons(){
	return widget->getWindow()->getNrOfColorButtons();
}

CE_ColorButton* Cppn::getColorButton(size_t i){
	return widget->getWindow()->getColorButton(i);
}

void Cppn::addColorButton(std::string text, QColor color){
	widget->getWindow()->addColorButton(QString(text.c_str()), color);
}
