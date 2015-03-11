// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/LineshapeMaker.h"
#include "Mint/CLEO2012_LineshapeMaker.h"
#include "Mint/ILineshape.h"
#include "Mint/Utils.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/IDalitzEventAccess.h"
#include "Mint/BW_BW.h"
#include "Mint/GounarisSakurai.h"
#include "Mint/Lass.h"
#include "Mint/Flatte.h"
#include "Mint/CrystalBarrelFOCUS.h"
#include "Mint/Rho0Omega.h"
#include "Mint/NonRes.h"

#include <iostream>

using namespace std;
using namespace MINT;

/* 
   possible options
   AWAYS_BW
   RHO_OMEGA
   WRONG_RHOOMEGA
   Flatte
   GS
   ExpNonRes
   PowNonRes
*/

ILineshape* LineshapeMaker(const AssociatedDecayTree* tree
			   , IDalitzEventAccess* events
			   , const std::string& lopt
			   ){
  bool dbThis=false;

  if(A_is_in_B("CLEO2012", lopt)){
    return CLEO2012_LineshapeMaker(tree, events, lopt);
  }

  if(0 == tree) return 0;
  
  if(dbThis){
    cout << "LineshapeMaker called with " 
	 << tree->getVal().pdg() 
	 << " lopt = " << lopt << endl;

    cout << tree->getVal().pdg() << ", "
	 << (abs(tree->getVal().pdg())%1000)
	 << endl;
  }
      

  if(A_is_in_B("ALWAYS_BW", lopt)){
    if(dbThis) cout << "LineshapeMaker returns BW_BW" << endl;
    return new BW_BW(*tree, events);
  }

  if((abs(tree->getVal().pdg())%1000)==113){
    if(abs(tree->getVal().pdg()) == 113 && A_is_in_B("RHO_OMEGA", lopt)){
      if(dbThis)cout << "LineshapeMaker returning rho-omega lineshape"
		     << endl;

      return new Rho0Omega(*tree, events);
    }else if(abs(tree->getVal().pdg()) == 113 && A_is_in_B("WRONG_RHOOMEGA", lopt)){
      if(dbThis)cout << "LineshapeMaker returning rho-omega lineshape"
		     << endl;

      std::cout << "OIII!!!  WARNING!!!!" << std::endl;
      std::cout << "CrystalBarrelFOCUS has many known issues" << std::endl;
      std::cout << "Use at your own risk" << std::endl << std::endl;

      return new CrystalBarrelFOCUS(*tree, events);
    }else if(abs(tree->getVal().pdg()) == 113 && A_is_in_B("GS", lopt)){
      if(dbThis) cout << "LineshapeMaker: return GS lineshape" << endl;
      return new GounarisSakurai(*tree, events);
    }else{
      if(dbThis)cout << "WARNING: LineshapeMaker:"
		     << " returning plain Breit-Wigner (BW_BW) for rho"
		     << endl;
      return new BW_BW(*tree, events);
    }
  }else if(abs(tree->getVal().pdg()) == 10321 || abs(tree->getVal().pdg()) == 10311 ){ // K0*(1430), charged or neutral
    if(A_is_in_B("Lass", lopt)){
      cout << "LineshapeMaker: "
	   << "\n\t> returning Lass lineshape"
	   << endl;
      return new Lass(*tree, events);
    }else{
      cout << "WARNING: LineshapeMaker:"
	   << " returning plain Breit-Wigner (BW_BW) for K0*(1430)"
	   << endl;
      return new BW_BW(*tree, events);
    }
  }else if(abs(tree->getVal().pdg()) == 9010221 ){ // f0(980)
    if(A_is_in_B("Flatte", lopt)){
      cout << "LineshapeMaker: "
	   << "\n\t> returning Flatte lineshape"
	   << endl;
      return new Flatte(*tree, events);
    }else{
      cout << "WARNING: LineshapeMaker:"
	   << " returning plain Breit-Wigner (BW_BW) for f0(980)"
	   << endl;
      return new BW_BW(*tree, events);
    }
  }else if( abs(tree->getVal().pdg()) == 9981 ||
	    abs(tree->getVal().pdg()) == 9991 ||
	    abs(tree->getVal().pdg()) == 9983 ||
	    abs(tree->getVal().pdg()) == 9993 ||
	    abs(tree->getVal().pdg()) == 9985 ||
	    abs(tree->getVal().pdg()) == 9975 ){ //Non-resonant
    std::cout << "asdfasdfasdf " << lopt << std::endl;
    if( A_is_in_B("NonRes", lopt) ){
      cout << "LineshapeMaker: "
	   << "\n\t> returning Non-resonant lineshape"
	   << endl;
      if( A_is_in_B("Exp", lopt) )
	return new NonRes(*tree, events, "Exp");
      else if( A_is_in_B("Pow", lopt) )
	return new NonRes(*tree, events, "Pow");
      else
	return new NonRes(*tree, events);
    }else{
      cout << "WARNING: LineshapeMaker:"
	   << " returning plain Breit-Wigner (BW_BW) for non-resonant"
	   << endl;
      return new BW_BW(*tree, events);
    }
  }else{
    if(dbThis) cout << "LineshapeMaker returns BW_BW" << endl;
    return new BW_BW(*tree, events);
  }

}

//
