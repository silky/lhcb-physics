//--------------------------------------------------------------------------
//
//
// Copyright Information: See EvtGen/COPYRIGHT
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
//
// Module: EvtGen/EvtBtoXsgamma.hh
//
// Description:
// Class to generate non-resonant two-body b->s,gamma decays.
//
// Modification history:
//
//    Mark Ian Williams       July 20, 2000       Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOXSGAMMA_HH
#define EVTBTOXSGAMMA_HH

#include "EvtGen/EvtDecayIncoherent.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtBtoXsgammaAbsModel.hh"

class EvtBtoXsgamma:public  EvtDecayIncoherent  {

public:
  
  EvtBtoXsgamma() {};

  virtual ~EvtBtoXsgamma();

  void getName(EvtString& name);

  EvtDecayBase* clone();

  void initProbMax();

  void init();

  void decay(EvtParticle *p);

private:

  EvtBtoXsgammaAbsModel *_model;
 
};

#endif

