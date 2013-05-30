#ifndef NEAT_DEFINES_H_INCLUDED
#define NEAT_DEFINES_H_INCLUDED

#define EPLEX_INTERNAL

#ifdef _MSC_VER
#pragma warning (disable : 4251)
#pragma warning (disable : 4996)
#endif

#include "tinyxmldll.h"

#include "NEAT_STL.h"

#include "JGTL_LocatedException.h"
#include "JGTL_Vector2.h"
#include "JGTL_StringConverter.h"
#include "JGTL_StackMap.h"
using namespace JGTL;

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost;

#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
//#define _INC_MALLOC      // exclude standard memory alloc procedures
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

#if defined( __CYGWIN__ )
#  ifdef BUILD_NEAT_DLL
#    define NEAT_DLL_EXPORT
#  else
#    define NEAT_DLL_EXPORT
#  endif
#elif defined( _WIN32 )
#  ifdef BUILD_NEAT_DLL
#    define NEAT_DLL_EXPORT __declspec( dllexport )
#  else
#    define NEAT_DLL_EXPORT __declspec( dllimport )
#  endif
#else
#  ifdef BUILD_NEAT_DLL
#    define NEAT_DLL_EXPORT
#  else
#    define NEAT_DLL_EXPORT
#  endif
#endif

#define EPLEX_INTERNAL



namespace NEAT
{
    /** typedefs **/
    typedef unsigned int uint;
    typedef unsigned long long ulong;
    typedef unsigned char uchar;
    typedef unsigned short ushort;

	class NetworkOutputNode;
    class NetworkNode;
    class NetworkLink;

    class GeneticIndividual;
    class GeneticGeneration;

    class GeneticLinkGene;
    class GeneticNodeGene;

    class Stringable
    {
    public:
        virtual ~Stringable()
        {}

        virtual string toString() const = 0;

        virtual string toMultiLineString() const
        {
            return toString();
        }

        virtual string summaryHeaderToString() const
        {
            return "";
        }

        virtual string summaryToString() const
        {
            return "";
        }

        virtual Stringable *clone() const = 0;
    };

    class FitnessStringable : public Stringable
    {
    protected:
        double fitness;

    public:
        FitnessStringable()
                :
                fitness(0)
        {
        }

        FitnessStringable(double _fitness)
                :
                fitness(_fitness)
        {
        }

        virtual ~FitnessStringable()
        {}

        virtual string toString() const
        {
			return JGTL::toString(fitness);
        }

        virtual string toMultiLineString() const
        {
            return toString();
        }

        virtual string summaryHeaderToString() const
        {
            return "Fitness";
        }

        virtual string summaryToString() const
        {
            return "(Fitness Summary)";
        }

        virtual Stringable *clone() const
        {
            return new FitnessStringable(fitness);
        }
    };
}

#endif // NEAT_DEFINES_H_INCLUDED
