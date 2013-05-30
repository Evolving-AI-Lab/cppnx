#include "NEAT_GeneticPopulation.h"
#include "NEAT_GeneticGeneration.h"
#include "NEAT_GeneticIndividual.h"
#include "NEAT_Random.h"

//#define DEBUG_GENPOP

namespace NEAT
{

    GeneticPopulation::GeneticPopulation()
            : onGeneration(0)
    {

        {
            generations.push_back(
                shared_ptr<GeneticGeneration>(
                    new GeneticGeneration(0)
                )
            );
        }
    }

    GeneticPopulation::GeneticPopulation(string fileName)
            : onGeneration(-1)
    {
        TiXmlDocument doc(fileName);

        bool loadStatus;

        if (iends_with(fileName,".gz"))
        {
            loadStatus = doc.LoadFileGZ();
        }
        else
        {
            loadStatus = doc.LoadFile();
        }

        if (!loadStatus)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Error trying to load the XML file!");
        }

        TiXmlElement *root;

        root = doc.FirstChildElement();

        {
            TiXmlElement *generationElement = root->FirstChildElement("GeneticGeneration");

            while (generationElement)
            {
                generations.push_back(shared_ptr<GeneticGeneration>(new GeneticGeneration(generationElement)));

                generationElement = generationElement->NextSiblingElement("GeneticGeneration");
                onGeneration++;
            }
        }

        if (onGeneration<0)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Tried to load a population with no generations!");
        }

        adjustFitness();
    }

    GeneticPopulation::~GeneticPopulation()
    {
        while (!species.empty())
            species.erase(species.begin());

        while (!extinctSpecies.empty())
            extinctSpecies.erase(extinctSpecies.begin());
    }

    int GeneticPopulation::getIndividualCount(int generation)
    {
        if (generation==-1)
					generation=int(generations.size()-1);//onGeneration);

        return generations[generation]->getIndividualCount();
    }

    void GeneticPopulation::addIndividual(shared_ptr<GeneticIndividual> individual)
    {
        generations[generations.size()-1/*onGeneration*/]->addIndividual(individual);
    }

    shared_ptr<GeneticIndividual> GeneticPopulation::getIndividual(int a,int generation)
    {
#ifdef DEBUG_GENPOP
        cout << a << ',' << generations.size() << endl;
#endif
        if (generation==-1)
        {
#ifdef DEBUG_GENPOP
            cout << "NO GEN GIVEN: USING onGeneration\n";
#endif
					generation=int(generations.size()-1);//onGeneration);
        }

        if (generation>=int(generations.size())||a>=generations[generation]->getIndividualCount())
        {
            cout << "GET_INDIVIDUAL: GENERATION OUT OF RANGE!\n";
            throw CREATE_LOCATEDEXCEPTION_INFO("GET_INDIVIDUAL: GENERATION OUT OF RANGE!\n");
        }
#ifdef DEBUG_GENPOP
		cout<<"return"<<endl;
#endif
        return generations[generation]->getIndividual(a);
    }

    vector<shared_ptr<GeneticIndividual> >::iterator GeneticPopulation::getIndividualIterator(int a,int generation)
    {
        if (generation==-1)
					generation=int(generations.size()-1);//onGeneration);

        if (generation>=int(generations.size())||a>=generations[generation]->getIndividualCount())
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Generation out of range!\n");
        }

        return generations[generation]->getIndividualIterator(a);
    }

    shared_ptr<GeneticIndividual> GeneticPopulation::getBestAllTimeIndividual()
    {
        shared_ptr<GeneticIndividual> bestIndividual;

        for (int a=0;a<(int)generations.size();a++)
        {
            for (int b=0;b<generations[a]->getIndividualCount();b++)
            {
                shared_ptr<GeneticIndividual> individual = generations[a]->getIndividual(b);
                if (bestIndividual==NULL||bestIndividual->getFitness()<=individual->getFitness())
                    bestIndividual = individual;
            }
        }

        return bestIndividual;
    }

    shared_ptr<GeneticIndividual> GeneticPopulation::getBestIndividualOfGeneration(int generation)
    {
        shared_ptr<GeneticIndividual> bestIndividual;

        if (generation==-1)
            generation = int(generations.size())-1;

        for (int b=0;b<generations[generation]->getIndividualCount();b++)
        {
            shared_ptr<GeneticIndividual> individual = generations[generation]->getIndividual(b);
            if (bestIndividual==NULL||bestIndividual->getFitness()<individual->getFitness())
                bestIndividual = individual;
        }

        return bestIndividual;
    }

    void GeneticPopulation::speciate()
    {
        double compatThreshold = Globals::getSingleton()->getParameterValue("CompatibilityThreshold");

        for (int a=0;a<generations[generations.size()-1/*onGeneration*/]->getIndividualCount();a++)
        {
            shared_ptr<GeneticIndividual> individual = generations[generations.size()-1/*onGeneration*/]->getIndividual(a);

            bool makeNewSpecies=true;

            for (int b=0;b<(int)species.size();b++)
            {
                double compatibility = species[b]->getBestIndividual()->getCompatibility(individual);
                if (compatibility<compatThreshold)
                {
                    //Found a compatible species
                    individual->setSpeciesID(species[b]->getID());
                    makeNewSpecies=false;
                    break;
                }
            }

            if (makeNewSpecies)
            {
                //Make a new species.  The process of making a new speceis sets the ID for the individual.
                shared_ptr<GeneticSpecies> newSpecies(new GeneticSpecies(individual));
                species.push_back(newSpecies);
            }
        }

        int speciesTarget = int(Globals::getSingleton()->getParameterValue("SpeciesSizeTarget"));

        double compatMod;
        double CompatibilityModifierParamVal = Globals::getSingleton()->getParameterValue("CompatibilityModifier");
        
        if ((int)species.size()<speciesTarget)
        {
            compatMod = -CompatibilityModifierParamVal;
        }
        else if ((int)species.size()>speciesTarget)
        {
            compatMod = +CompatibilityModifierParamVal;
        }
        else
        {
            compatMod=0.0;
        }

        if (compatThreshold<(fabs(compatMod)+0.3)&&compatMod<0.0)
        {
            //This is to keep the compatibility threshold from going ridiculusly small.
            if (compatThreshold<0.001)
                compatThreshold = 0.001;

            compatThreshold/=2.0;
        }
        else if (compatThreshold<(compatMod+0.3))
        {
            compatThreshold*=2.0;
        }
        else
        {
            compatThreshold+=compatMod;
        }

        Globals::getSingleton()->setParameterValue("CompatibilityThreshold",compatThreshold);
    }

    void GeneticPopulation::setSpeciesMultipliers()
    {}

    void GeneticPopulation::adjustFitness()
    {
        speciate();

        for (int a=0;a<(int)species.size();a++)
        {
            species[a]->resetIndividuals();
        }

        for (int a=0;a<generations[generations.size()-1/*onGeneration*/]->getIndividualCount();a++)
        {
            shared_ptr<GeneticIndividual> individual = generations[generations.size()-1/*onGeneration*/]->getIndividual(a);

            getSpecies(individual->getSpeciesID())->addIndividual(individual);
        }

        for (int a=0;a<(int)species.size();a++)
        {
            if (species[a]->getIndividualCount()==0)
            {
//                extinctSpecies.push_back(species[a]);
                species.erase(species.begin()+a);
                a--;
            }
        }

        for (int a=0;a<(int)species.size();a++)
        {
            species[a]->setMultiplier();
            species[a]->setFitness();
            species[a]->incrementAge();
        }

        //This function sorts the individuals by fitness
        generations[generations.size()-1/*onGeneration*/]->sortByFitness();
    }

    void GeneticPopulation::produceNextGeneration()
    {


        cout << "In produce next generation loop...\n";
        //This clears the link history so future links with the same toNode and fromNode will have different IDs
		Globals::getSingleton()->clearNodeHistory();
        Globals::getSingleton()->clearLinkHistory();

        int numParents = int(generations[generations.size()-1/*onGeneration*/]->getIndividualCount());

        for(int a=0;a<numParents;a++)
        {
			PRINT(generations[generations.size()-1/*onGeneration*/]->getIndividual(a)->getFitness());
            if(generations[generations.size()-1/*onGeneration*/]->getIndividual(a)->getFitness() < 1e-6)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Fitness must be a positive number!\n");
            }
        }

        double totalFitness=0;

        for (int a=0;a<(int)species.size();a++)
        {
            totalFitness += species[a]->getAdjustedFitness();
        }
        int totalOffspring=0;
        for (int a=0;a<(int)species.size();a++)
        {
            double adjustedFitness = species[a]->getAdjustedFitness();
            int offspring = int(adjustedFitness/totalFitness*numParents);
            totalOffspring+=offspring;
            species[a]->setOffspringCount(offspring);
        }
        //cout << "Pausing\n";
		//int result = system("PAUSE");
		//(void) result;

        //Some offspring were truncated.  Give these to the best individuals
        while (totalOffspring<numParents)
        {
            for (int a=0;totalOffspring<numParents&&a<generations[generations.size()-1/*onGeneration*/]->getIndividualCount();a++)
            {
                shared_ptr<GeneticIndividual> ind = generations[generations.size()-1/*onGeneration*/]->getIndividual(a);
                shared_ptr<GeneticSpecies> gs = getSpecies(ind->getSpeciesID());
                gs->setOffspringCount(gs->getOffspringCount()+1);
                totalOffspring++;

                /*
                //Try to give 2 offspring to the very best individual if it only has one offspring.
                //This fixes the problem where the best indiviudal sits in his own species
                //and duplicates every generation.
                if(a==0&&gs->getOffspringCount()==1&&totalOffspring<numParents)
                {
                gs->setOffspringCount(gs->getOffspringCount()+1);
                totalOffspring++;
                }*/

            }
        }
        for (int a=0;a<(int)species.size();a++)
        {
            cout << "Species ID: " << species[a]->getID() << " Age: " << species[a]->getAge() << " last improv. age: " << species[a]->getAgeOfLastImprovement() << " Fitness: " << species[a]->getFitness() << "*" << species[a]->getMultiplier() << "=" << species[a]->getAdjustedFitness() <<  " Size: " << int(species[a]->getIndividualCount()) << " Offspring: " << int(species[a]->getOffspringCount()) << endl;
        }

        //This is the new generation
        vector<shared_ptr<GeneticIndividual> > babies;

        double totalIndividualFitness=0;

        for (int a=0;a<(int)species.size();a++)
        {
            species[a]->setReproduced(false);
        }

        int smallestSpeciesSizeWithElitism
        = int(Globals::getSingleton()->getParameterValue("SmallestSpeciesSizeWithElitism"));
        static double mutateSpeciesChampionProbability
        = Globals::getSingleton()->getParameterValue("MutateSpeciesChampionProbability");

        double ForceCopyGenerationChampionParamVal = Globals::getSingleton()->getParameterValue("ForceCopyGenerationChampion");
        bool forceCopyGenerationChampion = ( ForceCopyGenerationChampionParamVal> Globals::getSingleton()->getRandom().getRandomDouble());

        for (int a=0;a<generations[generations.size()-1/*onGeneration*/]->getIndividualCount();a++)
        {
            //Go through and add the species champions
            shared_ptr<GeneticIndividual> ind = generations[generations.size()-1/*onGeneration*/]->getIndividual(a);
            shared_ptr<GeneticSpecies> species = getSpecies(ind->getSpeciesID());
            if (!species->isReproduced())
            {
                species->setReproduced(true);
                //This is the first and best organism of this species to be added, so it's the species champion
                //of this generation
                if (ind->getFitness()>species->getBestIndividual()->getFitness())
                {
                    //We have a new all-time species champion!
                    species->setBestIndividual(ind);
                    cout << "Species " << species->getID() << " has a new champ with fitness " << species->getBestIndividual()->getFitness() << endl;
                }

                if ((a==0&&forceCopyGenerationChampion)||(species->getOffspringCount()>=smallestSpeciesSizeWithElitism))
                {
                    //Copy species champion.
                    bool mutateChampion;
                    if (Globals::getSingleton()->getRandom().getRandomDouble()<mutateSpeciesChampionProbability)
                        mutateChampion = true;
                    else
                        mutateChampion = false;
                    babies.push_back(shared_ptr<GeneticIndividual>(new GeneticIndividual(ind,mutateChampion)));
                    species->decrementOffspringCount();
                }

                if (a==0)
                {
                    species->updateAgeOfLastImprovement();
                }
            }
            totalIndividualFitness+=ind->getFitness();
        }
        double averageFitness = totalIndividualFitness/generations[generations.size()-1/*onGeneration*/]->getIndividualCount();
        cout<<"Generation "<<int(onGeneration)<<": "<<"overall_average = "<<averageFitness<<endl;
        
        #ifdef PRINT_GENETIC_PERTUBATION_INFO
        //print the gen number to this file
        ofstream mutationEffects_file;        
        mutationEffects_file.open ("mutationEffects.txt", ios::app );
        mutationEffects_file << "\nGeneration "<<int(onGeneration+1)<<": " << endl; //plus 1 cause we print this line to the file before this gens data
        mutationEffects_file.close();

        ofstream mutationAndCrossoverEffects_file;        
        mutationAndCrossoverEffects_file.open ("mutationAndCrossoverEffects.txt", ios::app );
        mutationAndCrossoverEffects_file << "\nGeneration "<<int(onGeneration+1)<<": " << endl; //plus 1 cause we print this line to the file before this gens data
        mutationAndCrossoverEffects_file.close();

        ofstream crossoverEffects_file;        
        crossoverEffects_file.open ("crossoverEffects.txt", ios::app );
        crossoverEffects_file << "\nGeneration "<<int(onGeneration+1)<<": " << endl; //plus 1 cause we print this line to the file before this gens data
        crossoverEffects_file.close();
        #endif

        
        cout << "Champion fitness: " << generations[generations.size()-1/*onGeneration*/]->getIndividual(0)->getFitness() << endl;

        if (generations[generations.size()-1/*onGeneration*/]->getIndividual(0)->getUserData())
        {
            cout << "Champion data: " << generations[generations.size()-1/*onGeneration*/]->getIndividual(0)->getUserData()->summaryToString() << endl;
        }
        cout << "# of Species: " << int(species.size()) << endl;
        cout << "compat threshold: " << Globals::getSingleton()->getParameterValue("CompatibilityThreshold") << endl;

        for (int a=0;a<(int)species.size();a++)
        {
            //cout << "jmc: Making babies for species" << a << endl;
            species[a]->makeBabies(babies);
        }
        //cout << "jmc: done making babies\n";
        if ((int)babies.size()!=generations[generations.size()-1/*onGeneration*/]->getIndividualCount())
        {
            cout << "Population size changed!\n";
            throw CREATE_LOCATEDEXCEPTION_INFO("Population size changed!");
        }

        cout << "Making new generation\n";
        shared_ptr<GeneticGeneration> newGeneration(generations[generations.size()-1/*onGeneration*/]->produceNextGeneration(babies,onGeneration+1));
        //cout << "Done Making new generation!\n";

        /*for(int a=0;a<4;a++)
        {
        for(int b=0;b<4;b++)
        {
        cout << babies[a]->getCompatibility(babies[b]) << '\t';
        }

        cout << endl;
        }*/

        generations.push_back(newGeneration);
        onGeneration++;
    }




    void GeneticPopulation::dump(string filename,bool includeGenes,bool doGZ)
    {
        TiXmlDocument doc( filename );

        TiXmlElement *root = new TiXmlElement("Genetics");

        Globals::getSingleton()->dump(root);

        doc.LinkEndChild(root);

        for (int a=0;a<(int)generations.size();a++)
        {

            TiXmlElement *generationElementPtr = new TiXmlElement(generations[a]->getTypeName());

            root->LinkEndChild(generationElementPtr);

            generations[a]->dump(generationElementPtr,includeGenes);
        }

        if (doGZ)
        {
            doc.SaveFileGZ();
        }
        else
        {
            doc.SaveFile();
        }
    }

    void GeneticPopulation::dumpBest(string filename,bool includeGenes,bool doGZ)
    {
        TiXmlDocument doc( filename );

        TiXmlElement *root = new TiXmlElement("Genetics");

        Globals::getSingleton()->dump(root);

        doc.LinkEndChild(root);

        if (!NEAT::Globals::getSingleton()->getParameterValue("OnlySaveFinalPop")){
            for (int a=0;a<int(generations.size())-1;a++)
            {

                TiXmlElement *generationElementPtr = new TiXmlElement(generations[a]->getTypeName());

                root->LinkEndChild(generationElementPtr);

                generations[a]->dumpBest(generationElementPtr,includeGenes);
            }
        }
        if (generations.size())
        {
            //Always dump everyone from the final generation
            TiXmlElement *generationElementPtr = new TiXmlElement(generations[generations.size()-1]->getTypeName());
            generations[generations.size()-1]->dumpBest(generationElementPtr,includeGenes);
            root->LinkEndChild(generationElementPtr);
        }

        //cerr<<"%%%%%%%%%%%%%%%%%%\n";
        //if(generations.size())
        //	cerr<<generations[generations.size()-1]->getIndividualCount();
        //if(generations.size() > 1)
        //	cerr << endl << generations[generations.size()-2]->getIndividualCount();
//        if(generations.size() > 2) {
		if(generations.size() > 2) {
            generations.erase(generations.begin());
        //cerr << endl << generations[generations.size()-3]->getIndividualCount();
        }
        //cerr<<"\n%%%%%%%%%%%%%%%%%%\n";
			
        if (doGZ)
        {
            doc.SaveFileGZ();
        }
        else
        {
            doc.SaveFile();
        }
    }

    void GeneticPopulation::cleanupOld(int generationSkip)
    {
        for (int a=0;a<generations.size()-1/*onGeneration*/;a++)
        {
            if ( (a%generationSkip) == 0 )
                continue;

            generations[a]->cleanup();
        }
    }

    //jmc adding something to write generation champion fitness and average fitness              
    void GeneticPopulation::printToGenChampFile()
    {        
        double totalIndividualFitnessPrint =0; 
        
        for (int a=0;a<generations[generations.size()-1/*onGeneration*/]->getIndividualCount();a++)
        {
            shared_ptr<GeneticIndividual> ind = generations[generations.size()-1/*onGeneration*/]->getIndividual(a);
            totalIndividualFitnessPrint+=ind->getFitness();
        }

        double averageFitnessPrint = totalIndividualFitnessPrint/generations[generations.size()-1/*onGeneration*/]->getIndividualCount();

        ofstream output_file;        
        output_file.open ("gen-Genchamp-AvgFit.txt", ios::app );
        if(onGeneration==0)
        {
            output_file << "# 1. generation\n";
            output_file << "# 2. genChamp Fitness\n";
            output_file << "# 3. genChamp Node Count\n";
            output_file << "# 4. averageFitness\n";
						output_file << "# 5. genChamp Direction Changes\n";
			if(generations[generations.size()-1/*onGeneration*/]->getIndividual(0)->getUserData()) output_file << "# 6. UserData (if you implemented it)\n";
            output_file << endl;
        }
        output_file << onGeneration+1 << " " 
                    << generations[generations.size()-1/*onGeneration*/]->getIndividual(0)->getFitness() << " "
                    << generations[generations.size()-1/*onGeneration*/]->getIndividual(0)->getNodesCount() << " "
                    << averageFitnessPrint << " "
					<< generations[generations.size()-1/*onGeneration*/]->getIndividual(0)->getDirectionChanges();
		
					if(generations[generations.size()-1/*onGeneration*/]->getIndividual(0)->getUserData()) // not part of every experiment. 
					{
						output_file << " "
		         				   << generations[generations.size()-1/*onGeneration*/]->getIndividual(0)->getUserData()->summaryToString();  
					}
		
		output_file	<< endl;
        output_file.close();
    }
}


