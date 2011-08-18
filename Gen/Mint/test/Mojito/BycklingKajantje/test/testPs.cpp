// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/Mojito/BycklingKajantje/phaseSpaceIntegrals.h"
#include "Mint/Mint/NamedParameter/NamedParameter.h"
#include "Mint/Mojito/DalitzEvents/DalitzEventPattern.h"

#include <iostream>

using namespace std;
using namespace MINT;

int main(){

  DalitzEventPattern pdg3(421, 311, -211, +211);
  DalitzEventPattern pdg4(421, 321, -211, -211, 211);

  PhaseSpaceIntegral3body psi3;
  PhaseSpaceIntegral4body psi4;

  cout << " getting 3-body phase space "
       << psi3.getVal(pdg3)
       << endl;
  

  cout << " getting 4-body phase space "
       << psi4.getVal(pdg4)
       << endl;
  
  cout << " getting 4-body phase space (check) "
       << psi4.getValCheck(pdg4)
       << endl;
  
}
