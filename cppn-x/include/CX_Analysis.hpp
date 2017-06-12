/*
 * CX_Analysis.hpp
 *
 *  Created on: May 1, 2017
 *      Author: joost
 */

#ifndef CPPN_X_INCLUDE_CX_ANALYSIS_HPP_
#define CPPN_X_INCLUDE_CX_ANALYSIS_HPP_

//QT includes
#include <QList>
#include <QFile>

// Local includes
#include "CE_Defines.h"
#include "CX_Debug.hpp"
#include "CE_ActivationFunctions.h"
#include "CX_Statistics.hpp"
#include "CX_Timer.hpp"


enum MutationType {addNeuron, addConnection, makeColor};

// Debug macro's to reduce the length of debug statements.
#define DBOM dbg::out(dbg::info, "mutation")
#define DBOA dbg::out(dbg::info, "analysis")

namespace analysis{


QString getMainFile(QString genDir, int branchNr){
    QString branchNrStr = util::toQString(branchNr);
	if(branchNr == -1){
		QString message(QString("Invalid branch: ") + branchNrStr);
		return QString("");
	}

	QString directory = genDir + "/" + branchNrStr;
	QFileInfo dirInfo(directory);
	if(!dirInfo.exists()){
		QString message(QString("Directory does not exist: ") + directory);
		return QString("");
	}

	QString mainFile = directory + "/main.zip";
	QFileInfo fileInfo(mainFile);
	if(!fileInfo.exists()){
		QString message(QString("File does not exist: ") + mainFile);
		return QString("");
	}

	return mainFile;
}

Cppn* getBranchCppn(QString genDir, int branchNr, int generation=0){
    dbg::trace trace("window", DBG_HERE);
    QString branchNrStr = util::toQString(branchNr);
    QString mainFile = getMainFile(genDir, branchNr);
    if(mainFile == "") return 0;

	MainParser parser (mainFile);
	parser.parse();

	if(generation == -1){
		generation = parser.max_generation;
	}

	QString zipStr = util::toQString(parser.storage_map[generation]);
	QString zipFile = genDir + "/" + branchNrStr + "/" + zipStr + ".zip";

	//Get generation
	CppnParser cppnParser(zipFile.toStdString());
	cppnParser.parse(generation);

	Cppn* cppn = new Cppn;
	cppn->setParentId(parser.parent);
	cppn->setMaxGen(parser.max_generation);
	QList<Node*> nodes = cppnParser.takeNodes();
	QList<Edge*> edges = cppnParser.takeEdges();
	foreach(Node* node, nodes){
		cppn->addNode(node);
	}

	foreach(Edge* edge, edges){
		cppn->addConnection(edge);
	}
	return cppn;

	//loadFile(zipFile, parser.max_generation);
}

void writeBranchingVsMetric(
		QString genDir,
		QDir& directory,
        QString fileName,
        std::map<int, double>& map,
        std::map<int, std::vector<int> >& reverseFamilyMap,
        double correction = 0){
    dbg::trace trace("window", DBG_HERE);
    QFile file(genDir + fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream file_out(&file);
    for (uint i = 0; i < directory.count(); ++i){
        int currentBranch = util::toInt(directory[i].toStdString());
        if(map.count(currentBranch)){
            double metric = map[currentBranch] - correction;
            size_t branching = 0;
            if(reverseFamilyMap.count(currentBranch)){
                branching = reverseFamilyMap[currentBranch].size();
            }
            file_out << branching << " " << metric << "\n";
        }
    }
    file.close();
}

std::vector<double> readNullModels(QString genDir, QString fileName){
    dbg::trace trace("window", DBG_HERE);
    std::vector<double> result;
    double averageModularity = 0;
    double averageForwardHierarchy = 0;
    double averageBackwardHierarchy = 0;
    size_t individuals = 0;

    QFile file(genDir + fileName);
    if(file.exists()){
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);

            while(!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(" ");
//                int individualBranch = fields[0].toInt();
                averageModularity += fields[2].toDouble();
                averageForwardHierarchy += fields[3].toDouble();
                averageBackwardHierarchy += fields[4].toDouble();
                individuals++;
            }
            file.close();
        }
    }

    result.push_back(averageModularity/double(individuals));
    result.push_back(averageForwardHierarchy/double(individuals));
    result.push_back(averageBackwardHierarchy/double(individuals));
    return result;
}




/**
 * Creates a new edge that is randomly connected between two unconnected nodes.
 */
Edge* addConMut(Cppn* cppn){
    dbg::trace trace("mutation", DBG_HERE);
    QSet<Node*> illegalTargetNodes;
    QList<Node*> legalSourceNodes;
    QList<Node*> legalTargetNodes;
    //typedef QList<Node*> node_list_t;

    DBOM << "cppn address: " << cppn << std::endl;
    foreach(Edge* edge, cppn->getEdges()){
    	DBOM << "  edge: " << edge->sourceNode()->getId() << " -> " <<
    			edge->destNode()->getId() << std::endl;
    }


    //Determine the source node
    foreach(Node* node, cppn->getNodes()){
        dbg::check_ptr(node, DBG_HERE);
        if(node->getNodeType() != Node::output){
            legalSourceNodes.push_back(node);
        }
    }

    while(!legalSourceNodes.empty()){
    	DBOM << "Number of legal sources: "
        		<< legalSourceNodes.size() << std::endl;
//        size_t sourceNodeIndex = randIndex(legalSourceNodes.size());
        Node* source = randElement(legalSourceNodes);
        DBOM << "Source chosen: " << source->getId() <<
                " type: " << source->getType() << std::endl;

        //Determine the legal targets
        illegalTargetNodes = cppn->getInputs().toSet();
        foreach(Node* node, illegalTargetNodes){
        	DBOM << "  input: " << node->getId() << std::endl;
        }
        QSet<Node*> pred = cppn->getPredecessors(source);
        foreach(Node* node, pred){
        	DBOM << "  predecessor: " << node->getId() << std::endl;
        }
        illegalTargetNodes.unite(pred);
        foreach(Node* node, illegalTargetNodes){
        	DBOM << "  illegal target: " << node->getId() << std::endl;
        }
        foreach(Node* node, cppn->getNodes()){
            if(!illegalTargetNodes.contains(node) &&
            		!cppn->connected(source, node)){
                legalTargetNodes.push_back(node);
            }
        }
        DBOM << "Number of legal targets: "
        		<< legalTargetNodes.size() << std::endl;
        foreach(Node* node, legalTargetNodes){
        	DBOM << "  legal target: " << node->getId() << std::endl;
        }

        if(legalTargetNodes.empty()){
            legalSourceNodes.removeAll(source);
        } else {
            Node* target = randElement(legalTargetNodes);
            DBOM << "Target chosen: " <<
            		target->getId() << " type: " <<
					target->getType() << std::endl;
            qreal w = randDouble(-3.0, 3.0);
            Edge* edge = new Edge("0", cppn->getNextId(), source, target, w, w);
            return edge;
        }
    }
    return 0;
}

/**
 * Creates a node and two connections that are supposed to be splices onto an
 * existing connection.
 *
 * Returns a pair containing a list of three Edges and a Node.
 * The first edge is the selected edge, and this edge should be removed from the
 * cppn. The second and third edges are the new edges replacing the old edge,
 * and they are supposed to be added to the cppn after the node has been added.
 * The node should simply be added the the cppn.
 */
QPair<QList<Edge*>, Node*> spliceNodeMut(Cppn* cppn, std::string branch = ""){
    dbg::trace trace("mutation", DBG_HERE);
    QPair<QList<Edge*>, Node*> result;
    Node* node;

    qreal w;

    //Select random edge
    size_t selectedEdgeIndex = randIndex(cppn->getNrOfEdges());
    Edge* selectedEdge = cppn->getEdge(selectedEdgeIndex);

    std::string id = util::toString(cppn->getMaxId()+1);
    std::string type = XML_TYPE_HIDDEN;
    std::string activationFunction = util::getRandomActivationFunction();

    node = new Node(branch, id, type, activationFunction);

    w = randDouble(-3.0, 3.0);
    id = util::toString(cppn->getMaxId()+2);
    Edge* edge1 = new Edge(branch, id, selectedEdge->sourceNode(), node, w, w);

    id = util::toString(cppn->getMaxId()+3);
    Edge* edge2 = new Edge(branch, id, node, selectedEdge->destNode(), 1, 1);

    //Create the return object
    result.first.push_back(selectedEdge);
    result.first.push_back(edge1);
    result.first.push_back(edge2);
    result.second = node;
    return result;
}


Edge* randEdge(std::string branch, id_t& id, Node *source, Node *target){
    double w = randDouble(-3.0, 3.0);
    std::string id_str = util::toString(++id);
    Edge* edge = new Edge(branch, id_str, source, target, w, w);
    return edge;
}

/**
 * Creates a node and two connections that are supposed to be splices onto an
 * existing connection.
 *
 * Returns a pair containing a list of three Edges and a Node.
 * The first edge is the selected edge, and this edge should be removed from the
 * cppn. The second and third edges are the new edges replacing the old edge,
 * and they are supposed to be added to the cppn after the node has been added.
 * The node should simply be added the the cppn.
 */
QPair<QList<Edge*>, QList<Node*> > makeColorMut(Cppn* cppn){
    dbg::trace trace("mutation", DBG_HERE);
    QPair<QList<Edge*>, QList<Node*> > result;
    std::string br = "";
    std::string id;
    std::string type;
    std::string lbl;
    std::string af;
    std::string affi = AFFINITY_COLOR;
    id_t id_i = cppn->getMaxId();
    Node* out_val = cppn->getOutputs().first();

    // Create nodes
    id = util::toString(++id_i);
    type = XML_TYPE_OUTPUT;
    lbl = OUTPUT_SATURATION;
    af = util::getRandomActivationFunction();
    Node* out_sat = new Node(br, id, type, af, lbl, affi);
    result.second.append(out_sat);

    id = util::toString(++id_i);
    type = XML_TYPE_OUTPUT;
    lbl = OUTPUT_HUE;
    af = util::getRandomActivationFunction();
    Node* out_hue = new Node(br, id, type, af, lbl, affi);
    result.second.append(out_hue);

    id = util::toString(++id_i);
    type = XML_TYPE_HIDDEN;
    lbl = "";
    af = util::getRandomActivationFunction();
    Node* hid_1 = new Node(br, id, type, af, lbl, affi);
    result.second.append(hid_1);

    id = util::toString(++id_i);
    type = XML_TYPE_HIDDEN;
    lbl = "";
    af = util::getRandomActivationFunction();
    Node* hid_2 = new Node(br, id, type, af, lbl, affi);
    result.second.append(hid_2);

    // Create Edges
    result.first.append(randEdge(br, id_i, hid_1, out_sat));
    result.first.append(randEdge(br, id_i, hid_2, out_sat));
    result.first.append(randEdge(br, id_i, hid_1, out_hue));
    result.first.append(randEdge(br, id_i, hid_2, out_hue));

    result.first.append(randEdge(br, id_i, out_val, out_sat));
    result.first.append(randEdge(br, id_i, out_val, out_hue));
    result.first.append(randEdge(br, id_i, out_val, hid_1));
    result.first.append(randEdge(br, id_i, out_val, hid_2));

    foreach(Node* node, cppn->getInputs()){
    	result.first.append(randEdge(br, id_i, node, hid_1));
    	result.first.append(randEdge(br, id_i, node, hid_2));
    }

    return result;
}


/**
 * Creates a cppn that only has the basic inputs and outputs.
 */
Cppn* createCppnWithIO(bool color){
    dbg::trace trace("analysis", DBG_HERE);
    std::string branch = "0";

    //Create a fully connected network
    Cppn* result = new Cppn();
    result->addNode(new Node(branch, result->getNextId(), XML_TYPE_INPUT,
    		XML_LINEAR, INPUT_X));
    result->addNode(new Node(branch, result->getNextId(), XML_TYPE_INPUT,
    		XML_LINEAR, INPUT_Y));
    result->addNode(new Node(branch, result->getNextId(), XML_TYPE_INPUT,
    		XML_LINEAR, INPUT_D));
    result->addNode(new Node(branch, result->getNextId(), XML_TYPE_INPUT,
    		XML_LINEAR, INPUT_BIAS));

    if(color){
        result->addNode(new Node(branch, result->getNextId(), XML_TYPE_OUTPUT,
        		util::getRandomActivationFunction(), OUTPUT_BRIGTHNESS));
        result->addNode(new Node(branch, result->getNextId(), XML_TYPE_OUTPUT,
        		util::getRandomActivationFunction(), OUTPUT_SATURATION));
        result->addNode(new Node(branch, result->getNextId(), XML_TYPE_OUTPUT,
        		util::getRandomActivationFunction(), OUTPUT_HUE));
    } else {
        result->addNode(new Node(branch, result->getNextId(), XML_TYPE_OUTPUT,
        		XML_LINEAR, OUTPUT_INK));
    }

    return result;
}


/**
 * Creates an initial cppn network with 4 inputs, 1 (grey) or 3 (color) outputs,
 * where all inputs are connected to all outputs.
 */
Cppn* createInitialCppn(bool color){
    dbg::trace trace("analysis", DBG_HERE);
    std::string branch = "0";
    std::string id;
    qreal w;
    Edge* edge;

    //Create a fully connected network
    Cppn* result = createCppnWithIO(color);

    foreach(Node* input, result->getInputs()){
        foreach(Node* output, result->getOutputs()){
            w = randDouble(-3.0, 3.0);
            edge = new Edge(branch, result->getNextId(), input, output, w, w);
            result->addConnection(edge);
        }
    }

    return result;
}

/**
 * Generates a random cppn network according to NEAT mutation operators.
 *
 * Abides by the following constraints:
 * - The resulting network has a specific number of nodes and connections
 * - The network is feed-forward
 * - Inputs never have incoming connections, outputs never have outgoing
 *   connections
 * - There exists a path from every input to every output
 * - Every node is reachable from at least one input,
 *   and every node can reach at least one output
 */
Cppn* randCppnNeatMut(size_t nrOfNeurons, size_t nrOfConnections, bool color){
    dbg::trace trace("analysis", DBG_HERE);
    QList<MutationType> mutations;

    //Create a fully connected network
    Cppn* result = createInitialCppn(color);

   //Apply the requested mutations in a random order
   dbg::assertion(DBG_ASSERTION(result->getNrOfNodes() <= nrOfNeurons));
   for(size_t i=result->getNrOfNodes(); i<nrOfNeurons; ++i){
       mutations.push_back(addNeuron);
       --nrOfConnections;
   }

   dbg::assertion(DBG_ASSERTION(result->getNrOfEdges() <= nrOfConnections));
   for(size_t i=result->getNrOfEdges(); i<nrOfConnections; ++i){
       mutations.push_back(addConnection);
   }

   if(mutations.size() > 1){
       //Always start with an add neuron mutation, if possible
       randomShuffle(mutations.begin()+1, mutations.end());
   }

   QPair<QList<Edge*>, Node*> nodeAndEdges;
   Edge* edge;

   while(!mutations.empty()){
       MutationType mutationType = mutations.front();

       switch(mutationType){
       case addNeuron:
    	   DBOM << "Adding neuron" << std::endl;
           nodeAndEdges = spliceNodeMut(result);
           result->removeConnection(nodeAndEdges.first[0]);
           result->addNode(nodeAndEdges.second);
           result->addConnection(nodeAndEdges.first[1]);
           result->addConnection(nodeAndEdges.first[2]);
           mutations.pop_front();
           break;
       case addConnection:
    	   DBOM << "Attempt to add edge" << std::endl;
           edge = addConMut(result);
           if(edge){
        	   DBOM << "Adding edge success" << std::endl;
               result->addConnection(edge);
               mutations.pop_front();
           } else {
        	   DBOM << "Adding edge failed" << std::endl;
               int index = mutations.indexOf(addNeuron);
               if(index != -1){
            	   DBOM << "Trying add neuron mutation" << std::endl;
                   mutations.swap(0, index);
               } else {
                   std::cout << "Warning: CPPN does not have the requested size"
                		   << std::endl;
                   //We have failed to produce a network of the required size
                   //Return our current result anyway.
                   return result;
               }
           }
           break;
       default:
           dbg::sentinel(DBG_HERE);
       }
   }
   return result;
}

/**
 * Generates a random cppn network according to NEAT mutation operators.
 *
 * Abides by the following constraints:
 * - The resulting network has a specific number of nodes and connections
 * - The network is feed-forward
 * - Inputs never have incoming connections, outputs never have outgoing
 *   connections
 * - There exists a path from every input to every output
 * - Every node is reachable from at least one input,
 *   and every node can reach at least one output
 */
Cppn* randCppnNeatMutPar(size_t nrOfNeurons, size_t nrOfCon, bool color,
		Cppn* parent)
{
    dbg::trace trace("analysis", DBG_HERE);
    QList<MutationType> mutations;

    bool parentColor = parent->getOutputs().size() > 1;
    if(parentColor && !color){
    	mutations.push_back(makeColor);
    }

    //Create a fully connected network
    Cppn* result = new Cppn(parent);

   //Apply the requested mutations in a random order
   dbg::assertion(DBG_ASSERTION(result->getNrOfNodes() <= nrOfNeurons));
   for(size_t i=result->getNrOfNodes(); i<nrOfNeurons; ++i){
       mutations.push_back(addNeuron);
       --nrOfCon;
   }

   dbg::assertion(DBG_ASSERTION(result->getNrOfEdges() <= nrOfCon));
   for(size_t i=result->getNrOfEdges(); i<nrOfCon; ++i){
       mutations.push_back(addConnection);
   }

   if(mutations.size() > 1){
       //Always start with an add neuron mutation, if possible
       randomShuffle(mutations.begin()+1, mutations.end());
   }

   QPair<QList<Edge*>, Node*> nodeAndEdges;
   QPair<QList<Edge*>, QList<Node*> > nodeAndEdges_2;
   Edge* edge;

   while(!mutations.empty()){
       MutationType mutationType = mutations.front();

       switch(mutationType){
       case addNeuron:
    	   DBOM << "Adding neuron" << std::endl;
           nodeAndEdges = spliceNodeMut(result);
           result->removeConnection(nodeAndEdges.first[0]);
           result->addNode(nodeAndEdges.second);
           result->addConnection(nodeAndEdges.first[1]);
           result->addConnection(nodeAndEdges.first[2]);
           mutations.pop_front();
           break;
       case addConnection:
    	   DBOM << "Attempt to add edge" << std::endl;
           edge = addConMut(result);
           if(edge){
        	   DBOM << "Adding edge success" << std::endl;
               result->addConnection(edge);
               mutations.pop_front();
           } else {
        	   DBOM << "Adding edge failed" << std::endl;
               int index = mutations.indexOf(addNeuron);
               if(index != -1){
            	   DBOM << "Trying add neuron mutation" << std::endl;
                   mutations.swap(0, index);
               } else {
                   std::cout << "Warning: CPPN does not have the requested size"
                		   << std::endl;
                   //We have failed to produce a network of the required size
                   //Return our current result anyway.
                   return result;
               }
           }
           break;
       case makeColor:
    	   DBOM << "Making color" << std::endl;
    	   nodeAndEdges_2 = makeColorMut(result);
           foreach(Node* node, nodeAndEdges_2.second){
        	   result->addNode(node);
           }
           foreach(Edge* edge, nodeAndEdges_2.first){
        	   result->addConnection(edge);
           }
           break;
       default:
           dbg::sentinel(DBG_HERE);
       }
   }
   return result;
}

Statistics calcStat(Cppn* cppn, Cppn* orgCppn, int branch, size_t it)
{
    dbg::trace trace("analysis", DBG_HERE);
    Statistics stat;
    size_t possibleCon = 0;
    size_t matchingCon = 0;
    size_t actualCon = cppn->getNrOfEdges();
    for(size_t i=0; i<cppn->getNrOfNodes(); ++i){
        for(size_t j=0; j<cppn->getNrOfNodes(); ++j){
            if(cppn->getNode(i)->outgoingEdges().size() > 0 &&
                    cppn->getNode(j)->incomingEdges().size() > 0){
                ++possibleCon;
                if(cppn->connected(i,j) && orgCppn->connected(i,j)){
                    ++matchingCon;
                }
            }
        }
    }
    double chanceOfSelection = double(actualCon)/double(possibleCon);
    double expectedValue = chanceOfSelection*double(actualCon);

    double modularity = cppn->calculateModularity();
    double hierarchyForwardRandom =  getGlobalReachingCentrality(*cppn,
    		forwardReachableNodeCount());
    double hierarchyBackwardRandom =  getGlobalReachingCentrality(*cppn,
    		backwardReachableNodeCount());

    stat.branch = branch;
    stat.iteration = it;
    stat.modularity = modularity;
    stat.hierarchyForward = hierarchyForwardRandom;
    stat.hierarchyBackward = hierarchyBackwardRandom;
    stat.possibleCon = possibleCon;
    stat.actualCon = actualCon;
    stat.matchingCon = matchingCon;
    stat.expectedValue = expectedValue;

    dbg::out(dbg::info, "analysis") <<
            "  Modularity: "<< modularity <<
            " hierarchy forward: " << hierarchyForwardRandom <<
            " hierarchy backward: " << hierarchyBackwardRandom <<
            " nb of edges: " << actualCon <<
            " nb of nodes: " << cppn->getNrOfNodes() <<
            " possible connections: " << possibleCon <<
            " matching connections: " << matchingCon <<
            " expected value: " << expectedValue <<
            std::endl;

    return stat;
}

void generateRandomNetwork(Cppn* originalCppn, int branch, size_t iteration,
		QTextStream& stream)
{
    dbg::trace trace("analysis", DBG_HERE);
    Cppn cppn;
    cppn.copy(originalCppn);
    size_t rewires = 10*cppn.getNrOfEdges();
    size_t tries = 200*cppn.getNrOfEdges();
    dbg::out(dbg::info, "analysis") << "Generating random network" << std::endl;
    while(rewires > 0 && tries > 0){
        --tries;
        size_t connectionIndex1 = randIndex(cppn.getNrOfEdges());
        size_t connectionIndex2 = randIndex(cppn.getNrOfEdges());
        Edge* edge1 = cppn.getEdge(connectionIndex1);
        Edge* edge2 = cppn.getEdge(connectionIndex2);
        Node* source1 = edge1->sourceNode();
        Node* target1 = edge1->destNode();
        Node* source2 = edge2->sourceNode();
        Node* target2 = edge2->destNode();
        if(cppn.connected(source1, target2) || cppn.connected(source2, target1)){
            continue;
        }
        cppn.rewireConnection(edge1, source1, target2);
        cppn.rewireConnection(edge2, source2, target1);
        if(cppn.connectionCausesCycle(edge1) || cppn.connectionCausesCycle(edge2)){
            cppn.rewireConnection(edge1, source1, target1);
            cppn.rewireConnection(edge2, source2, target2);
            continue;
        }
        --rewires;
    }
    dbg::out(dbg::info, "analysis") << "Generating random network done" << std::endl;

    size_t possibleConnections = 0;
    size_t matchingConnections = 0;
    size_t actualConnections = cppn.getNrOfEdges();
    for(size_t i=0; i<cppn.getNrOfNodes(); ++i){
        for(size_t j=0; j<cppn.getNrOfNodes(); ++j){
            if(cppn.getNode(i)->outgoingEdges().size() > 0 &&
                    cppn.getNode(j)->incomingEdges().size() > 0){
                ++possibleConnections;
                if(cppn.connected(i,j) && originalCppn->connected(i,j)){
                    ++matchingConnections;
                }
            }
        }
    }
    double chanceOfSelection = double(actualConnections)/double(possibleConnections);
    double expectedValue = chanceOfSelection*double(actualConnections);

    double modularity = cppn.calculateModularity();
    double hierarchyForwardRandom =  getGlobalReachingCentrality(cppn,
    		forwardReachableNodeCount());
    double hierarchyBackwardRandom =  getGlobalReachingCentrality(cppn,
    		backwardReachableNodeCount());

    stream << branch <<
            " " << iteration <<
            " " << modularity <<
            " " << hierarchyForwardRandom <<
            " " << hierarchyBackwardRandom <<
            " " << rewires <<
            " " << tries <<
            " " << possibleConnections <<
            " " << actualConnections <<
            " " << matchingConnections <<
            " " << expectedValue << "\n";

    dbg::out(dbg::info, "analysis") <<
            "  Modularity: "<< modularity <<
            " hierarchy forward: " << hierarchyForwardRandom <<
            " hierarchy backward: " << hierarchyBackwardRandom <<
            " rewires: " << rewires <<
            " retries: " << tries <<
            " possible connections: " << possibleConnections <<
            " actual connections: " << actualConnections <<
            " matching connections: " << matchingConnections <<
            " expected value: " << expectedValue <<
            std::endl;
}

/***
 * Highly experimental
 */
void startAnalysis(QString genDir){
    dbg::trace trace("analysis", DBG_HERE);
    QIODevice::OpenMode mode;
    QDir dir;
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setPath(genDir);
    std::cout << "Paths found: " << dir.count() << std::endl;

    //Build the family map, modularity map and reverse family maps based on the
    //genome data
    std::map<int, int> familyMap;
    std::map<int, double> modularityMap;
    std::map<int, double> forwardHierarchyMap;
    std::map<int, double> backwardHierarchyMap;
    std::map<int, double> corrModularityMap;
    std::map<int, double> corrForwardHierarchyMap;
    std::map<int, double> corrBackwardHierarchyMap;
    std::map<int, std::vector<int> > reverseFamilyMap;

    QList<int> parentPointToSelf;

    QFile file(genDir + "/genome_data.dat");
    if(file.exists()){
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);

            while(!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(" ");
                int child = fields[0].toInt();
                int parent = fields[1].toInt();
                if (child == parent){
                    parentPointToSelf.append(child);
                }
                double modularity = fields[2].toDouble();
                double forwardHierarchy = fields[3].toDouble();
                double backwardHierarchy = fields[4].toDouble();
                corrModularityMap[child] = fields[5].toDouble();
                corrForwardHierarchyMap[child] = fields[6].toDouble();
                corrBackwardHierarchyMap[child] = fields[7].toDouble();
                familyMap[child] = parent;
                modularityMap[child] = modularity;
                forwardHierarchyMap[child] = forwardHierarchy;
                backwardHierarchyMap[child] = backwardHierarchy;
                if(!reverseFamilyMap.count(parent)){
                    reverseFamilyMap[parent] = std::vector<int>();
                }
                reverseFamilyMap[parent].push_back(child);
            }
            file.close();
        }
    }

    // Open files for writing
    mode = QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append;

    //Log broken dependencies
    QFile brDepFile(genDir + "/broken_depencies.dat");
    brDepFile.open(mode);
    QTextStream brokenDepFileStream(&brDepFile);
    foreach(int branch, parentPointToSelf){
    	brokenDepFileStream << branch << "\n";
    }
    brokenDepFileStream.flush();
    brDepFile.close();

    // Read the data from the null models
    std::vector<double> nmData = analysis::readNullModels(genDir,
    		"/random_neat_mut_data.dat");

    //Write correlation files
    analysis::writeBranchingVsMetric(genDir, dir,
    		"/modularity_branching.dat",
    		modularityMap, reverseFamilyMap);
    analysis::writeBranchingVsMetric(genDir, dir,
    		"/forward_hierarchy_branching.dat",
    		forwardHierarchyMap, reverseFamilyMap);
    analysis::writeBranchingVsMetric(genDir, dir,
    		"/backward_hierarchy_branching.dat",
    		backwardHierarchyMap, reverseFamilyMap);
    analysis::writeBranchingVsMetric(genDir, dir,
    		"/corr2_modularity_branching.dat",
			corrModularityMap, reverseFamilyMap);
    analysis::writeBranchingVsMetric(genDir, dir,
    		"/corr2_forward_hierarchy_branching.dat",
    		corrForwardHierarchyMap, reverseFamilyMap);
    analysis::writeBranchingVsMetric(genDir, dir,
    		"/corr2_backward_hierarchy_branching.dat",
    		corrBackwardHierarchyMap, reverseFamilyMap);
    analysis::writeBranchingVsMetric(genDir, dir,
    		"/corrected_modularity_branching.dat",
    		modularityMap, reverseFamilyMap, nmData[0]);
    analysis::writeBranchingVsMetric(genDir, dir,
    		"/corrected_forward_hierarchy_branching.dat",
    		forwardHierarchyMap, reverseFamilyMap, nmData[1]);
    analysis::writeBranchingVsMetric(genDir, dir,
    		"/corrected_backward_hierarchy_branching.dat",
    		backwardHierarchyMap, reverseFamilyMap, nmData[2]);

    Timer timer;


    //Open the genome file
    file.open(mode);
    QTextStream out(&file);

    QFile randomNeatMutNullModelFile(genDir + "/child_parent_null_data.dat");
    randomNeatMutNullModelFile.open(mode);
    QTextStream randNeatMutNullModelFs(&randomNeatMutNullModelFile);

    //For each network
    uint directoryCount = dir.count();
    for (uint i = 0; i < directoryCount; ++i){
        int branch = util::toInt(dir[i].toStdString());
        std::cout << "Processing individual: " << i <<
        		"  branch: " << branch << "... ";
        if(familyMap.count(branch)){
            std::cout << " Branch already in map, break." << std::endl;
            continue;
        }

        // Time current operation
        timer.update();
        timer.printExpected(directoryCount, i);

        // Load the current network
        Cppn* origCppn = 0;
        try{
        	std::cout << "Loading network: " << branch << std::endl;
        	origCppn = analysis::getBranchCppn(genDir, branch);
//            _loadBranch(branch);
        } catch(CeParseException& e){
        	std::cout << "###### ERROR LOADING CHILD #######" << std::endl;
        	std::cout << "##################################" << std::endl;
            std::cout << e.what() << std::endl;
            std::cout << "##################################" << std::endl;
            continue;
        }

        // Load the parent network
        Cppn* parCppn = 0;
        int parent = origCppn->getParentId();
        if(parent != -1){
        	try{
        		std::cout << "Loading parent: " << parent << std::endl;
        		parCppn = analysis::getBranchCppn(genDir, parent);
        	} catch(CeParseException& e){
        		std::cout << "###### ERROR LOADING PARENT ######" << std::endl;
        		std::cout << "##################################" << std::endl;
        		std::cout << e.what() << std::endl;
        		std::cout << "##################################" << std::endl;
        		continue;
        	}
        } else {
        	try{
        		std::cout << "Loading parent: " << branch << std::endl;
        		parCppn = analysis::getBranchCppn(genDir, branch, 0);
        	} catch(CeParseException& e){
        		std::cout << "###### ERROR LOADING PARENT ######" << std::endl;
        		std::cout << "##################################" << std::endl;
        		std::cout << e.what() << std::endl;
        		std::cout << "##################################" << std::endl;
        		continue;
        	}
        }

        // Calculate modularity and hierarchy
        double qscore = origCppn->calculateModularity();
        double forHierOrg = getGlobalReachingCentrality(*origCppn,
        		forwardReachableNodeCount());
        double backHierOrg = getGlobalReachingCentrality(*origCppn,
        		backwardReachableNodeCount());

        dbg::out(dbg::info, "analysis") << "Original network:" << std::endl;
        dbg::out(dbg::info, "analysis") << "  Modularity: " << qscore <<
                " hierarchy forward: " << forHierOrg <<
                " hierarchy backward: " << backHierOrg <<
                " nr of nodes: " << origCppn->getNrOfNodes() <<
                " nr of edges: " << origCppn->getNrOfEdges() <<
                std::endl;

        bool color = origCppn->getOutputs().size() > 1;

        // Generate null-models
        DBOA << "Generating null-models networks: " << std::endl;

        size_t nbNullModels = 10;
        double avNullQ = 0;
        double avNullFH = 0;
        double avNullBH = 0;
        Statistics statNeatMut;
        for(size_t j=0; j<nbNullModels; ++j){
        	DBOA << "  Generating random NEAT network" << std::endl;
        	Cppn* randNeatMut = 0;
        	size_t nbNodes = origCppn->getNrOfNodes();
        	size_t nbEdges = origCppn->getNrOfEdges();
        	randNeatMut = analysis::randCppnNeatMutPar(nbNodes, nbEdges, color,
        			parCppn);
            DBOA << "  Dumping random NEAT network" << std::endl;
            statNeatMut = analysis::calcStat(randNeatMut, origCppn, branch, j);
            statNeatMut.dump(randNeatMutNullModelFs);

            avNullQ += statNeatMut.modularity;
            avNullFH += statNeatMut.hierarchyForward;
            avNullBH += statNeatMut.hierarchyBackward;
            delete randNeatMut;
        }

        randNeatMutNullModelFs.flush();
        avNullQ /= double(nbNullModels);
        avNullFH /= double(nbNullModels);
        avNullBH /= double(nbNullModels);

        double corrQ = qscore - avNullQ;
        double corrFH = forHierOrg - avNullFH;
        double corrBH = backHierOrg - avNullBH;


        //Write all relevant information to a file
        out << branch <<
                " " << parent <<
                " " << qscore <<
                " " << forHierOrg <<
                " " << backHierOrg <<
				" " << corrQ <<
				" " << corrFH <<
				" " << corrBH <<
                " " << origCppn->getMaxGen() <<"\n";
        out.flush();

        //Delete cppn explicitly (easier to debug memory leaks).
        if(origCppn) delete origCppn;
        if(parCppn) delete parCppn;
    }

    file.close();
}

}

#undef DBOM
#undef DBOA
#endif /* CPPN_X_INCLUDE_CX_ANALYSIS_HPP_ */
