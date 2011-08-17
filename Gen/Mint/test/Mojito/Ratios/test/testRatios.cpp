// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include "FitParameter.h"
#include "NamedParameter.h"
#include "CLHEPPhysicalConstants.h"

#include "FitAmplitude.h"
#include "FitAmpSum.h"

#include "DalitzEvent.h"

#include "AmpRatios.h"

#include "cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"

#include <iostream>

using namespace std;
using namespace MINT;

#include "TRandom2.h"
#include <ctime>

int ampRatiosTest(){
  time_t startTime = time(0);

  TRandom2 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed(0);
  gRandom = &ranLux;

  FitAmplitude::AutogenerateFitFile();

   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  DalitzEventList eventList;
  cout << "now about ot generate " << 5 << " events" << endl;
  eventList.generatePhaseSpaceEvents(5, pdg);
  eventList.Start();

  FitAmpSum amps(&eventList);
  // IMPORTANT: figure out why amps(pdg) leads to crash in AmpRatios

  AmpRatios rats(pdg);

  rats.getRatios(amps);
  
  cout << "this took " << (time(0) - startTime) << " s" << endl;
  return 0;
}


int main(){

  return ampRatiosTest();

}
//
