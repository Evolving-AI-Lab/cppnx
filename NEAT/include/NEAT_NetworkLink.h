#ifndef __NETWORKLINK_H__
#define __NETWORKLINK_H__

#include "NEAT_Defines.h"
#include "NEAT_STL.h"
#include "tinyxmldll.h"

namespace NEAT
{
    class NetworkLink
    {
    protected:
        NetworkNode *fromNode,*toNode;

        bool forward;

    public:
        double weight;

        //GeneticLinkGene *genotype;

        NEAT_DLL_EXPORT NetworkLink(NetworkNode *_fromNode,NetworkNode *_toNode,bool _forward,double _weight);

        NEAT_DLL_EXPORT NetworkLink(NetworkNode *_fromNode,NetworkNode *_toNode,double _weight);

        NEAT_DLL_EXPORT NetworkLink()
        {}

        NEAT_DLL_EXPORT virtual ~NetworkLink()
        {}

        inline NetworkNode* getFromNode() const
        {
            return fromNode;
        }

        inline NetworkNode* getToNode() const
        {
            return toNode;
        }

        inline bool isForward() const
        {
            return forward;
        }

        inline const double &getWeight() const
        {
            return weight;
        }

        inline void setWeight(double _weight)
        {
            weight = _weight;
        }

    protected:
        //Do not use these because of pointer copy

        NetworkLink(const NetworkLink &other)
        {
        }

        const NetworkLink &operator=(const NetworkLink &other)
        {
            return *this;
        }
    };

}

#endif
