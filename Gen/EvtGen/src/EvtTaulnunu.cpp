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
// Module: EvtTaulnunu.cc
//
// Description: The leptonic decay of the tau meson.
//              E.g., tau- -> e- nueb nut
//
// Modification history:
//
//    RYD       January 17, 1997       Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include <iostream.h>
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtTaulnunu.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtReport.hh"

EvtTaulnunu::~EvtTaulnunu() {}

void EvtTaulnunu::getName(EvtString& model_name){

  model_name="TAULNUNU";     

}


EvtDecayBase* EvtTaulnunu::clone(){

  return new EvtTaulnunu;

}

void EvtTaulnunu::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(3);

  checkSpinParent(EvtSpinType::DIRAC);

  checkSpinDaughter(0,EvtSpinType::DIRAC);
  checkSpinDaughter(1,EvtSpinType::NEUTRINO);
  checkSpinDaughter(2,EvtSpinType::NEUTRINO);

}

void EvtTaulnunu::initProbMax(){

  setProbMax(650.0);

}

void EvtTaulnunu::decay(EvtParticle *p){
  static EvtId TAUM=EvtPDL::getId("tau-");

  p->initializePhaseSpace(getNDaug(),getDaugs());

  EvtParticle *l, *nul, *nut;

  l = p->getDaug(0);
  nul = p->getDaug(1);
  nut = p->getDaug(2);
  
  EvtVector4C l1, l2, tau1, tau2;

  if (p->getId()==TAUM) {

    tau1=EvtLeptonVACurrent(nut->spParentNeutrino(),p->sp(0));
    tau2=EvtLeptonVACurrent(nut->spParentNeutrino(),p->sp(1));
    l1=EvtLeptonVACurrent(l->spParent(0),nul->spParentNeutrino());
    l2=EvtLeptonVACurrent(l->spParent(1),nul->spParentNeutrino());

  }
  else{
    tau1=EvtLeptonVACurrent(p->sp(0),nut->spParentNeutrino());
    tau2=EvtLeptonVACurrent(p->sp(1),nut->spParentNeutrino());
    l1=EvtLeptonVACurrent(nul->spParentNeutrino(),l->spParent(0));
    l2=EvtLeptonVACurrent(nul->spParentNeutrino(),l->spParent(1));
  }

  vertex(0,0,tau1*l1);
  vertex(0,1,tau1*l2);
  vertex(1,0,tau2*l1);
  vertex(1,1,tau2*l2);
  
  return;

}

