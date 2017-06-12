/*
 * CX_Timer.hpp
 *
 *  Created on: May 2, 2017
 *      Author: joost
 */

#ifndef CPPN_X_INCLUDE_CX_TIMER_HPP_
#define CPPN_X_INCLUDE_CX_TIMER_HPP_

#include <ctime>

class Timer{
public:
	Timer():
	    _currentTime(0),
	    _previousTime(0),
	    _secondsDif(0),
	    _processed(0),
	    _expected(0),
	    _expectedStable(0),
	    _stability_threshold(0.2),
	    _stableTime(0),
	    _stable(false)
	{

	}

	void update(){
        std::time(&_currentTime);
        if(_previousTime){
        	_secondsDif = std::difftime(_currentTime, _previousTime);
            _expected *= (_processed/(_processed+1.0));
            _expected += (_secondsDif)/(_processed+1.0);
            _processed += 1.0;

            if((fabs(_expected - _expectedStable) > _stability_threshold) ||
            		(_expectedStable == 0 && _expected > 0))
            {
                _expectedStable = _expected;
                _stableTime = 0;
            } else {
                _stableTime++;
                _stable = true;
            }
        }
        _previousTime = _currentTime;
	}

	void printExpected(int total, int current){
		if(_previousTime){
			int secondsRemaining = _expectedStable * double(total-current);
			int minutes = secondsRemaining/60;
			int hours = minutes/60;
			secondsRemaining = secondsRemaining % 60;
			minutes = minutes % 60;

			std::cout << " Processing time (seconds): " << _secondsDif <<
					" average: " << _expected;
			std::cout << " remaining: ";
			if(_stable){
				std::cout << hours << ":" << minutes << ":" << secondsRemaining;
			} else {
				std::cout << "calculating";
			}
		}
		std::cout << std::endl;
	}

protected:
    std::time_t _currentTime;
    std::time_t _previousTime;
    double _secondsDif;
    double _processed;
    double _expected;
    double _expectedStable;
    double _stability_threshold;
    size_t _stableTime;
    bool _stable;
};



#endif /* CPPN_X_INCLUDE_CX_TIMER_HPP_ */
