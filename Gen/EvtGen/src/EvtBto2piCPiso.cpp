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
// Module: EvtBto2piCPiso.cc
//
// Description: Routine to decay B -> pi pi with isospin amplitudes 
//
// Modification history:
//
//    RYD,NK     Febuary 7, 1998         Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtRandom.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtCPUtil.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtBto2piCPiso.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtConst.hh"

EvtBto2piCPiso::~EvtBto2piCPiso() {}

void EvtBto2piCPiso::getName(EvtString& model_name){

  model_name="BTO2PI_CP_ISO";     

}


EvtDecayBase* EvtBto2piCPiso::clone(){

  return new EvtBto2piCPiso;

}

void EvtBto2piCPiso::init(){

  // check that there are 11 arguments

  checkNArg(11);
  checkNDaug(2);

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);

}


void EvtBto2piCPiso::initProbMax() {

  //added by Lange Jan4,2000
  static EvtId PI0=EvtPDL::getId("pi0");
  static EvtId PIP=EvtPDL::getId("pi+");
  static EvtId PIM=EvtPDL::getId("pi-");

//this may need to be revised

if (((getDaugs()[0]==PIP) && (getDaugs()[1]==PIM)) || ((getDaugs()[0]==PIM) && (getDaugs()[1]==PIP))) {
   setProbMax(4.0*(getArg(2)*getArg(2)+getArg(4)*getArg(4)));
}

if ((getDaugs()[0]==PI0) && (getDaugs()[1]==PI0)) {
   setProbMax(2.0*(4.0*getArg(2)*getArg(2)+getArg(4)*getArg(4)));
}

if (((getDaugs()[0]==PIP) && (getDaugs()[1]==PI0)) || ((getDaugs()[0]==PI0) && (getDaugs()[1]==PIP))) {
   setProbMax(6.0*getArg(2)*getArg(2));
}

if (((getDaugs()[0]==PI0) && (getDaugs()[1]==PIM)) || ((getDaugs()[0]==PIM) && (getDaugs()[1]==PI0))) {
   setProbMax(6.0*getArg(4)*getArg(4));
}

}

void EvtBto2piCPiso::decay( EvtParticle *p ){

  //added by Lange Jan4,2000
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");
  static EvtId PI0=EvtPDL::getId("pi0");
  static EvtId PIP=EvtPDL::getId("pi+");
  static EvtId PIM=EvtPDL::getId("pi-");

  double t;
  EvtId other_b;
  int charged;

//randomly generate the tag (B0 or B0B) 

  double tag = EvtRandom::Flat(0.0,1.0);
  if (tag < 0.5) {

   EvtCPUtil::OtherB(p,t,other_b,1.0);
   other_b = B0;
  }
  else {
   
   EvtCPUtil::OtherB(p,t,other_b,0.0);
   other_b = B0B;
  }

  EvtParticle *s1,*s2;

  p->makeDaughters(getNDaug(),getDaugs());
  s1=p->getDaug(0);
  s2=p->getDaug(1);

  double m_parent,mass[2];
  EvtVector4R p4[2];

  m_parent = p->mass();

  findMasses(p,getNDaug(),getDaugs(),mass);

//  Need phase space random numbers

  EvtGenKine::PhaseSpace( getNDaug(), mass, p4, m_parent );

//  Put phase space results into the daughters.

   s1->init( getDaugs()[0], p4[0] );
   s2->init( getDaugs()[1], p4[1] );

   EvtComplex amp;

   EvtComplex A,Abar;
   EvtComplex A2, A2_bar, A0, A0_bar;

   A2 = EvtComplex(getArg(2)*cos(getArg(3)),getArg(2)*sin(getArg(3)));
   A2_bar = EvtComplex(getArg(4)*cos(getArg(5)),getArg(4)*sin(getArg(5)));

   A0 = EvtComplex(getArg(6)*cos(getArg(7)),getArg(6)*sin(getArg(7)));
   A0_bar = EvtComplex(getArg(8)*cos(getArg(9)),getArg(8)*sin(getArg(9)));

//depending on what combination of pi pi we have, there will be different 
//A and Abar

if (((getDaugs()[0]==PIP) && (getDaugs()[1]==PI0)) || ((getDaugs()[0]==PI0) && (getDaugs()[1]==PIP))) {

//pi+ pi0, so just A_2
  
    charged = 1;
    A = 3.0*A2;
  
  }

if (((getDaugs()[0]==PI0) && (getDaugs()[1]==PIM)) || ((getDaugs()[0]==PIM) && (getDaugs()[1]==PI0))) {

//pi- pi0, so just A2_bar

    charged = 1;  
    A = 3.0*A2_bar;

  }

if (((getDaugs()[0]==PIP) && (getDaugs()[1]==PIM)) || ((getDaugs()[0]==PIM) && (getDaugs()[1]==PIP))) {

//pi+ pi-, so A_2 - A_0

   charged = 0;
   A=sqrt(2.0)*(A2-A0);
   Abar=sqrt(2.0)*(A2_bar-A0_bar);
  }

  if ((getDaugs()[0]==PI0) && (getDaugs()[1]==PI0)) {

//pi0 pi0, so 2*A_2 + A_0

    charged = 0;
    A=2.0*A2 + A0;
    Abar=2.0*A2_bar + A0_bar;
  }

if(charged == 0) {

   if (other_b==B0B){
     amp=A*cos(getArg(1)*t/(2*EvtConst::c))+
       EvtComplex(cos(-2.0*getArg(0)),sin(-2.0*getArg(0)))*
       EvtComplex(0.0,1.0)*Abar*sin(getArg(1)*t/(2*EvtConst::c));
   }
   if (other_b==B0){
     amp=A*EvtComplex(cos(2.0*getArg(0)),sin(2.0*getArg(0)))*
       EvtComplex(0.0,1.0)*sin(getArg(1)*t/(2*EvtConst::c))+       
       Abar*cos(getArg(1)*t/(2*EvtConst::c));
   }
}
else amp = A;

   vertex(amp);

  return ;
}

