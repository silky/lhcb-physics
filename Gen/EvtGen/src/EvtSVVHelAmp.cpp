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
// Module: EvtSVVHelAmp.cc
//
// Description: Routine to decay scalar -> 2 vectors
//              by specifying the helicity amplitudes
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
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtVector4C.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtVector3C.hh"
#include "EvtGen/EvtVector3R.hh"
#include "EvtGen/EvtTensor3C.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtSVVHelAmp.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"

EvtSVVHelAmp::~EvtSVVHelAmp() {}

void EvtSVVHelAmp::getName(EvtString& model_name){

  model_name="SVV_HELAMP";     

}


EvtDecayBase* EvtSVVHelAmp::clone(){

  return new EvtSVVHelAmp;

}

void EvtSVVHelAmp::init(){

  // check that there are 6 arguments
  checkNArg(6);
  checkNDaug(2);

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::VECTOR);

}


void EvtSVVHelAmp::initProbMax(){

  setProbMax(3.0);

}


void EvtSVVHelAmp::decay( EvtParticle *p){

  SVVHel(p,_amp2,getDaug(0),getDaug(1),
	 EvtComplex(getArg(0)*cos(getArg(1)),getArg(0)*sin(getArg(1))),
	 EvtComplex(getArg(2)*cos(getArg(3)),getArg(2)*sin(getArg(3))),
	 EvtComplex(getArg(4)*cos(getArg(5)),getArg(4)*sin(getArg(5))));
				 
  return ;

}


void EvtSVVHelAmp::SVVHel(EvtParticle *parent,EvtAmp& amp,EvtId n_v1,EvtId n_v2,
               const EvtComplex& hp,const EvtComplex& h0,
               const EvtComplex& hm){

  //  Routine to decay a vector into a vector and scalar.  Started
  //  by ryd on Oct 17, 1996.
    
  int tndaug = 2;
  EvtId tdaug[2];
  tdaug[0] = n_v1;
  tdaug[1] = n_v2;


  parent->initializePhaseSpace(tndaug,tdaug);

  EvtParticle *v1,*v2;
  v1 = parent->getDaug(0);
  v2 = parent->getDaug(1);

  EvtVector4R momv1 = v1->getP4();

  EvtVector3R v1dir(momv1.get(1),momv1.get(2),momv1.get(3));
  v1dir=v1dir/v1dir.d3mag();

  EvtComplex a=-0.5*(hp+hm);
  EvtComplex b=EvtComplex(0.0,0.5)*(hp-hm);
  EvtComplex c=h0+0.5*(hp+hm);

  EvtTensor3C M=a*EvtTensor3C::id()+
                b*eps(v1dir)+
                c*directProd(v1dir,v1dir);

  EvtVector3C t0=M.cont1(v1->eps(0).vec().conj());
  EvtVector3C t1=M.cont1(v1->eps(1).vec().conj());
  EvtVector3C t2=M.cont1(v1->eps(2).vec().conj());

  EvtVector3C eps0=v2->eps(0).vec().conj();
  EvtVector3C eps1=v2->eps(1).vec().conj();
  EvtVector3C eps2=v2->eps(2).vec().conj();

  amp.vertex(0,0,t0*eps0);
  amp.vertex(0,1,t0*eps1);
  amp.vertex(0,2,t0*eps2);

  amp.vertex(1,0,t1*eps0);
  amp.vertex(1,1,t1*eps1);
  amp.vertex(1,2,t1*eps2);

  amp.vertex(2,0,t2*eps0);
  amp.vertex(2,1,t2*eps1);
  amp.vertex(2,2,t2*eps2);

  return ;

}


