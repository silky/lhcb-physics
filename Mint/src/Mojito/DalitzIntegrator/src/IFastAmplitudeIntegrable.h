#ifndef IFAST_AMPLITUDE_INTEGRABLE_HH
#define IFAST_AMPLITUDE_INTEGRABLE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "counted_ptr.h"
#include "IIntegrationCalculator.h"
#include "IGetRealEvent.h"
#include "IDalitzEvent.h"
#include "IUnweightedEventGenerator.h"
#include "TRandom.h"

#include "IntegCalculator.h"

#include <iostream>

class IFastAmplitudeIntegrable 
: virtual public MINT::IGetRealEvent<IDalitzEvent>{
 public:
  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator()=0;
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator()=0;

  virtual MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    makeEventGenerator(TRandom* rnd=gRandom)=0;

  virtual void print(std::ostream& os=std::cout) const=0;

  virtual ~IFastAmplitudeIntegrable(){}
};

#endif
//
