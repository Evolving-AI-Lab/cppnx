/*
 * CX_Statistics.hpp
 *
 *  Created on: May 2, 2017
 *      Author: joost
 */

#ifndef CPPN_X_INCLUDE_CX_STATISTICS_HPP_
#define CPPN_X_INCLUDE_CX_STATISTICS_HPP_

struct Statistics{
	Statistics():
		branch(-1),
		iteration(0),
		possibleCon(0),
		matchingCon(0),
		actualCon(0),
		chanceOfSelection(-1),
		expectedValue(-1),
		modularity(-1),
		hierarchyForward(-1),
		hierarchyBackward(-1)
	{
		// nix
	}

	void dump(QTextStream& stream){
		stream << branch <<
				" " << iteration <<
				" " << modularity <<
				" " << hierarchyForward <<
				" " << hierarchyBackward <<
				" " << possibleCon <<
				" " << actualCon <<
				" " << matchingCon <<
				" " << expectedValue << "\n";
	}

	int branch;
	size_t iteration;
    size_t possibleCon;
    size_t matchingCon;
    size_t actualCon;
	double chanceOfSelection;
	double expectedValue;
	double modularity;
	double hierarchyForward;
	double hierarchyBackward;
};



#endif /* CPPN_X_INCLUDE_CX_STATISTICS_HPP_ */
