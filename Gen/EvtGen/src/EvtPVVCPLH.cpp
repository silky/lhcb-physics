//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1999      Caltech, UCSB
//
// Module: EvtPVVCPLH.cc
//
// Description: The decay of a scalar to two vector particles are 
//              performed with CP violation and different widths for
//              the CP-even and CP-odd states. E.g. Bs->J/psi phi.
//
// Modification history:
//
//    RYD       November 5, 1999       Module EvtSVVCPLH created
//
//    DUPREE    October 10, 2006       Large modification: EvtSVVCPLH->EvtPVVCPLH
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtIncoherentMixing.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenModels/EvtSVVHelAmp.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtPVVCPLH.hh"
#include "EvtGenBase/EvtId.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"
#include "EvtGenBase/EvtRandom.hh"

EvtPVVCPLH::~EvtPVVCPLH() {}

void EvtPVVCPLH::getName(std::string& model_name){

  model_name="PVV_CPLH";     

}


EvtDecayBase* EvtPVVCPLH::clone(){

  return new EvtPVVCPLH;

}

void EvtPVVCPLH::init(){

  // check that there are 8 arguments (deltaMs no argument anymore)
  checkNArg(8);
  checkNDaug(2);

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::VECTOR);

}

void EvtPVVCPLH::initProbMax(){

  //This is probably not quite right, but it should do as a start...
  //Anders

  setProbMax(2*(getArg(2)*getArg(2)+getArg(4)*getArg(4)+getArg(6)*getArg(6)));

}

void EvtPVVCPLH::decay( EvtParticle *p){

  //added by Lange Jan4,2000
  static EvtId BS0=EvtPDL::getId("B_s0");
  static EvtId BSB=EvtPDL::getId("anti-B_s0");

  //This is only to get tag-ID
  //Mixing is not relevant
  //Lifetime is made correctly later
  //Tristan
  EvtId other_b;
  double t;
  EvtIncoherentMixing::OtherB(p,t,other_b);
//EvtIncoherentMixing::OtherB(p,t,other_b,0.5); also possible 

  //Here we're gonna generate and set the "envelope" lifetime
  //So we take the longest living component (for positive deltaGamma: tauH)
  //The double exponent will be taken care of later, by the amplitudes
  //Tristan
  static double Gamma = EvtConst::c/(EvtPDL::getctau(BS0));
  static double deltaGamma = EvtIncoherentMixing::getdGammas();
  static double ctauLong = EvtConst::c/(Gamma-fabs(deltaGamma)/2);
  // if dG>0: tauLong=tauH(CP-odd) is then largest

  //This overrules the lifetimes made in OtherB
  t=-log(EvtRandom::Flat())*(ctauLong);//ctauLong has same dimensions as t
  if((EvtIncoherentMixing::isBsMixed(p))){
    p->getParent()->setLifetime(t);
  }else{
    p->setLifetime(t);
  }

  //These should be filled with the transversity amplitudes at t=0 //Tristan
  EvtComplex G0P,G1P,G1M;  
  G1P=EvtComplex(getArg(2)*cos(getArg(3)),getArg(2)*sin(getArg(3)));
  G0P=EvtComplex(getArg(4)*cos(getArg(5)),getArg(4)*sin(getArg(5)));
  G1M=EvtComplex(getArg(6)*cos(getArg(7)),getArg(6)*sin(getArg(7)));

  EvtComplex lambda_km=EvtComplex(cos(2*getArg(0)),sin(2*getArg(0)));

  //deltaMs is no argument anymore
  //Tristan
  static double deltaMs = EvtIncoherentMixing::getdeltams();
  double cdmt=cos(deltaMs*t/(2*EvtConst::c));
  double sdmt=sin(deltaMs*t/(2*EvtConst::c));

  EvtComplex cG0P,cG1P,cG1M;

  double mt = exp(-std::max(0.,deltaGamma)*t/(2*EvtConst::c));
  double pt = exp(+std::min(0.,deltaGamma)*t/(2*EvtConst::c));

  if (other_b==BSB){
    //These are the right equations for the transversity formalism
    //cGOP is de 0-component, CP-even, so lives shorter: lifetime tauL
    //cG1P is the //-component, also CP-even, also smaller exponent
    //cG1M is the transverse component, CP-odd, so has longer lifetime tauH
    //Tristan
    cG0P = mt*G0P*(cdmt+lambda_km*EvtComplex(0.0,getArg(1)*sdmt));
    cG1P = mt*G1P*(cdmt+lambda_km*EvtComplex(0.0,getArg(1)*sdmt));
    cG1M = pt*G1M*(cdmt-lambda_km*EvtComplex(0.0,getArg(1)*sdmt));
  } else if (other_b==BS0){
    //The equations for BsBar
    //Note: a minus-sign difference
    //Tristan
    cG0P = mt*G0P*(cdmt+(1.0/lambda_km)*EvtComplex(0.0,getArg(1)*sdmt));
    cG1P = mt*G1P*(cdmt+(1.0/lambda_km)*EvtComplex(0.0,getArg(1)*sdmt));
    cG1M =-pt*G1M*(cdmt-(1.0/lambda_km)*EvtComplex(0.0,getArg(1)*sdmt));
  } else{
    report(ERROR,"EvtGen") << "other_b was not BSB or BS0!"<<std::endl;
    ::abort();
  }

  EvtComplex A0,AP,AM;
  //Converting the transversity amplitudes
  //to helicity amplitudes
  //(to plug them into SVVHelAmp)
  A0=cG0P;
  AP=(cG1P+cG1M)/sqrt(2.0);
  AM=(cG1P-cG1M)/sqrt(2.0);
  
  EvtSVVHelAmp::SVVHel(p,_amp2,getDaug(0),getDaug(1),AP,A0,AM);

  return ;
}



