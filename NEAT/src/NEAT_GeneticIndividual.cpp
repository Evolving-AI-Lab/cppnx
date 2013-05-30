#include "NEAT_GeneticIndividual.h"

#include "NEAT_GeneticNodeGene.h"
#include "NEAT_GeneticLinkGene.h"
#include "NEAT_GeneticSpecies.h"
#include "NEAT_ModularNetwork.h"
#include "NEAT_FastNetwork.h"
#include "NEAT_NetworkNode.h"
#include "NEAT_NetworkLink.h"
#include "NEAT_Random.h"
#include "NEAT_Globals.h"

#define DEBUG_GENETIC_INDIVIDUAL (0)

namespace NEAT
{
    //jmc: used at the beginning of hyperneat runs
    GeneticIndividual::GeneticIndividual(
        const vector<GeneticNodeGene> &_nodes,
        bool createTopology,
        double edgeDensity
        )
        :
    nodes(_nodes),
      fitness(0),
      mutatedAtBirth(false), 
      canReproduce(true),
      parent1Fitness(-1.0),//indiactes no 1st parent
      parent2Fitness(-1.0), //indicates no 2nd parent
      bestFitnessInLineage(0.0),
      directionChanges(-1) //number of leg direction changes in quadruped experiments


    {   
      bestFitnessInLineage = max(parent1Fitness, parent2Fitness);
        double AllowRecurrentConnectionsParamVal = Globals::getSingleton()->getParameterValue("AllowRecurrentConnections");
        bool allowRecurrentConnections = (AllowRecurrentConnectionsParamVal > Globals::getSingleton()->getRandom().getRandomDouble());

        double AllowSelfRecurrentConnectionsParamVal = Globals::getSingleton()->getParameterValue("AllowSelfRecurrentConnections");
        bool allowSelfRecurrentConnections = (AllowSelfRecurrentConnectionsParamVal> Globals::getSingleton()->getRandom().getRandomDouble());

        if (createTopology)
        {

            for (int a=0;a<(int)nodes.size();a++)
            {
                for (int b=0;b<(int)nodes.size();b++)
                {
                    if (nodes[a].isEnabled() && nodes[b].isEnabled())
                    {
                        if( (!nodes[a].isTopologyFrozen()) || (!nodes[b].isTopologyFrozen()))
                        {
                            //If either node allows topology, do it

                            if (nodes[b].getType()!=string("NetworkSensor"))
                            {
                                //Don't connect links to sensors

                                if(Globals::getSingleton()->getRandom().getRandomDouble()<edgeDensity)
                                {
                                    //Random chance of having a link

                                    if (allowRecurrentConnections||(nodes[a].getDrawingPosition()<nodes[b].getDrawingPosition()))
                                    {
                                        //Make sure recurrent connections are allowed

                                        if (allowSelfRecurrentConnections||((nodes[a].getID()!=nodes[b].getID())))
                                        {
                                            //Make sure self-recurrent connections are allowed

                                            addLink(GeneticLinkGene(nodes[a].getID(),nodes[b].getID()));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //jmc: used at the beginning of pneat runs
    GeneticIndividual::GeneticIndividual(
        const vector<GeneticNodeGene> &_nodes,
        const vector<GeneticLinkGene> &_links,
        bool randomizeLinkWeights,
        bool createTopology,
        double edgeDensity

        )
        :
    nodes(_nodes),
      links(_links),
      fitness(0),
      mutatedAtBirth(false), 
      canReproduce(true),
      parent1Fitness(-1.0),//indicates no 1st parent
      parent2Fitness(-1.0), //indicates no 2nd parent
      bestFitnessInLineage(0.0),
      directionChanges(-1) //number of leg direction changes in quadruped experiments

    {
      bestFitnessInLineage = max(parent1Fitness, parent2Fitness);

        for (int a=0;a<(int)_links.size();a++)
        {
            if (randomizeLinkWeights)
            {
                double weight = Globals::getSingleton()->getRandom().getRandomDouble(0,6)-3.0;
                links[a].setWeight(weight);
            }
        }

        bool allowRecurrentConnections
            = (
            Globals::getSingleton()->getParameterValue("AllowRecurrentConnections")>
            Globals::getSingleton()->getRandom().getRandomDouble()
            );

        bool allowSelfRecurrentConnections
            = (
            Globals::getSingleton()->getParameterValue("AllowSelfRecurrentConnections")>
            Globals::getSingleton()->getRandom().getRandomDouble()
            );

        if (createTopology)
        {

            for (int a=0;a<(int)nodes.size();a++)
            {
                for (int b=0;b<(int)nodes.size();b++)
                {
                    if (nodes[a].isEnabled() && nodes[b].isEnabled())
                    {
                        if( (!nodes[a].isTopologyFrozen()) || (!nodes[b].isTopologyFrozen()))
                        {
                            //If either node allows topology, do it

                            if (nodes[b].getType()!=string("NetworkSensor"))
                            {
                                //Don't connect links to sensors

                                if(Globals::getSingleton()->getRandom().getRandomDouble()<edgeDensity)
                                {
                                    //Random chance of having a link

                                    if (allowRecurrentConnections||(nodes[a].getDrawingPosition()<nodes[b].getDrawingPosition()))
                                    {
                                        //Make sure recurrent connections are allowed

                                        if (allowSelfRecurrentConnections||((nodes[a].getID()!=nodes[b].getID())))
                                        {
                                            //Make sure self-recurrent connections are allowed

                                            //Check to see if the link already exists
                                            bool skip=false;
                                            for (int c=0;c<(int)_links.size();c++)
                                            {
                                                if (_links[c].getFromNodeID()==nodes[a].getID()
                                                    &&_links[c].getToNodeID()==nodes[b].getID())
                                                {
                                                    skip=true;
                                                    break;
                                                }

                                                if (_links[c].getFromNodeID()==nodes[b].getID()
                                                    &&_links[c].getToNodeID()==nodes[a].getID())
                                                {
                                                    skip=true;
                                                    break;
                                                }
                                            }

                                            if (!skip)
                                            {
                                                addLink(GeneticLinkGene(nodes[a].getID(),nodes[b].getID(),Globals::getSingleton()->getRandom().getRandomDouble(-3,3)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //jmc: used when loading from xlm files
    GeneticIndividual::GeneticIndividual(TiXmlElement *individualElement):
      mutatedAtBirth(false), 
      canReproduce(true),
      parent1Fitness(-1.0),//indicates no 1st parent
      parent2Fitness(-1.0), //indicates no 2nd parent
      directionChanges(-1), //number of leg direction changes in quadruped experiments
      bestFitnessInLineage(0.0)
    {
      bestFitnessInLineage = max(parent1Fitness, parent2Fitness);
        fitness = atof(individualElement->Attribute("Fitness"));
        speciesID = atoi(individualElement->Attribute("SpeciesID"));
        TiXmlElement *nodesElementPtr = individualElement->FirstChildElement("Nodes");
        TiXmlElement *nodeElementPtr = nodesElementPtr->FirstChildElement();

        do
        {
            addNode(GeneticNodeGene(nodeElementPtr));
        }
        while ( (nodeElementPtr = nodeElementPtr->NextSiblingElement()) );

        TiXmlElement *linksElementPtr = individualElement->FirstChildElement("Links");

        TiXmlElement *linkElementPtr = linksElementPtr->FirstChildElement();

        do
        {
            addLink(GeneticLinkGene(linkElementPtr));
            linkElementPtr = linkElementPtr->NextSiblingElement();
        }
        while ( linkElementPtr!=NULL );
    }

    //don't know when this is used (but if it is used, it should have a mutatedAtBirth, parent1Fitness, and parent2Fitness in its constructor
    GeneticIndividual::GeneticIndividual(istream &istr)
        :
    canReproduce(true)
    {
        CREATE_PAUSE(string("Error!: ")+toString(__LINE__));

    
        istr >> fitness >> speciesID;

#if DEBUG_GENETIC_INDIVIDUAL
        cout << "Fitness: " << fitness << " speciesID: " << speciesID << endl;
#endif

        int numNodes;

        istr >> numNodes;

#if DEBUG_GENETIC_INDIVIDUAL
        cout << "NumNodes: " << numNodes << endl;
#endif

        for (int a=0;a<numNodes;a++)
        {
            addNode(GeneticNodeGene(istr));
        }

        int numLinks;

        istr >> numLinks;

#if DEBUG_GENETIC_INDIVIDUAL
        cout << "NumLinks: " << numLinks << endl;
#endif

        for (int a=0;a<numLinks;a++)
        {
            addLink(GeneticLinkGene(istr));
        }
      bestFitnessInLineage = 0.0;
      parent1Fitness = -1.0;
      parent2Fitness = -1.0;
      
    }

    //jmc: combining two parents. mutates also with some probability
    GeneticIndividual::GeneticIndividual(shared_ptr<GeneticIndividual> parent1,shared_ptr<GeneticIndividual> parent2,bool mate_multipoint_avg)
        :
    fitness(0),
    mutatedAtBirth(false),
    canReproduce(true),
    parent1Fitness(parent1->getFitness()),
    parent2Fitness(parent2->getFitness()),
	directionChanges(-1) //number of leg direction changes in quadruped experiments

    {
      bestFitnessInLineage = max(parent1Fitness, parent2Fitness);
        if (parent1->getFitness()==0)
        {
            parent1->setFitness(0.0001);
        }

        if (parent2->getFitness()==0)
        {
            parent2->setFitness(0.0001);
        }

        double totalFitness = parent1->getFitness()+parent2->getFitness();

        int link1index = 0,link2index=0;

        double randomChoice;

        bool chooseFirstParent;

        #if DEBUG_MUTATE
        printf("jmc: parent1->getLinksCount(): %i\n", parent1->getLinksCount());
        printf("jmc: parent2->getLinksCount(): %i\n", parent2->getLinksCount());
        #endif
        while (link1index<parent1->getLinksCount()||link2index<parent2->getLinksCount())
        {
            randomChoice = Globals::getSingleton()->getRandom().getRandomDouble(double(0),totalFitness);
            chooseFirstParent = (randomChoice<=parent1->getFitness());

            GeneticLinkGene *link1=NULL,*link2=NULL;

            if (link1index<parent1->getLinksCount())
                link1 = parent1->getLink(link1index);
            if (link2index<parent2->getLinksCount())
                link2 = parent2->getLink(link2index);

            if(link1 && link1->isFixed())
            {
                cerr << "skipping" << endl;
                //Skip the link if it's fixed
                link1index++;
            }
            else if(link2 && link2->isFixed())
            {
                cerr << "skipping" << endl;
                //Skip the link if it's fixed
                link2index++;
            }
            else if (!link1 ||  (link1 && link2 && link2->getID()<link1->getID()))
            {
								if (!link2)
								{
										throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: LINK1 is non-null but LINK2 is NULL!");
								}
								addLink(GeneticLinkGene(*link2));
                link2index++;
            }
            else if (!link2 ||  (link1&&link2&&link1->getID()<link2->getID()))
            {
								addLink(GeneticLinkGene(*link1));
                link1index++;
            }
            else //both links exist and have the same ID
            {
                if (mate_multipoint_avg)
                {
                    double avgWeight = (link1->getWeight()+link2->getWeight())/2.0;
                    GeneticLinkGene newLink = GeneticLinkGene(link1->getFromNodeID(),link1->getToNodeID(),avgWeight);
                    addLink(newLink);
                }
                else
                {
                    //cout << "link1: " << link1->getWeight() << endl;
                    //cout << "link2: " << link2->getWeight() << endl;

                    if (chooseFirstParent)
                    {
                        addLink(GeneticLinkGene(*link1));
                    }
                    else
                    {
                        addLink(GeneticLinkGene(*link2));
                    }
                }
                link1index++;
                link2index++;
            }
        }

//        cout << "jmc here1\n";
        //Here we need to add the nodes that have incident links
        vector<int> nodesNeeded;

        //This loop populates the nodesNeeded list
        for (int linkIndex=0;linkIndex<(int)links.size();linkIndex++)
        {
            int fromNodeID = links[linkIndex].getFromNodeID();
            int toNodeID = links[linkIndex].getToNodeID();

            if (find(nodesNeeded.begin(),nodesNeeded.end(),fromNodeID)==nodesNeeded.end())
                nodesNeeded.push_back(fromNodeID);

            if (find(nodesNeeded.begin(),nodesNeeded.end(),toNodeID)==nodesNeeded.end())
                nodesNeeded.push_back(toNodeID);

        }
//        cout << "jmc here2\n";
        //These next two loops make sure that all the sensors and output nodes are
        //in both individuals.
        for (int nodeIndex=0;nodeIndex<parent1->getNodesCount();nodeIndex++)
        {
            GeneticNodeGene *nodeGene = parent1->getNode(nodeIndex);

            int nodeID = nodeGene->getID();

            if (nodeGene->getType()==string("NetworkSensor")||nodeGene->getType()==string("NetworkOutputNode"))
            {
                if (find(nodesNeeded.begin(),nodesNeeded.end(),nodeID)==nodesNeeded.end())
                    nodesNeeded.push_back(nodeID);
            }
        }
//        cout << "jmc here3\n";

        for (int nodeIndex=0;nodeIndex<parent2->getNodesCount();nodeIndex++)
        {
            GeneticNodeGene *nodeGene = parent2->getNode(nodeIndex);

            int nodeID = nodeGene->getID();

            if (nodeGene->getType()==string("NetworkSensor")||nodeGene->getType()==string("NetworkOutputNode"))
            {
                if (find(nodesNeeded.begin(),nodesNeeded.end(),nodeID)==nodesNeeded.end())
                    nodesNeeded.push_back(nodeID);
            }
        }
//        cout << "jmc here4\n";

        //These next two loops go through all of the nodes of both parents and include them
        //if they are needed in the child
        for (int nodeIndex=0;nodeIndex<parent1->getNodesCount();nodeIndex++)
        {
            GeneticNodeGene *parentNode = parent1->getNode(nodeIndex);

            int parentNodeID = parentNode->getID();

            vector<int>::iterator target = find(nodesNeeded.begin(),nodesNeeded.end(),parentNodeID);

            if (target!=nodesNeeded.end())
            {
                addNode(GeneticNodeGene(*parentNode));

                nodesNeeded.erase(target);
            }
        }
//        cout << "jmc here5\n";

        for (int nodeIndex=0;nodeIndex<parent2->getNodesCount();nodeIndex++)
        {
            GeneticNodeGene *parentNode = parent2->getNode(nodeIndex);

            int parentNodeID = parentNode->getID();

            vector<int>::iterator target = find(nodesNeeded.begin(),nodesNeeded.end(),parentNodeID);

            if (target!=nodesNeeded.end())
            {
                addNode(GeneticNodeGene(*parentNode));

                nodesNeeded.erase(target);
            }
        }
//        cout << "jmc here6\n";

        testMutate();
//        cout << "leaving GenInd constructor\n";
    }

    //jmc: possibly mutate only (no crossover)
    #include <iomanip>
    GeneticIndividual::GeneticIndividual(shared_ptr<GeneticIndividual> parent1,bool tryMutation)
        :
        nodes(parent1->nodes),
        links(parent1->links),
        fitness(0),
        mutatedAtBirth(false), 
        canReproduce(true),
        parent1Fitness(parent1->getFitness()),
        parent2Fitness(-1.0), //indicates no 2nd parent
		directionChanges(-1) //number of leg direction changes in quadruped experiments

    { 
      bestFitnessInLineage = max(parent1Fitness, parent2Fitness);
      if (tryMutation)
        testMutate();
    }

    GeneticIndividual::GeneticIndividual(GeneticIndividual &copy)
        :
        nodes(copy.nodes),
        links(copy.links)
    {
        fitness = copy.fitness;
        mutatedAtBirth = copy.mutatedAtBirth, 

        canReproduce = copy.canReproduce;

        speciesID = copy.speciesID;

        if (copy.userData)
        {
            userData = shared_ptr<Stringable>(copy.userData->clone());
        }
        else
        {
            userData = shared_ptr<Stringable>();
        }
      
      bestFitnessInLineage = copy.bestFitnessInLineage;
      parent1Fitness = copy.parent1Fitness;
      parent2Fitness = copy.parent2Fitness;
    }

    GeneticIndividual::~GeneticIndividual()
    {
    }

    bool GeneticIndividual::operator==(const GeneticIndividual &other) const
    {
        if (
            nodes.size() != other.nodes.size() ||
            links.size() != other.links.size()
            )
        {
            return false;
        }

        for (int a=0;a<(int)nodes.size();a++)
        {
            if (!(nodes[a]==other.nodes[a]))
            {
                return false;
            }
        }

        for (int a=0;a<(int)links.size();a++)
        {
            if (!(links[a]==other.links[a]))
            {
                return false;
            }
        }

        return true;
    }


#define DEBUG_MUTATE (0)

#define DEBUG_ONLY_ONE_MUTATION (0)

    void GeneticIndividual::testMutate()
    {
#if DEBUG_MUTATE
        cout << "Mutating...";
#endif
        double addNodeProb = Globals::getSingleton()->getParameterValue("MutateAddNodeProbability");
        double addLinkProb = Globals::getSingleton()->getParameterValue("MutateAddLinkProbability");
        double mutateLinkWeightsProb = Globals::getSingleton()->getParameterValue("MutateLinkWeightsProbability");
        double mutateLinkProb = Globals::getSingleton()->getParameterValue("MutateLinkProbability");
        double mutateDemolishLinkProb = Globals::getSingleton()->getParameterValue("MutateDemolishLinkProbability");
        bool finished=false;

        int adultLinkAge = int(Globals::getSingleton()->getParameterValue("AdultLinkAge"));

        if (Globals::getSingleton()->getRandom().getRandomDouble()<addNodeProb)
        {
            mutateDemolishLinkProb = true;

#if DEBUG_MUTATE
            cout << "Adding a node...";
#endif
            if (mutateAddNode())
            {
                finished=true;
                mutateDemolishLinkProb = true;
            }
            else
            {
#if DEBUG_MUTATE
                cout << "Failed! Mutating link weights instead...";
#endif
            }
        }

#if DEBUG_ONLY_ONE_MUTATION
        else
#endif
            if (Globals::getSingleton()->getRandom().getRandomDouble()<addLinkProb)
            {
#if DEBUG_MUTATE
                cout << "Adding a link...";
#endif
                if (mutateAddLink())
                {
                    finished=true;
                    mutatedAtBirth = true;
                }
                else
                {
#if DEBUG_MUTATE
                    cout << "Failed! Mutating link weights instead...";
#endif
                }
            }

#if DEBUG_ONLY_ONE_MUTATION
            if (!finished)
#endif
            {
                if (Globals::getSingleton()->getRandom().getRandomDouble()<mutateLinkWeightsProb)
                {
                    for (int a=0;a<(int)links.size();a++)
                    {
                        if ((links[a].getAge()<adultLinkAge)||Globals::getSingleton()->getRandom().getRandomDouble()<mutateLinkProb)
                        {
                            links[a].mutate();
                            mutatedAtBirth = true;

                        }
                        else if ((links[a].getAge()<adultLinkAge)||Globals::getSingleton()->getRandom().getRandomDouble()<mutateDemolishLinkProb)
                        {
                            links[a].setWeight(0);
                            mutatedAtBirth = true;
                        }
                    }
                }
            }

#if DEBUG_MUTATE
            cout << "Done\n";
#endif
    }


    void GeneticIndividual::addNode(GeneticNodeGene node)
    {
        int nodeID = node.getID();
        for (int a=0;a<(int)nodes.size();a++)
        {
            int curID = nodes[a].getID();
            if (curID==nodeID)
			{
                throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to add a node when it already existed!");
			}
            else if (curID>nodeID)
            {
                nodes.insert(nodes.begin()+a,node);
                return;
            }
        }

        nodes.push_back(node);
    }

    /*void GeneticIndividual::deleteNode(GeneticNodeGene *node)
    {
    for(int a=0;a<nodes.size();a++)
    {
    if(nodes[a]->getID()==node->getID())
    delete nodes[a];
    return;
    }

    throw "Node to be deleted not found!";
    }*/

    int GeneticIndividual::getNodesCount() const
    {
        return nodes.size();
    }

    GeneticNodeGene *GeneticIndividual::getNode(int index)
    {
        return &nodes[index];
    }

    const GeneticNodeGene *GeneticIndividual::getNode(int index) const
    {
        return &nodes[index];
    }

    GeneticNodeGene* GeneticIndividual::getNode(const string &name)
    {
        for(int a=0;a<(int)nodes.size();a++)
        {
            if(iequals(nodes[a].getName(),name))
            {
                return &nodes[a];
            }
        }

        return NULL;
    }

	const GeneticNodeGene* GeneticIndividual::getNode(const string &name) const
	{
		for(int a=0;a<(int)nodes.size();a++)
		{
			if(iequals(nodes[a].getName(),name))
			{
				return &nodes[a];
			}
		}
		
		return NULL;
	}
	
    int GeneticIndividual::getMaxNodePositionOccurance() const
    {
        vector< pair<double,int> > positionHistogram;
        int maxVal=0;
        for (int a=0;a<(int)getNodesCount();a++)
        {
            const GeneticNodeGene *node = getNode(a);
            for (int b=0;b<=(int)positionHistogram.size();b++)
            {
                if (b==(int)positionHistogram.size())
                {
                    positionHistogram.push_back(pair<double,int>(node->getDrawingPosition(),1));
                    break;
                }
                else if (fabs(positionHistogram[b].first-node->getDrawingPosition())<0.01)
                {
                    positionHistogram[b].second++;
                    maxVal = max(maxVal,positionHistogram[b].second);
                }
            }
        }

        return maxVal;
    }


    void GeneticIndividual::addLink(GeneticLinkGene link)
    {
        int linkID = link.getID();
        for (int a=0;a<(int)links.size();a++)
        {
            int curID = links[a].getID();
            if (curID==linkID)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to add a link when it already existed!");
            }

            else if (curID>linkID)
            {
                links.insert(links.begin()+a,link);
                return;
            }
        }

        links.push_back(link);
    }



    int GeneticIndividual::getLinksCount() const
    {
        return (int)links.size();
    }

    GeneticLinkGene *GeneticIndividual::getLink(int index)
    {
        return &links[index];
    }

    const GeneticLinkGene *GeneticIndividual::getLink(int index) const
    {
        return &links[index];
    }

    GeneticLinkGene *GeneticIndividual::getLink(int fromNodeID,int toNodeID)
    {
        for (int a=0;a<(int)links.size();a++)
        {
            if (links[a].getFromNodeID()==fromNodeID&&links[a].getToNodeID()==toNodeID)
                return &links[a];
        }

        throw CREATE_LOCATEDEXCEPTION_INFO("Tried to get a link which doesn't exist!");
    }

    const GeneticLinkGene *GeneticIndividual::getLink(int fromNodeID,int toNodeID) const
    {
        for (int a=0;a<(int)links.size();a++)
        {
            if (links[a].getFromNodeID()==fromNodeID&&links[a].getToNodeID()==toNodeID)
                return &links[a];
        }

        throw CREATE_LOCATEDEXCEPTION_INFO("Tried to get a link which doesn't exist!");
    }

    bool GeneticIndividual::linkExists(int fromNode,int toNode) const
    {
        for (int a=0;a<(int)links.size();a++)
        {
            if (links[a].getFromNodeID()==fromNode&&links[a].getToNodeID()==toNode)
                return true;
        }

        return false;
    }

    void GeneticIndividual::dump(TiXmlElement *root,bool dumpGenes)
    {
        root->SetDoubleAttribute("Fitness",fitness);

        root->SetAttribute("SpeciesID",speciesID);

        if (userData)
            root->SetAttribute("UserData",userData->toString());

        if (dumpGenes)
        {
            TiXmlElement *nodesElementPtr = new TiXmlElement("Nodes");

            root->LinkEndChild(nodesElementPtr);

            for (int a=0;a<(int)nodes.size();a++)
            {
                TiXmlElement *nodeElementPtr = new TiXmlElement("Node");

                nodes[a].dump(nodeElementPtr);

                nodesElementPtr->LinkEndChild(nodeElementPtr);
            }

            TiXmlElement *linksElementPtr = new TiXmlElement("Links");

            root->LinkEndChild(linksElementPtr);

            for (int a=0;a<(int)links.size();a++)
            {
                TiXmlElement *linkElementPtr = new TiXmlElement("Link");

                links[a].dump(linkElementPtr);

                linksElementPtr->LinkEndChild(linkElementPtr);
            }
        }
    }

    void GeneticIndividual::dump(ostream &ostr)
    {
        ostr << fitness << ' ' << speciesID << ' ';

        ostr << nodes.size() << ' ';

        for (int a=0;a<(int)nodes.size();a++)
        {
            nodes[a].dump(ostr);
            ostr << ' ';
        }

        ostr << links.size() << ' ';

        for (int a=0;a<(int)links.size();a++)
        {
            links[a].dump(ostr);
            ostr << ' ';
        }
    }

    void GeneticIndividual::print() const
    {
        cout << "NEW INDIVIDUAL:\n";
        for (int a=0;a<(int)links.size();a++)
        {
            cout << "LINK: " << links[a].getFromNodeID() << " " << links[a].getToNodeID() << " " << links[a].getWeight() << endl;
        }
        cout << endl;
    }

    void GeneticIndividual::printDot(ostream &ostr) const
    {
    	ostr << "digraph G {\n";
        for (int a=0;a<(int)nodes.size();a++)
        {
        	ostr <<  nodes[a].getID();
        	ostr << " [label=\""<< nodes[a].getName();
        	ostr << " ";

        	switch (nodes[a].getActivationFunction())
        	{
        	case ACTIVATION_FUNCTION_SIN:
        		ostr << "sin";
        		break;
        	case ACTIVATION_FUNCTION_SIGMOID:
        		ostr << "sig";
        		break;
        	case ACTIVATION_FUNCTION_GAUSSIAN:
        		ostr << "gau";
        		break;
        	case ACTIVATION_FUNCTION_LINEAR:
        		ostr << "lin";
        		break;
        	case ACTIVATION_FUNCTION_COS:
        		ostr << "cos";
        		break;
        	case ACTIVATION_FUNCTION_SQUARE:
        		ostr << "sqr";
        		break;
        	case ACTIVATION_FUNCTION_ABS_ROOT:
        		ostr << "root";
        		break;
        	case ACTIVATION_FUNCTION_ONES_COMPLIMENT:
        		ostr << "comp";
        		break;
        	default:
        		cout << "Warning: Unknown activation function!" << endl;
        		ostr << "unk";
        	}
        	ostr <<"\"";
        	//        		ofs << "| pf"<< network[v].get_pfparams()[0] <<"\"";
        	if (nodes[a].getType()!= "HiddenNode")
        		ostr<<" shape=doublecircle";

        	ostr <<"]\n";
        }
        for (int a=0;a<(int)links.size();a++)
        {
        	ostr << links[a].getFromNodeID()
                				 << " -> " << links[a].getToNodeID()
                				 << "[label=\"" << links[a].getWeight() << "\"]\n";
        }
        ostr << "}\n";
    }

    double GeneticIndividual::getCompatibility(shared_ptr<GeneticIndividual> other)
    {
        GeneticIndividual *ind1 = this;
        GeneticIndividual *ind2 = other.get();

        int numExcess = abs(int(ind1->getLinksCount())-int(ind2->getLinksCount()));

        int maxIndividualSize = max(int(ind1->getLinksCount()),int(ind2->getLinksCount()));

        int numDisjoint=0,numMatching=0;

        double weightDiffTotal=0;

        int link1index = 0,link2index=0;

        while (link1index<ind1->getLinksCount()&&link2index<ind2->getLinksCount())
        {
            GeneticLinkGene *link1=NULL,*link2=NULL;

            link1 = ind1->getLink(link1index);
            link2 = ind2->getLink(link2index);

            if (link2->getID()<link1->getID())
            {
                numDisjoint++;
                link2index++;
            }
            else if (link1->getID()<link2->getID())
            {
                numDisjoint++;
                link1index++;
            }
            else //both links have the same ID
            {
                weightDiffTotal += fabs(link1->getWeight()-link2->getWeight());
                numMatching++;
                link1index++;
                link2index++;
            }

        }

        //Return the compatibility number using compatibility formula
        //Note that mut_diff_total/num_matching gives the AVERAGE
        //difference between mutation_nums for any two matching Genes
        //in the Genome
        double disjointCoeff = Globals::getSingleton()->getParameterValue("DisjointCoefficient");
        double excessCoeff = Globals::getSingleton()->getParameterValue("ExcessCoefficient");
        double weightDiffCoeff = Globals::getSingleton()->getParameterValue("WeightDifferenceCoefficient");
        double fitnessCoeff = Globals::getSingleton()->getParameterValue("FitnessCoefficient");
        //This is a hack that sets N to 1.  If N is small enough, 1 works.
        maxIndividualSize=1;
        double normalizedFitnessDifference;
        normalizedFitnessDifference = (fitness/other->fitness);
        if (normalizedFitnessDifference<1.0)
        {
            normalizedFitnessDifference = 1.0/normalizedFitnessDifference;
        }
        //Normalizing for genome size
        double difference = (
            disjointCoeff*(numDisjoint/double(maxIndividualSize))+
            excessCoeff*(numExcess/double(maxIndividualSize))+
            weightDiffCoeff*(weightDiffTotal/numMatching)+
            fitnessCoeff*(normalizedFitnessDifference)
            );
        return difference;
        //Look at disjointedness and excess in the absolute (ignoring size)
        /*return (disjointCoeff*(numDisjoint/1.0)+
        excessCoeff*(numExcess/1.0)+
        weightDiffCoeff*(weightDiffTotal/numMatching));*/
    }

    bool GeneticIndividual::mutateAddLink()
    {
        GeneticNodeGene *fromNode=NULL,*toNode=NULL;

        int placeForLink = Globals::getSingleton()->getRandom().getRandomWithinRange(0,int(nodes.size())*int(nodes.size()));

        bool foundLink=false;

        bool allowRecurrentConnections
            = (
            Globals::getSingleton()->getParameterValue("AllowRecurrentConnections")>
            Globals::getSingleton()->getRandom().getRandomDouble()
            );

        bool allowSelfRecurrentConnections
            = (
            Globals::getSingleton()->getParameterValue("AllowSelfRecurrentConnections")>
            Globals::getSingleton()->getRandom().getRandomDouble()
            );

        do
        {
            int changePerLoop=0;
            for (int fromNodeIndex=0;!foundLink&&fromNodeIndex<(int)nodes.size();fromNodeIndex++)
                for (int toNodeIndex=0;!foundLink&&toNodeIndex<(int)nodes.size();toNodeIndex++)
                {
                    fromNode = &nodes[fromNodeIndex];
                    toNode = &nodes[toNodeIndex];

                    if (!fromNode->isEnabled() || !toNode->isEnabled())
                    {
                        continue;
                    }

                    if (toNode->getType()==string("NetworkSensor")||linkExists(fromNode->getID(),toNode->getID()))
                        continue;

                    if (fromNode->getDrawingPosition()>=toNode->getDrawingPosition()&&!allowRecurrentConnections)
                        continue;

                    if ((fromNode->getID()==toNode->getID())&&!allowSelfRecurrentConnections)
                        continue;

                    if(fromNode->isTopologyFrozen() && toNode->isTopologyFrozen())
                        continue;

                    if (changePerLoop==placeForLink)
                        foundLink=true;
                    changePerLoop++;
                }

                if (!foundLink&&changePerLoop==0)
                    return false; //not Possible to add a link!


                //This guarantees that if a link can be placed, we'll place it next time.
                placeForLink = placeForLink%changePerLoop;
        }
        while (!foundLink);

        addLink(GeneticLinkGene(fromNode->getID(),toNode->getID(),Globals::getSingleton()->getRandom().getRandomDouble(-3,3)));
        return true;
    }

    bool GeneticIndividual::mutateAddNode(int fromNodeID,int toNodeID)
    {
        if (fromNodeID>=0&&fromNodeID==toNodeID)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Error: Tried to create a node within a loop connection");
        }
      

        GeneticLinkGene *randomLink;

        double AllowAddNodeToRecurrentConnectionParamVal = Globals::getSingleton()->getParameterValue("AllowAddNodeToRecurrentConnection");

        bool allowAddNodeToRecurrentConnection
            = (AllowAddNodeToRecurrentConnectionParamVal> Globals::getSingleton()->getRandom().getRandomDouble());

        int chances=0;
        #if DEBUG_MUTATE
        cout << __FILE__ << ": Trying to add a node\n";
        #endif  

        int tmpFromNodeID,tmpToNodeID;

		int randomLinkIndex;

        while (true)
        {
            chances++;
            #if DEBUG_MUTATE
			//cout << "Trying to add node...\n";
            #endif

            if (chances>=200000)
            {
                #if DEBUG_MUTATE
                cout << "Blown second chance limit.  Giving up\n";
                #endif
                return false;
            }
            if (chances>=100000)
            {
				randomLinkIndex = (randomLinkIndex+1)%links.size();
                #if DEBUG_MUTATE
				cout << "Blown chance limit.  incrementing over all possible links\n";
                #endif
            }
			else
			{
				//This loop ensures that you don't get a recurrent or loop connection when you add complexity.
				randomLinkIndex = Globals::getSingleton()->getRandom().getRandomInt(int(links.size()));
			}

            if (fromNodeID>=0)
            {
                randomLink = getLink(fromNodeID,toNodeID);

                tmpFromNodeID = fromNodeID;
                tmpToNodeID = toNodeID;
            }
            else
            {
                randomLink = &links[randomLinkIndex];

                tmpFromNodeID = randomLink->getFromNodeID();
                tmpToNodeID = randomLink->getToNodeID();
				
				//cout << "IDs: " << tmpFromNodeID << " , " << tmpToNodeID << endl;
            }

            if (tmpFromNodeID==tmpToNodeID)
            {
                //loop link, try again
                #if DEBUG_MUTATE
				cout << "Loop link, ignore\n";
                #endif
                continue;
            }

            GeneticNodeGene *fromNode=NULL,*toNode=NULL;

            for (int a=0;a<(int)nodes.size();a++)
            {
                //This loop goes through and gets the sum of the two nodes' drawing positions for averaging.
                if (nodes[a].getID()==tmpFromNodeID)
                    fromNode = &nodes[a];

                if (nodes[a].getID()==tmpToNodeID)
                    toNode = &nodes[a];
            }

            if (!fromNode->isEnabled() || !toNode->isEnabled())
            {
                //Disabled nodes, don't add anything.
                #if DEBUG_MUTATE
				cout << "One of the nodes is disabled...\n";
                #endif
                continue;
            }
            
            if (fromNode->getDrawingPosition()>=toNode->getDrawingPosition())
            {
                //Recurrent connection.
                #if DEBUG_MUTATE
				cout << "Recurrent connection.  Ignoring\n";
                #endif
                if (!allowAddNodeToRecurrentConnection)
                    continue;
            }
  
            if(fromNode->isTopologyFrozen() && toNode->isTopologyFrozen())
            {
                #if DEBUG_MUTATE
				cout << "Topology frozen on both nodes.  Ignoring\n";
                #endif
                //Don't add links between two frozen topology nodes
                continue;
            }

            double newPosition = (fromNode->getDrawingPosition()+toNode->getDrawingPosition())/2.0;

            randomLink->setAge(0);
            randomLink->setWeight(randomLink->getWeight()/2.0);

            bool randomActivation=false;
            
            double ExtraActivationFunctions = Globals::getSingleton()->getParameterValue("ExtraActivationFunctions");

            if (ExtraActivationFunctions>Globals::getSingleton()->getRandom().getRandomDouble())
                randomActivation=true;

            GeneticNodeGene newNode = GeneticNodeGene("","HiddenNode",newPosition,randomActivation);

            GeneticLinkGene sourceLink = GeneticLinkGene(randomLink->getFromNodeID(),newNode.getID(),1.0);
            GeneticLinkGene destLink = GeneticLinkGene(newNode.getID(),randomLink->getToNodeID(),randomLink->getWeight()/2.0);

            addNode(newNode);
            addLink(sourceLink);
            addLink(destLink);
            if (Globals::getSingleton()->getParameterValue("AddBiasToHiddenNodes")>Globals::getSingleton()->getRandom().getRandomDouble())
            {
                int biasNodeID=-1;
                for (int a=0;a<(int)nodes.size();a++)
                {
                    if (nodes[a].getName()==string("Bias"))
                    {
                        biasNodeID=(int)a;
                        break;
                    }
                }

                if (biasNodeID==-1)
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("Error, tried to add link from bias node when biad node didn't exist!");
                }

                if (randomLink->getFromNodeID()==biasNodeID)
                {
                    //Oops, the from link was already the bias node, don't bother
                }
                else
                {
                    GeneticLinkGene biasLink = GeneticLinkGene(biasNodeID,newNode.getID(),0.0);
                    addLink(biasLink);
                }
            }
            return true;
        }
        return false;
    }

    void GeneticIndividual::incrementAge()
    {
        for (int a=0;a<(int)nodes.size();a++)
        {
            nodes[a].incrementAge();
        }
        for (int a=0;a<(int)links.size();a++)
        {
            links[a].incrementAge();
        }
    }


    ModularNetwork *GeneticIndividual::spawnPhenotype() const
    {
        vector<NetworkNode *> networkNodes;
        for (int a=0;a<(int)nodes.size();a++)
        {
            if (!nodes[a].isEnabled())
                continue;

            if (nodes[a].getType()==string("NetworkSensor"))
                networkNodes.push_back(new NetworkNode(nodes[a].getName(),false,nodes[a].getActivationFunction()));
            else
                networkNodes.push_back(new NetworkNode(nodes[a].getName(),true,nodes[a].getActivationFunction()));
        }

        double linkGeneMinimumWeightForPhentoype =
            Globals::getSingleton()->getParameterValue("LinkGeneMinimumWeightForPhentoype");

        vector<NetworkLink *> networkLinks;
        for (int a=0;a<(int)links.size();a++)
        {
            const GeneticLinkGene *linkGene = &links[a];

            if (!linkGene->isEnabled()||( fabs(linkGene->getWeight())<linkGeneMinimumWeightForPhentoype ))
                continue;


            NetworkNode *fromNode=NULL;
            NetworkNode *toNode=NULL;

            int fromNodeIndex=-1,toNodeIndex=-1;

            bool create=true;

            for (int b=0;b<(int)nodes.size();b++)
            {
                //this holds because there's a match between genotype indicies and phenotype indicies
                if (nodes[b].getID()==links[a].getFromNodeID())
                {
                    fromNode = networkNodes[b];
                    fromNodeIndex = (int)b;
                }

                if (nodes[b].getID()==links[a].getToNodeID())
                {
                    toNode = networkNodes[b];
                    toNodeIndex = (int)b;
                }

                if (fromNode!=NULL&&toNode!=NULL)
                    break;

                if ((b+1) == (int)nodes.size()) //couldn't find a nodeID.  Node must have been disabled
                {
                    create=false;
                    break;
                }

            }

            if (fromNodeIndex==-1 || toNodeIndex==-1)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR!");
            }

            if (create)
                networkLinks.push_back(
                new NetworkLink(
                fromNode,
                toNode,
                nodes[fromNodeIndex].getDrawingPosition()<nodes[toNodeIndex].getDrawingPosition(),
                links[a].getWeight()
                )
                );
        }

        ModularNetwork *network = new ModularNetwork(networkNodes,networkLinks);

        while (!networkNodes.empty())
        {
            delete networkNodes[0];
            networkNodes.erase(networkNodes.begin());
        }

        while (!networkLinks.empty())
        {
            delete networkLinks[0];
            networkLinks.erase(networkLinks.begin());
        }

        return network;
    }

    ModularNetwork GeneticIndividual::spawnPhenotypeStack() const
    {
        vector<NetworkNode *> networkNodes;
        for (int a=0;a<(int)nodes.size();a++)
        {
            if (!nodes[a].isEnabled())
                continue;

            NetworkNode *networkNode;
            if (nodes[a].getType()==string("NetworkSensor"))
                networkNode = new NetworkNode(nodes[a].getName(),false,nodes[a].getActivationFunction());
            else
                networkNode = new NetworkNode(nodes[a].getName(),true,nodes[a].getActivationFunction());
            networkNodes.push_back(networkNode);
        }

        double linkGeneMinimumWeightForPhentoype =
            Globals::getSingleton()->getParameterValue("LinkGeneMinimumWeightForPhentoype");

        vector<NetworkLink *> networkLinks;
        for (int a=0;a<(int)links.size();a++)
        {
            const GeneticLinkGene *linkGene = &links[a];

            if (!linkGene->isEnabled()||( fabs(linkGene->getWeight())<linkGeneMinimumWeightForPhentoype ))
                continue;


            NetworkNode *fromNode=NULL;
            NetworkNode *toNode=NULL;

            bool create=true;

            int fromNodeIndex=-1,toNodeIndex=-1;

            for (int b=0;b<(int)nodes.size();b++)
            {
                //this holds because there's a match between genotype indicies and phenotype indicies
                if (nodes[b].getID()==links[a].getFromNodeID())
                {
                    fromNode = networkNodes[b];
                    fromNodeIndex = (int)b;
                }

                if (nodes[b].getID()==links[a].getToNodeID())
                {
                    toNode = networkNodes[b];
                    toNodeIndex = (int)b;
                }

                if (fromNode!=NULL&&toNode!=NULL)
                    break;

                if ((b+1) == (int)nodes.size()) //couldn't find a nodeID.  Node must have been disabled
                {
                    create=false;
                    break;
                }

            }

            if (fromNodeIndex==-1 || toNodeIndex==-1)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR!");
            }

            if (create)
                networkLinks.push_back(
                new NetworkLink(
                fromNode,
                toNode,
                nodes[fromNodeIndex].getDrawingPosition()<nodes[toNodeIndex].getDrawingPosition(),
                links[a].getWeight()
                )
                );
        }

        ModularNetwork network(networkNodes,networkLinks);

        while (!networkNodes.empty())
        {
            delete networkNodes[0];
            networkNodes.erase(networkNodes.begin());
        }

        while (!networkLinks.empty())
        {
            delete networkLinks[0];
            networkLinks.erase(networkLinks.begin());
        }

        return network;
    }

    ostream& operator<<(ostream& stream, const GeneticIndividual& gi)
    {
        return stream;
    }

    istream& operator>>(istream& stream, GeneticIndividual& gi)
    {
        return stream;
    }

}
