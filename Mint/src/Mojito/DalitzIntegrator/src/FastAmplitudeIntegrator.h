#ifndef FAST_AMPLITUDE_INTEGRATOR_HH
#define FAST_AMPLITUDE_INTEGRATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "IGetDalitzEvent.h"
#include "IDalitzIntegrator.h"

#include "DalitzEventPattern.h"

#include "DalitzEvent.h"
#include "DalitzEventList.h"

#include "TRandom.h"

#include "IEventGenerator.h"

#include "IFastAmplitudeIntegrable.h"
#include "IIntegrationCalculator.h"
#include "IntegCalculator.h"

#include "DalitzHistoSet.h"
#include "counted_ptr.h"

class FitAmpSum;
class IDalitzEvent;

namespace MINT{
  class Minimiser;
}
class FastAmplitudeIntegrator : virtual public IDalitzIntegrator{
  mutable double _mean;
  const static long int _minEvents=1000;
  int _numEvents;
  int _Ncalls;

  bool _initialised;

  bool _db;
 protected:

  DalitzEventPattern _pat;
  IFastAmplitudeIntegrable* _amps; // amplitudes
  //MINT::counted_ptr<IGetDalitzEvent> _weight; // for efficiencies etc
  //  FitAmpPairList _ampList;
  //MINT::counted_ptr<IIntegrationCalculator> _integCalc;
  //MINT::counted_ptr<IIntegrationCalculator> _integCalc_copyForDebug;
  MINT::counted_ptr<IntegCalculator> _integCalc;
  MINT::counted_ptr<IntegCalculator> _integCalc_copyForDebug;

  TRandom* _rnd;
  MINT::counted_ptr<TRandom> _localRnd;
  double _precision;

  double weight(IDalitzEvent* evtPtr);

  int updateEventSet(long int Nevents);
  double evaluateSum();
  int determineNumEvents();
  int generateEnoughEvents();
  int addEvents(long int Nevents);

  TRandom* makeNewRnd(int seed=-9999);
  MINT::IEventGenerator<IDalitzEvent>* _generator;
 public:
  FastAmplitudeIntegrator();

  FastAmplitudeIntegrator(const DalitzEventPattern& pattern
			  , IFastAmplitudeIntegrable* amps=0
			  //, MINT::counted_ptr<IGetDalitzEvent> weight=0
			  , MINT::IEventGenerator<IDalitzEvent>* 
			    eventGenerator=0
			  , TRandom* rnd = 0
			  , double precision = 1.e-2
			  );

  FastAmplitudeIntegrator( const DalitzEventPattern& pattern
			   , IFastAmplitudeIntegrable* amps
			   , const std::string& fname
			   );

  bool initialise(const DalitzEventPattern& pattern
		  , IFastAmplitudeIntegrable* amps=0
		  //, MINT::counted_ptr<IGetDalitzEvent> weight=0
		  , MINT::IEventGenerator<IDalitzEvent>* eventGenerator=0
		  , TRandom* rnd = 0
		  , double precision = 1.e-2
		  );
  bool initialise(const std::string& commaSeparatedList
		  , const DalitzEventPattern& pattern
		  , IFastAmplitudeIntegrable* amps=0
		  //, MINT::counted_ptr<IGetDalitzEvent> weight=0
		  , MINT::IEventGenerator<IDalitzEvent>* eventGenerator=0
		  , TRandom* rnd = 0
		  , double precision = 1.e-2
		  );
  bool initialiseFromFile(const DalitzEventPattern& pattern
			  , IFastAmplitudeIntegrable* amps
			  , const std::string& commaSeparatedList
			  );

  virtual bool add(const FastAmplitudeIntegrator& other);

  bool setValues(const DalitzEventPattern& pattern
		 , IFastAmplitudeIntegrable* amps=0
		 //, MINT::counted_ptr<IGetDalitzEvent> weight=0
		 , MINT::IEventGenerator<IDalitzEvent>* eventGenerator=0
		 , TRandom* rnd = 0
		 , double precision = 1.e-2
		 );

  void setPrecision(double prec){
    _precision=prec;
    if(initialised()){
      generateEnoughEvents();
    }
  }

  // warning - this will not re-evaluate the #events needed for
  // the precision etc.

  bool initialised() const{
    return _initialised;
  }

  double getVal();

  double RealVal(){
    return getVal();
  }

  double variance() const;

  DalitzHistoSet histoSet() const;
  void saveEachAmpsHistograms(const std::string& prefix) const;
  virtual void doFinalStats(MINT::Minimiser* mini=0);
  double getFractionChi2()const;

  bool ensureFreshEvents();

  bool save(const std::string& fname)const;

};
#endif
//
