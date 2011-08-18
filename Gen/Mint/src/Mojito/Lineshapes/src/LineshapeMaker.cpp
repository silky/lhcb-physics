// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/Mojito/Lineshapes/LineshapeMaker.h"
#include "Mint/Mojito/Lineshapes/ILineshape.h"
#include "Mint/Mint/Utils/Utils.h"
#include "Mint/Mojito/DecayTrees/AssociatedDecayTree.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEventAccess.h"
#include "Mint/Mojito/Lineshapes/BW_BW.h"
#include "Mint/Mojito/Lineshapes/GounarisSakurai.h"
#include "Mint/Mojito/Lineshapes/Lass.h"
#include "Mint/Mojito/Lineshapes/Flatte.h"
#include "Mint/Mojito/Lineshapes/FocusFlatte.h"
#include "Mint/Mojito/Lineshapes/CrystalBarrelFOCUS.h"

#include <iostream>

using namespace std;
using namespace MINT;

/* 
   possible options
   AWAYS_BW
   RHO_OMEGA
   Flatte
   GS
*/
ILineshape* LineshapeMaker(const AssociatedDecayTree* tree
			   , IDalitzEventAccess* events
			   , const std::string& lopt
			   ){
  bool dbThis=false;
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
      
      return new CrystalBarrelFOCUS(*tree, events);
      //return new BW_BW(*tree, events);
    }else if((abs(tree->getVal().pdg())%1000)==113 && A_is_in_B("GS", lopt)){
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
    if(A_is_in_B("FocusFlatte", lopt)){
      cout << "LineshapeMaker: "
	   << "\n\t> returning Flatte lineshape"
	   << endl;
      return new FocusFlatte(*tree, events);
    }else if(A_is_in_B("Flatte", lopt)){
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
  }else{
    if(dbThis) cout << "LineshapeMaker returns BW_BW" << endl;
    return new BW_BW(*tree, events);
  }

}

//
