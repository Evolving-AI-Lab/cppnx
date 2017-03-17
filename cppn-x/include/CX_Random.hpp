/*
 * Random.hpp
 *
 *  Created on: Jan 23, 2015
 *      Author: Joost Huizinga
 *
 * A small collection of functions for creating random doubles.
 *
 * You do not need to modify this file.
 */

#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <CX_Debug.hpp>

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>

#include <QtGlobal>

/**
 * Seeds the random number generator with the indicated seed.
 */
inline void seed(unsigned int seed){
    dbg::out(dbg::info, "random") << "Seed set: " << seed << std::endl;
    qsrand(seed);
}

/**
 * Seeds the random number generator based on the current time.
 */
inline void seed(){
    time_t current_time = time(NULL);
    seed(current_time);
}

/**
 * Returns a random double in the interval [0,1].
 */
inline double randDouble(){
    double random = (double)qrand()/(double)RAND_MAX;
    dbg::out(dbg::info, "random") << "randDouble: " << random << std::endl;
    return random;
}

/**
 * Returns a random double in the interval [0,max]
 */
inline double randDouble(double max){
    double random = randDouble() * max;
    dbg::out(dbg::info, "random") << "randDouble: " << random << std::endl;
    return random;
}

/**
 * Returns a random double in the interval [min, max]
 */
inline double randDouble(double min, double max){
    double random = randDouble() * (max - min) + min;
    dbg::out(dbg::info, "random") << "randDouble: " << random << std::endl;
    return random;
}

inline int randInt(int min, int max){
    int randomIndex = int(randDouble(min, max));
    while(randomIndex >= max){
        randomIndex = int(randDouble(min, max));
    }
    dbg::out(dbg::info, "random") << "randInt: " << randomIndex << std::endl;
    return randomIndex;
}

inline int randInt(int max){
    return randInt(0, max);
}

/**
 * Gaussian random function to produce a real number according to a Gaussian distribution.
 *
 * Original implementation came from the sferes2 framework:
 * https://github.com/jbmouret/sferes2
 *
 * @param m The median of the Gaussian distribution.
 * @param v The deviation (the default is 1, the standard deviation).
 */
inline double randGaussian(double m=0.0,double v=1.0)
{
    double factor = sqrt(-2.0f * log(randDouble()));
    float angle  = 2.0f * M_PI * randDouble();
    double random =  double(m + v * factor * cos(angle));
    dbg::out(dbg::info, "random") << "randGaussian: " << random << std::endl;
    return random;
}

/**
 * When generating a double in [0, max], there exists some non-zero
 * probability that the number will be exactly max.
 *
 * Another way to solve this issue is to generate number with
 * (double)rand()/((double)RAND_MAX-1);
 * but there are some reports that this can still generate a 1.0,
 * even though it should not.
 *
 * Thus I chose to go with the while loop
 */
inline size_t randIndex(size_t min, size_t max){
    dbg::out(dbg::info, "random") << "randIndex min: " << min << " max: " << max << std::endl;
    dbg::assertion(DBG_ASSERTION(max > min));
    size_t randomIndex = size_t(randDouble(min, max));
    while(randomIndex >= max){
        randomIndex = size_t(randDouble(min, max));
    }
    dbg::out(dbg::info, "random") << "randIndex: " << randomIndex << std::endl;
    return randomIndex;
}

/**
 * Overloaded version of randIndex
 */
inline size_t randIndex(size_t max){
    return randIndex(0, max);
}

/**
 * Returns a random element from the container.
 */
template<typename Container>
inline typename std::iterator_traits<typename Container::iterator>::reference randElement(Container& container){
    typedef typename Container::iterator it_t;
    typedef typename std::iterator_traits<it_t>::difference_type difference_type;
    difference_type difference = container.end() - container.begin();
    difference_type index = randIndex(difference);
    return *(container.begin() + index);
}

/**
 * Wrapper around std::random_shuffle so we can replace it with something else if required.
 */
template <class _RandomAccessIterator>
inline void randomShuffle(_RandomAccessIterator __first, _RandomAccessIterator __last){
    std::random_shuffle(__first, __last);
}


#endif /* RANDOM_HPP_ */
