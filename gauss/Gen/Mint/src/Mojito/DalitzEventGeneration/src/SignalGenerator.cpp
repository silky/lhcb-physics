// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
#include "Mint/SignalGenerator.h"
#include <ctime>
#include <iostream>
#include <complex>

using namespace std;
using namespace MINT;

SignalGenerator::SignalGenerator(const DalitzEventPattern& pat, TRandom* rnd)
  : BaseGenerator(rnd)
  , _myOwnPSet()
  , _counted_amps(new FitAmpSum(pat, &_myOwnPSet))
  , _amps(_counted_amps.get())
  , _boxes(_amps->makeEventGenerator())
{
  _boxes->setRnd(rnd);
}
SignalGenerator::SignalGenerator(const DalitzEventPattern& pat
				 , double rB
				 , double phase
				 , TRandom* rnd)
  : BaseGenerator(rnd)
  , _myOwnPSet()
  , _counted_amps(0)
  , _amps(0)
  , _boxes(0)
{
  DalitzEventPattern cpPat(pat);
  cpPat[0].antiThis();

  counted_ptr<FitAmpSum> fs(new FitAmpSum(pat, &_myOwnPSet));
  FitAmpSum cpAmps(pat, &_myOwnPSet);
  cpAmps *= polar(rB, phase);
  fs->add(cpAmps);
  _counted_amps = (counted_ptr<IFastAmplitudeIntegrable>) fs;
  _amps = _counted_amps.get();

  makeBoxes();

  /*
  counted_ptr< IUnweightedEventGenerator<IDalitzEvent> > 
    bpt(fs->makeEventGenerator());

  _boxes = bpt;
  _boxes->setRnd(_rnd);
  */
}
SignalGenerator::SignalGenerator(IFastAmplitudeIntegrable* amps, TRandom* rnd)
  : BaseGenerator(rnd)
  , _myOwnPSet()
  , _amps(amps)
  , _boxes(_amps->makeEventGenerator())
{
  _boxes->setRnd(_rnd);
}

bool SignalGenerator::makeBoxes(){
  if(0 == _amps) return 0;
  counted_ptr< IUnweightedEventGenerator<IDalitzEvent> > 
    bpt(_amps->makeEventGenerator());
  _boxes = bpt;
  if(0 == _boxes) return 0;
  _boxes->setRnd(_rnd);
  return true;
}

counted_ptr<IDalitzEvent> SignalGenerator::tryDalitzEvent(){
  bool dbThis=false;
  if(0 == _boxes) makeBoxes();
  if(_unWeighted){
    counted_ptr<IDalitzEvent> evtPtr(_boxes->newUnweightedEvent());
    if(0 != evtPtr) evtPtr->setMothers3Momentum(mothers3Momentum());
    if(dbThis && 0 != evtPtr){
      cout << "SignalGenerator::tryDalitzEvent(): made un-weighted event "
	   << "with weight " << evtPtr->getWeight() << endl;
    }
    return evtPtr;
  }else{
    counted_ptr<IDalitzEvent> evtPtr(_boxes->newEvent());
    if(0 != evtPtr) evtPtr->setMothers3Momentum(mothers3Momentum());
    if(dbThis && 0 != evtPtr){
      cout << "SignalGenerator::tryDalitzEvent(): made weighted event "
	   << "with weight " << evtPtr->getWeight() << endl;
    }
    return evtPtr;
  }
}

counted_ptr<IDalitzEvent> SignalGenerator::newDalitzEvent(){
  counted_ptr<IDalitzEvent> evt(0);
  int counter(0);
  int largeNumber(1000000);

  do{
    evt = tryDalitzEvent();
  }while(0 == evt &&  counter++ < largeNumber);
  if(saveEvents()) _evtList->Add(evt);
  return evt;
}

bool SignalGenerator::ensureFreshEvents(){
  _boxes->ensureFreshEvents();
  return BaseGenerator::ensureFreshEvents();
}
counted_ptr<IDalitzEvent> SignalGenerator::newEvent(){
  return counted_ptr<IDalitzEvent>(newDalitzEvent());
}

//
