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
// Module: EvtTauVectornu.cc
//
// Description: The vector decay of the tau meson.
//              E.g., tau- -> rho- + nut
//
// Modification history:
//
//    RYD/SHY   April 23, 1997           Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include <iostream.h>
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtTauVectornu.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtReport.hh"

EvtTauVectornu::~EvtTauVectornu() {}

void EvtTauVectornu::getName(EvtString& model_name){

  model_name="TAUVECTORNU";     

}


EvtDecayBase* EvtTauVectornu::clone(){

  return new EvtTauVectornu;

}

void EvtTauVectornu::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(2);

  checkSpinParent(EvtSpinType::DIRAC);
    
  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::NEUTRINO);
}

void EvtTauVectornu::initProbMax(){

  setProbMax(55.0);

}

void EvtTauVectornu::decay(EvtParticle *p){

  static EvtId TAUM=EvtPDL::getId("tau-");
  p->initializePhaseSpace(getNDaug(),getDaugs());

  EvtParticle *v, *nut;
  v=p->getDaug(0);
  nut=p->getDaug(1);
  double mvec = v->mass();
  EvtVector4C tau1, tau2;

  if (p->getId()==TAUM) {
    tau1=EvtLeptonVACurrent(nut->spParentNeutrino(),p->sp(0));
    tau2=EvtLeptonVACurrent(nut->spParentNeutrino(),p->sp(1));
  }
  else{
    tau1=EvtLeptonVACurrent(p->sp(0),nut->spParentNeutrino());
    tau2=EvtLeptonVACurrent(p->sp(1),nut->spParentNeutrino());
  }

  double norm=mvec*sqrt(mvec);
  
  vertex(0,0,norm*tau1*(v->epsParent(0).conj()));
  vertex(0,1,norm*tau1*(v->epsParent(1).conj()));
  vertex(0,2,norm*tau1*(v->epsParent(2).conj()));
  vertex(1,0,norm*tau2*(v->epsParent(0).conj()));
  vertex(1,1,norm*tau2*(v->epsParent(1).conj()));
  vertex(1,2,norm*tau2*(v->epsParent(2).conj()));
  
  return;

}

