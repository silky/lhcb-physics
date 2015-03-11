#ifndef DALITZ_CHI_SQUARED_BOX_HH
#define DALITZ_CHI_SQUARED_BOX_HH

#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzArea.h"
#include <vector>
#include <iostream>

class IDalitzEvent;
class Chi2Box{
  DalitzArea _area;

  int _nData;
  int _nMC;
  double _nWeightedMC;

  double _weightMC_Squared;

 public:
  Chi2Box();
  Chi2Box(const DalitzEventPattern& pat);
  Chi2Box(const DalitzArea& area);
  Chi2Box(const Chi2Box& other);

  std::vector<Chi2Box> split(int n=2);

  void resetEventCounts();
  void resetAll(); // changes box size!!
  void enclosePhaseSpace(double safetyFactor=1.2);

  bool addData(const IDalitzEvent& evt);
  bool addData(const IDalitzEvent* evt);
  bool addMC(const IDalitzEvent& evt, double weight);
  bool addMC(const IDalitzEvent* evt, double weight);

  int nData() const;

  int nMC() const;  
  double weightedMC() const;
  
  double weightedMC2() const;
  double rmsMC(int Ntotal) const;

  double t01Min() const{return _area._t01.min();} // = s234
  double t01Max() const{return _area._t01.max();}

  double s12Min() const{return _area._s12.min();}
  double s12Max() const{return _area._s12.max();}

  double s23Min() const{return _area._s23.min();}
  double s23Max() const{return _area._s23.max();}

  double s34Min() const{return _area._s34.min();}
  double s34Max() const{return _area._s34.max();}

  double t40Min() const{return _area._t40.min();} // = s123
  double t40Max() const{return _area._t40.max();}

  void print(std::ostream& os = std::cout) const;
};

std::ostream& operator<<(std::ostream& os, const Chi2Box& box);

#endif
//
