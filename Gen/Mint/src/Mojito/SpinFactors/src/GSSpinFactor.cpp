
#include "Mint/GSSpinFactor.h"
#include "Mint/CLHEPSystemOfUnits.h"

#include <iostream>

using namespace std;

double GSSpinFactor::getVal(){
  bool dbThis=false;
  if(dbThis) cout << "GSSpinFactor called" << endl;

  if(_nonResonant){
    return nonResVal();
  }
  
  if(! ( fsPS[0] && fsPS[1] && fsPS[2])) parseTree();
  
  if(0 == R){
    cout << "ERROR in GSSpinFactor::getVal(): 0 == R"
	 << endl;
    throw "such things shouldn't happen";
  }

  double m2AC = (p(0) + p(1)).M2();
  double m2BC = (p(0) + p(2)).M2();

  return (m2AC - m2BC)/(GeV*GeV);

}
//
