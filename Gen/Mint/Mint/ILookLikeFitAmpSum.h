#ifndef MINTDALITZ_ILOOKLIKE_FITAMPSUM_HH
#define MINTDALITZ_ILOOKLIKE_FITAMPSUM_HH

#include "Mint/ILookLikeFitAmpSum.h"
#include "Mint/IIntegrationCalculator.h"
#include "Mint/IntegCalculator.h"
#include "Mint/IGetRealEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IFastAmplitudeIntegrable.h"

#include "Mint/counted_ptr.h"
#include "Mint/DalitzBWBoxSet.h"
#include "TRandom.h"
#include <iostream>

class ILookLikeFitAmpSum
: virtual public MINT::IGetRealEvent<IDalitzEvent>
, virtual public IFastAmplitudeIntegrable
{
 public:
  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator()=0;
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator()=0;

  virtual double RealVal()=0; // | sum A |^2

  virtual MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    makeEventGenerator(TRandom* rnd=gRandom)=0;

  virtual void print(std::ostream& os=std::cout) const=0;
  virtual void printNonZero(std::ostream& os=std::cout) const=0;

  virtual DalitzBWBoxSet makeBWBoxes(TRandom* rnd=gRandom)=0;

  virtual ~ILookLikeFitAmpSum(){};

};

#endif
//
