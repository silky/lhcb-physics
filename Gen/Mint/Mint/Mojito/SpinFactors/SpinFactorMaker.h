// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT
#ifndef SPINFACTOR_MAKER
#define SPINFACTOR_MAKER

#include <iostream>
#include "Mint/Mojito/DecayTrees/DecayTree.h"
#include "Mint/Mojito/SpinFactors/ISpinFactor.h"

#include "Mint/Mojito/DalitzEvents/IDalitzEventAccess.h"

void PrintAllSpinFactors(std::ostream& out = std::cout);

ISpinFactor* SpinFactorMaker(const DecayTree& thisDcy
			     , IDalitzEventAccess* dad
			     , char SPD_Wave='?'
			     , const std::string& lopt=""
			    );
ISpinFactor* SpinFactorMaker4Body(const DecayTree& thisDcy
				  , IDalitzEventAccess* dad
				  , char SPD_Wave='?'
				  //, const std::string& lopt=""
				  );
#endif
//
