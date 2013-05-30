#include "NEAT_Random.h"

#include "NEAT_STL.h"

//This is the Avida RNG-based file. See the commented out one below for the original RNG. 
//The original was not consistent across platforms

namespace NEAT
{
    Random::Random(unsigned int _seed)
            :
            seed(_seed),
	    generator(_seed?_seed:42) //-1 means set to time with the Avida RNG, this is an unsigned int so we can't set to -1. Default to 42 if nothing set explicitly
    {
            cout << "SEED set to : " << seed << "for this: " << this << endl;
    }

    int Random::getRandomInt(int limit)
    {   
          ///Gets a random int 0 <= x < limit, in jason's boost RNG, and in Avida RNG
                    
          return generator.GetInt(limit); 
    }

    int Random::getRandomWithinRange(int min,int max)
    {
          ///Gets a random int min <= x <= max;

          return generator.GetInt(min, max+1);   //+1 because jason's code needs to include max, and avida RNG does not
    }

    double Random::getRandomDouble()
    {
          //return a number between (but not including) 0 and 1, true for both boost and Avida RNG

          return generator.GetDouble(); 
    }

    double Random::getRandomDouble(double low,double high)
    {
          //generate a double betwen (but not including) low and high (true for both boost and avida RNG)          
          
          return generator.GetDouble(low, high);          
    }
}


// this is the old random number generator file released by Jason and using boost's RNG, before the switch to avida 
// the boost RNG was not consistent across platforms
//#include "NEAT_Random.h"
//
//#include "NEAT_STL.h"
//
//namespace NEAT
//{
//    Random::Random(unsigned int _seed)
//            :
//            seed(_seed),
//            generator(_seed?_seed:static_cast<unsigned int>(std::time(0))),
//            intDist(0,INT_MAX/2),
//            intGen(generator,intDist),
//            realDist(0,1),
//            realGen(generator,realDist),
//            normalDist(0,1.0),
//            normalGen(generator,normalDist)
//    {
////        cout << "SEED is: " << seed << "for this: " << this << endl;                                                
//    }
//
//    int Random::getRandomInt(int limit)
//    {
////        cout << "this for RandINt: " << this << endl;                                                               
//        return intGen()%limit;
//    }
//
//    int Random::getRandomWithinRange(int min,int max)
//    {
//        int value = intGen()%( (max-min) + 1);
////        cout << "this for getRandomWithinRange: " << this << endl;                                                  
//
//        return min+value;
//    }
//
//    double Random::getRandomDouble()
//    {
//
//        return realGen();
//
////        cout << "this for getRandomDouble: " << this << endl;                                                       
//    }
//
//    double Random::getRandomDouble(double low,double high)
//    {
////        cout << "this for getRandomDoubleII: " << this << endl;                                                     
//        return realGen()*(high-low) + low;
//    }
//}

