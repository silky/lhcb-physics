// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include <iostream>
#include <ctime>

#include "TError.h"

#include "DalitzEvent.h"
#include "DalitzEventList.h"

#include "DalitzBWArea.h"
#include "DalitzBWBox.h"
#include "MappedDalitzBWArea.h"
#include "DalitzCoordinate.h"

#include "FitAmpSum.h"
#include "DalitzHistoSet.h"
#include "DalitzPdfBase.h"

#include "Utils.h"

#include "NamedParameter.h"

#include "Amplitude.h"
#include "ReturnWeight.h"

#include "TRandom3.h"

using namespace std;
using namespace MINT;

class AmpsPdf
  : public DalitzPdfBase
{
protected:
  FitAmpSum _amps;
public:
  double un_normalised(){
    complex<double> cval = _amps.getVal();
    return cval.real()*cval.real() + cval.imag()*cval.imag();
  }

  AmpsPdf(IDalitzEventAccess* events=0) 
    : DalitzPdfBase(events) 
      // WARNING: this is out of date. 
      // Use DalitzPdfBaseFastInteg for 
      // faster and more accurate integration.
    , _amps((IDalitzEventAccess*)this)
  {
  }
  
};

int testBWBoxesWithAmpSum(){
  time_t t0 = time(0);

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int> RandomSeed("RandomSeed", 0);

  TRandom3 rnd((int)RandomSeed);
  gRandom = &rnd;

  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  DalitzEventList eventList;

  eventList.generatePhaseSpaceEvents(100, pdg);

  /*

  FitAmpSum fas(pdg);

  DalitzBWBoxSet boxes(fas.makeBWBoxes(pdg));

  bool verbose = false;
  
  for(int i=0; i< Nevents; i++){
    int printEvery  = 1000;
    if(i < 1000) printEvery=100;
    bool printout = ( i%printEvery == 0 || i < 10);
    if(printout){
      cout << " ================================== " << endl;
      cout << " made event number " << i << endl;
      cout << "   current list size " << eventList.size() << endl;
      double dt = difftime(time(0), t0);
      cout << "     this took " << dt << "s" << endl;
      if(dt > 0) cout << "     or " << eventList.size()/dt << "evt/s" << endl;
    }
    counted_ptr<DalitzEvent> evt(boxes.makeEventForOwner());
    if(printout && verbose) cout << "tried one event " << evt << endl;
    if(printout && verbose && 0 != evt){
      fas.setEvent(&(*evt));
      cout << " weight = " << evt->getWeight()
	   << " boxes.genValueNoPs " << boxes.genValueNoPs(*evt)
	   << " fas.RealVal() " << fas.RealVal()
	   << "." << endl;
      cout << " largest Amp ";
      fas.printLargestAmp();
      cout << endl;
      fas.resetEventRecord();
      
      cout << " adding event with weight " << evt->getWeight() << endl;
      fas.resetEventRecord();
    }
    if(0 != evt) eventList.Add(*evt);
  }
  ReturnWeight w(&eventList);
  */

  DalitzHistoSet psp= eventList.histoSet();
  psp.begin()->second.saveAsDir();
  cout << "managed to save one DalitzHistogram" << endl;
  DalitzHistogram retrievedHisto;
  retrievedHisto.retrieveFromDir(".", "sij(1,2)");
  cout << "retrieved previously saved histogram" << endl;
  retrievedHisto.saveAsDir("reSaved");
  cout << "and saved it" << endl;
  psp.save("AmpPlots.root");
  psp.saveAsDir("AmpPlots");
  cout << "now saved them all" << endl;
  DalitzHistoSet retrievedSet;
  retrievedSet.retrieveFromDir("AmpPlots");
    
  retrievedSet.saveAsDir("retrievedSet");

  //TTree* t = psp.begin()->second.makeTree();
  //DalitzHistogram testHisto(t);
  //testHisto.save("testHisto.root");
  //TFile tf("testHisto.root", "RECREATE");
  //tf.cd();
  //  testHisto.histo()->Write();
  //tf.Close();

  std::string fname = "eventList" 
    + anythingToString(Nevents/1000) +"k_seed" 
    + anythingToString((int)RandomSeed) + ".root";

  eventList.save(fname);
  cout << " the filename is " << fname << " and the format"
       << " is that of a root ntuple." << endl;
  double dt = difftime(time(0), t0);
  cout << " Done. This took " << dt/60.0 << "min" << endl;
  if(dt > 0) cout << "  - or  " << eventList.size()/dt << "evt/s" << endl;
  return 0;
}



int main(){
  //  return testBoxes();

  return testBWBoxesWithAmpSum();
}

//
