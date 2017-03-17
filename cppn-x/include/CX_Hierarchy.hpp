/*
 * CX_Hierarchy.hpp
 *
 *  Created on: Mar 30, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_HIERARCHY_HPP_
#define CPPN_X_INCLUDE_CX_HIERARCHY_HPP_

//Standard includes
#include <set>
#include <algorithm>

//Local includes
#include "CE_Cppn.h"
#include "CX_Debug.hpp"

class forwardReachableNodeCount{
public:
    forwardReachableNodeCount(Cppn* cppn = 0):_cppn(cppn){

    }

    void setCppn(Cppn* cppn){
        _cppn = cppn;
    }

    double lrc(size_t start){
        std::vector<Node*> frontier;
        std::set<Node*> visited;
        frontier.push_back(_cppn->getNode(start));
        dbg::out(dbg::info, "hierarchy") << "From: " << start <<
                " reachable " << _cppn->getNode(start)->outgoingEdges().size() << std::endl;

        while(!frontier.empty()){
            Node* currentNode = frontier.back();
            frontier.pop_back();
            if(visited.count(currentNode) > 0){
                continue;
            }
            visited.insert(currentNode);
            foreach(Edge* edge, currentNode->outgoingEdges()){
                frontier.push_back(edge->destNode());
            }
        }
        return double(visited.size()-1) / double(_cppn->getNrOfNodes()-1);
    }

private:
    Cppn* _cppn;
};

class backwardReachableNodeCount{
public:
    backwardReachableNodeCount(Cppn* cppn = 0):_cppn(cppn){

    }

    void setCppn(Cppn* cppn){
        _cppn = cppn;
    }

    double lrc(size_t start){
        std::vector<Node*> frontier;
        std::set<Node*> visited;
        frontier.push_back(_cppn->getNode(start));
        dbg::out(dbg::info, "hierarchy") << "From: " << start <<
                " reachable " << _cppn->getNode(start)->incomingEdges().size() << std::endl;
        while(!frontier.empty()){
            Node* currentNode = frontier.back();
            frontier.pop_back();
            if(visited.count(currentNode) > 0){
                continue;
            }
            visited.insert(currentNode);

            foreach(Edge* edge, currentNode->incomingEdges()){
                frontier.push_back(edge->sourceNode());
            }
        }
        return double(visited.size()-1) / double(_cppn->getNrOfNodes()-1);
    }

private:
    Cppn* _cppn;
};


template<typename LRC>
double getGlobalReachingCentrality(Cppn& cppn, LRC lrcCalculator){
    std::vector<double> localReachingCentrality;
    lrcCalculator.setCppn(&cppn);
    for(size_t i=0; i<cppn.getNrOfNodes(); ++i){
        localReachingCentrality.push_back(lrcCalculator.lrc(i));
        dbg::out(dbg::info, "hierarchy") << " Node: " << i << " lrc: " << localReachingCentrality.back() << std::endl;
    }
    double maxLrc = *std::max_element(localReachingCentrality.begin(), localReachingCentrality.end());
    dbg::out(dbg::info, "hierarchy") << "Maximum lrc: " << maxLrc << std::endl;

    double globalReachingCentrality = 0.0;
    for(size_t i=0; i<localReachingCentrality.size(); ++i){
        globalReachingCentrality += maxLrc - localReachingCentrality[i];
    }

    globalReachingCentrality /= double(cppn.getNrOfNodes()-1);
    dbg::out(dbg::info, "hierarchy") << "Grc: " << globalReachingCentrality << std::endl;

    return globalReachingCentrality;
}




#endif /* CPPN_X_INCLUDE_CX_HIERARCHY_HPP_ */
