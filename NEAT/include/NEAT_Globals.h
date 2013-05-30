#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "NEAT_Defines.h"
#include "NEAT_STL.h"
#include "NEAT_Random.h"
#include "tinyxmldll.h"

/* #defines */
#define LAST_GENERATION  (-1)
#define PRINT(var) if(true) cout << "  " << #var << ": " << var << endl

#define GET_PARAMETER(_name) (NEAT::Globals::getSingleton()->getParameterValue(_name))
#define HAS_PARAMETER(_name) (NEAT::Globals::getSingleton()->hasParameterValue(_name))
#define SET_PARAMETER(_name,_val) (NEAT::Globals::getSingleton()->setParameterValue(_name,_val))

enum ActivationFunction
{
    ACTIVATION_FUNCTION_SIGMOID = 0,
    ACTIVATION_FUNCTION_SIN,             //1
    ACTIVATION_FUNCTION_COS,             //2
    ACTIVATION_FUNCTION_GAUSSIAN,        //3
    ACTIVATION_FUNCTION_SQUARE,          //4
    ACTIVATION_FUNCTION_ABS_ROOT,        //5
    ACTIVATION_FUNCTION_LINEAR,          //6
    ACTIVATION_FUNCTION_ONES_COMPLIMENT, //7
//	ACTIVATION_FUNCTION_ZERO,			//8
    ACTIVATION_FUNCTION_END              //9
};

extern const char *activationFunctionNames[ACTIVATION_FUNCTION_END];

namespace NEAT
{
    class Globals
    {
    protected:
        NEAT_DLL_EXPORT static Globals *singleton;

        int nodeCounter,linkCounter,speciesCounter;

		vector<shared_ptr<GeneticNodeGene> > nodeGenesThisGeneration;
        vector<shared_ptr<GeneticLinkGene> > linkGenesThisGeneration;

		StackMap<string,double,4096> parameters;

        Random random;

		int extraActivationUpdates;

		bool signedActivation;

		bool useTanhSigmoid;
    public:
        static inline Globals *getSingleton()
        {
            if (!singleton)
                throw CREATE_LOCATEDEXCEPTION_INFO("You didn't initialize Globals before using it!");

            return singleton;
        }

        static inline Globals *init()
        {
            if (singleton)
                delete singleton;

            singleton = new Globals();

            return singleton;
        }

        static inline Globals *init(string filename)
        {
            if (singleton)
                delete singleton;

            singleton = new Globals(filename);

            return singleton;
        }

        static inline Globals *init(TiXmlElement *root)
        {
            if (singleton)
                delete singleton;

            singleton = new Globals(root);

            return singleton;
        }

        static inline void deinit()
        {
            delete singleton;
        }

        NEAT_DLL_EXPORT void assignNodeID(GeneticNodeGene *testNode);

        NEAT_DLL_EXPORT void assignLinkID(GeneticLinkGene *testLink,bool ignoreHistory=false);
		
		NEAT_DLL_EXPORT void clearNodeHistory();

        NEAT_DLL_EXPORT void clearLinkHistory();

        NEAT_DLL_EXPORT int generateSpeciesID();

        NEAT_DLL_EXPORT void addParameter(string name,double value);

        NEAT_DLL_EXPORT bool hasParameterValue(const string &name);

		NEAT_DLL_EXPORT double getParameterValue(const char *cname);

        NEAT_DLL_EXPORT double getParameterValue(string name);

        NEAT_DLL_EXPORT void setParameterValue(string name,double value);
		
		NEAT_DLL_EXPORT void overrideParametersFromFile(string fileName);

        inline StackMap<string,double,4096>::iterator getMapBegin()
        {
            return parameters.begin();
        }

        inline StackMap<string,double,4096>::iterator getMapEnd()
        {
            return parameters.end();
        }

        NEAT_DLL_EXPORT void initRandom();

        inline Random &getRandom()
        {
            return random;
        }

        NEAT_DLL_EXPORT void seedRandom(unsigned int newSeed);

        NEAT_DLL_EXPORT void dump(TiXmlElement *root);

		inline int hasExtraActivationUpdates()
		{
			return extraActivationUpdates;
		}

		inline bool hasSignedActivation()
		{
			return signedActivation;
		}

		inline bool isUsingTanhSigmoid()
		{
			return useTanhSigmoid;
		}

    protected:
        NEAT_DLL_EXPORT Globals();

        NEAT_DLL_EXPORT Globals(TiXmlElement *root);

        NEAT_DLL_EXPORT Globals(string fileName);

        NEAT_DLL_EXPORT virtual ~Globals();

        int generateNodeID();

        int generateLinkID();

		void cacheParameters();
    };

}

#endif
