//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtDecayProb.cc
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include "EvtGenBase/EvtDecayBase.hh"
#include "EvtGenBase/EvtDecayProb.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRadCorr.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"

void EvtDecayProb::makeDecay(EvtParticle* p){

  int ntimes=10000;

  double dummy;

  do{
    _weight=1.0;

    decay(p);
    //report(INFO,"EvtGen") << _weight << std::endl;
    ntimes--;
    
    _prob = _prob/_weight;
    
    dummy=getProbMax(_prob)*EvtRandom::Flat();

    //  report(INFO,"EvtGen") << _prob <<" "<<dummy<<" "<<ntimes<<std::endl;
  }while(ntimes&&(_prob<dummy));
  //report(INFO,"EvtGen") << ntimes <<std::endl;
  if (ntimes==0){
    report(DEBUG,"EvtGen") << "Tried accept/reject:10000"
			   <<" times, and rejected all the times!"<<std::endl;
    report(DEBUG,"EvtGen") << "Is therefore accepting the last event!"<<std::endl;
    report(DEBUG,"EvtGen") << "Decay of particle:"<<
      EvtPDL::name(p->getId()).c_str()<<"(channel:"<<
      p->getChannel()<<") with mass "<<p->mass()<<std::endl;
    
    int ii;
    for(ii=0;ii<p->getNDaug();ii++){
      report(DEBUG,"EvtGen") <<"Daughter "<<ii<<":"<<
	EvtPDL::name(p->getDaug(ii)->getId()).c_str()<<" with mass "<<
	p->getDaug(ii)->mass()<<std::endl;
    }				   
  }


  EvtSpinDensity rho;
  rho.SetDiag(p->getSpinStates());
  p->setSpinDensityBackward(rho);
  if (getPHOTOS() || EvtRadCorr::alwaysRadCorr()) {
    EvtRadCorr::doRadCorr(p);
  }


  //Now decay the daughters.
  int i;
  for(i=0;i<p->getNDaug();i++){
    //Need to set the spin density of the daughters to be
    //diagonal.
    rho.SetDiag(p->getDaug(i)->getSpinStates());
    p->getDaug(i)->setSpinDensityForward(rho);

    //Now decay the daughter.  Really!
    p->getDaug(i)->decay();
  } 
			    
}




