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
// Module: EvtSSSCP.cc
//
// Description: Routine to decay scalar -> 2 scalars
//
// Modification history:
//
//    RYD       November 24, 1996       Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtCPUtil.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtSSSCP.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtConst.hh"

EvtSSSCP::~EvtSSSCP() {}

void EvtSSSCP::getName(EvtString& model_name){

  model_name="SSS_CP";     

}


EvtDecayBase* EvtSSSCP::clone(){

  return new EvtSSSCP;

}

void EvtSSSCP::init(){

  // check that there are 7 arguments
  checkNArg(7);
  checkNDaug(2);
  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);

}

void EvtSSSCP::initProbMax(){

  //This is probably not quite right, but it should do as a start...
  //Anders

  setProbMax(2*(getArg(3)*getArg(3)+getArg(5)*getArg(5)));

}

void EvtSSSCP::decay( EvtParticle *p ){

  //added by Lange Jan4,2000
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");

  double t;
  EvtId other_b;

  EvtCPUtil::OtherB(p,t,other_b);

  p->initializePhaseSpace(getNDaug(),getDaugs());


  EvtComplex amp;

  EvtComplex A,Abar;
  
  A=EvtComplex(getArg(3)*cos(getArg(4)),getArg(3)*sin(getArg(4)));
  Abar=EvtComplex(getArg(5)*cos(getArg(6)),getArg(5)*sin(getArg(6)));
   
  if (other_b==B0B){
    amp=A*cos(getArg(1)*t/(2*EvtConst::c))+
      EvtComplex(cos(-2.0*getArg(0)),sin(-2.0*getArg(0)))*
      getArg(2)*EvtComplex(0.0,1.0)*Abar*sin(getArg(1)*t/(2*EvtConst::c));
  }
  if (other_b==B0){
    amp=A*EvtComplex(cos(2.0*getArg(0)),sin(2.0*getArg(0)))*
      EvtComplex(0.0,1.0)*sin(getArg(1)*t/(2*EvtConst::c))+       
      getArg(2)*Abar*cos(getArg(1)*t/(2*EvtConst::c));
  }
  
  vertex(amp);
  
  return ;
}

