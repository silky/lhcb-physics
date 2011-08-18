#ifndef DALITZ_BOX_HH
#define DALITZ_BOX_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include "Mint/Mojito/BetterMC/MappedDalitzArea.h"
#include "Mint/Mojito/DalitzEvents/DalitzEventPattern.h"
#include "Mint/Mojito/BreitWignerMC/DalitzCoordinate.h"
#include "Mint/Mint/Events/IGetRealEvent.h"
#include "Mint/Mojito/DalitzEvents/DalitzEventList.h"


#include "Mint/Mint/Utils/counted_ptr.h"

#include "TRandom.h"

#include <vector>
#include <string>
#include <iostream>

class DalitzBox{
  double _max_s0, _max_s1, _max_s2, _max_s3, _max_s4;
  double _maxPhaseSpace;

  int _number;
  std::string _name;

  bool _ready;

  MappedDalitzArea _area;

  DalitzEventPattern _pat;
  MINT::IGetRealEvent<IDalitzEvent>* _amps;

  TRandom* _rnd;
  double _height;
  DalitzEventList _eventList;

  int _nTries;
  int _nEventsForTries;
  const DalitzBox* _daddy;

  double _guessedHeight;
  unsigned int _heightProblems;

  bool _flatPhaseSpace;

  int makeFlatEventsKeepAll(int N);

  bool linkAmpsToEvents();
  bool unLinkAmps();
  bool estimateHeight(std::vector<double>& vals);
  bool estimateHeight_old(std::vector<double>& vals);
  bool estimateHeightMinuit();

  bool makeStarterSet();
  int throwAwayData(const std::vector<double>& vals);

  MINT::counted_ptr<DalitzEvent> tryNewEvent();
  MINT::counted_ptr<DalitzEvent> tryEventFromList();
  MINT::counted_ptr<DalitzEvent> popEventFromList();

  double getEventsPdf(DalitzEvent& evt);
  
  //  void getReady();
 public:
  void getReady();
  
  void setFlatPhaseSpace(bool flat=true){
    _flatPhaseSpace = flat;
  }
  double amps(IDalitzEvent* ep=0);
  double phaseSpace(IDalitzEvent* ep=0);
  double ampsWithPhaseSpace(IDalitzEvent* ep=0);

  DalitzBox();
  DalitzBox(const DalitzEventPattern& pat
	    , MINT::IGetRealEvent<IDalitzEvent>* amps = 0
	    , TRandom* rnd = gRandom
	    );
  
  DalitzBox(const DalitzEventPattern& pat
	    , const DalitzCoordinate& limit
	    , MINT::IGetRealEvent<IDalitzEvent>* amps =0
	    , TRandom* rnd = gRandom);

  DalitzBox(const DalitzEventPattern& pat
	    , const std::vector<DalitzCoordinate>& limits
	    , MINT::IGetRealEvent<IDalitzEvent>* amps =0
	    , TRandom* rnd = gRandom);

  DalitzBox(const DalitzBox& other);
  ~DalitzBox();

  void setNumber(int num){_number = num;}
  int number() const{return _number;}

  void setName(const std::string& name){  _name = name;}
  const std::string& name() const{return _name;}

  void setGuessedHeight(double h){_guessedHeight = h;}
  double guessedHeight() const{return _guessedHeight;}

  const MappedDalitzArea& area()const{return _area;}
  MappedDalitzArea& area(){return _area;}

  double height() const{
    return _height;
  }
  double setHeight(double h){
    return _height = h;
  }
  DalitzEventList& eventList(){ return _eventList;}
  const DalitzEventList& eventList() const{ return _eventList;}
  
  double volume() const;

  void setDaddy(const DalitzBox* daddy);
  bool setAmps(MINT::IGetRealEvent<IDalitzEvent>* amps);

  bool insideArea(const DalitzEvent& evt) const;
  bool insideDaddysArea(const DalitzEvent& evt) const;
  bool insideMyOrDaddysArea(const DalitzEvent& evt) const;

  MINT::counted_ptr<DalitzEvent> tryEventForOwner();

  MINT::counted_ptr<DalitzEvent> tryWeightedEventForOwner();

  void encloseInPhaseSpaceArea(){
    area().encloseInPhaseSpaceArea();
  }

  std::vector<DalitzBox> split(unsigned int nWays) const;
  std::vector<DalitzBox> splitIfWiderThan(double maxWidth) const;

  bool setRnd(TRandom* rnd=gRandom);

  void print(std::ostream& os = std::cout) const;
};

ostream& operator<<(ostream& os, const DalitzBox& box);


#endif
//
