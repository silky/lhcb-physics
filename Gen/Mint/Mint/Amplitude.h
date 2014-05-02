#ifndef AMPLITUDE_HH
#define AMPLITUDE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include <complex>
#include <vector>
#include <iostream>

#include "TRandom.h"

#include "Mint/AmpInitialiser.h"
#include "Mint/AssociatingDecayTree.h"
#include "Mint/ISpinFactor.h"
#include "Mint/LineshapeMaker.h"

#include "Mint/ILineshape.h"

#include "Mint/IReturnRealForEvent.h"
#include "Mint/IReturnComplexForEvent.h"

#include "Mint/DalitzBox.h"
#include "Mint/DalitzBWBox.h"
#include "Mint/DalitzBoxSet.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/DalitzEventPattern.h"

#include "Mint/Permutation.h"

#include "Mint/DalitzEvent.h"

#include "Mint/counted_ptr.h"

class Amplitude 
: virtual public MINT::IReturnRealForEvent<IDalitzEvent>
, virtual public MINT::IReturnComplexForEvent<IDalitzEvent>
{
 protected:
  AssociatingDecayTree _associatingDecayTree;
  ISpinFactor* _spinFactor;
  char _spd;
  std::string _lopt;

  DalitzEventPattern _pat;
  bool _init;

  std::vector<ILineshape*> LineshapeList;

  virtual double boxFactor(){ return 1;}

  bool createLineshapes(const MINT::const_counted_ptr< AssociatedDecayTree >& 
			counted_tree_ptr);
  bool createLineshapes(const AssociatedDecayTree* treePtr=0);
  bool deleteLineshapes();

  bool deleteDependants();
  bool createDependants();

  bool renew();

  bool initialised() const{
    if(! _init) return false;
    if(_pat.empty()) return false;
    return true;
  }  
  bool initialise(const DalitzEventPattern& pat){
     _pat = pat;
    return renew();
  }
  bool initialiseIfNeeded(const DalitzEventPattern& pat){
    if(initialised()) return true;
    return initialise(pat);
  }

  std::complex<double> LineshapeProduct(IDalitzEvent& evt);
  //  double LineshapeGaussProduct();
  double SpinFactorValue(IDalitzEvent& evt);
  std::complex<double> getOnePermutationsVal(IDalitzEvent& evt);
 public:
  Amplitude( const DecayTree& decay
	     , char SPD_Wave='?'
	     , const std::string& opt=""
	     );

  Amplitude( const AmpInitialiser& ampInit
	     );

  Amplitude( const Amplitude& other);

  bool resetTree(const DecayTree& dt);
  bool CPConjugate();
 
  virtual std::complex<double> getVal(IDalitzEvent& evt);
  virtual std::complex<double> getVal(IDalitzEvent* evt); // for backward compatibility only, will be removed


  double Prob(IDalitzEvent& evt){
    std::complex<double> res = getVal(evt);
    return res.real()*res.real() + res.imag()*res.imag();
  }

 //  virtual double gaussProb();

  virtual double RealVal(IDalitzEvent& evt){return Prob(evt);}
  virtual std::complex<double> ComplexVal(IDalitzEvent& evt){return getVal(evt);}

  const AssociatedDecayTree& theDecay(const DalitzEventPattern& pat){
    return _associatingDecayTree.getTree(pat);
  }
  const AssociatedDecayTree& theDecay(IDalitzEvent& evt) const{
    return _associatingDecayTree.getTree(evt.eventPattern());
  }

  DecayTree theBareDecay() const{
    return _associatingDecayTree.getBareTree();
  }

  DalitzEventPattern getTreePattern() const{
    return _associatingDecayTree.getTreePattern();
  }
  

  std::string name() const;
  void print(std::ostream& out = std::cout) const;

  ISpinFactor* spinFactor(){
    return _spinFactor;
  }

  
  virtual DalitzBoxSet MakeBox(const DalitzEventPattern &pat
			       , const Permutation& perm
			       , double nSigma = 3
			       );

  virtual DalitzBoxSet MakeBoxes(const DalitzEventPattern& pat
				 , double nSigma=3);

  
  virtual DalitzBWBox MakeBWBox(const DalitzEventPattern &pat
				, const Permutation& perm
				, TRandom* rnd=gRandom
				);
  virtual DalitzBWBoxSet MakeBWBoxes(const DalitzEventPattern &pat
				     , TRandom* rnd=gRandom
				     );


  virtual ~Amplitude();

};

std::ostream& operator<<(std::ostream& out, const Amplitude& amp);

#endif
//
