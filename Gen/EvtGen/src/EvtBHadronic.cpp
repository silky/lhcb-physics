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
// Module: EvtBHadronic.cc
//
// Description: Model for hadronic B decays. Uses naive factorization. 
//
// Modification history:
//
//    RYD     June 14, 1997         Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtISGW2FF.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtISGW2FF.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtBHadronic.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtString.hh"

EvtBHadronic::~EvtBHadronic() {}

void EvtBHadronic::getName(EvtString& model_name){

  model_name="BHADRONIC";    

}


EvtDecayBase* EvtBHadronic::clone(){

  return new EvtBHadronic;

}


void EvtBHadronic::init(){

  // check that there are 2 argument
  checkNArg(2);


}

void EvtBHadronic::decay( EvtParticle *p){

  //added by Lange Jan4,2000
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId D0=EvtPDL::getId("D0");
  static EvtId DST0=EvtPDL::getId("D*0");
  static EvtId D3P10=EvtPDL::getId("D'_10");
  static EvtId D3P20=EvtPDL::getId("D_2*0");
  static EvtId D3P00=EvtPDL::getId("D_0*0");
  static EvtId D1P10=EvtPDL::getId("D_10");

  static EvtISGW2FF ffmodel;

  EvtParticle *pdaug[MAX_DAUG];
  
  EvtVector4R p4[MAX_DAUG];
  double mass[MAX_DAUG],m;

  // Prepare for phase space routine. 
 
  findMasses(p,getNDaug(),getDaugs(),mass);

  //  Need phase space random numbers

  m = p->mass();

  EvtGenKine::PhaseSpace( getNDaug(), mass, p4, m );

  int i,j;

  if (p->getNDaug()==0){
    p->makeDaughters(getNDaug(),getDaugs());
  }

  for(i=0;i<getNDaug();i++){

     pdaug[i]=p->getDaug(i);

   }

  

  for(i=0;i<getNDaug();i++){
    pdaug[i]->init( getDaugs()[i], p4[i] );
  }

  int bcurrent,wcurrent;
  int nbcurrent = 0,nwcurrent = 0;

  bcurrent=(int)getArg(0);
  wcurrent=(int)getArg(1);

  EvtVector4C jb[5],jw[5];
  EvtTensor4C g,tds;

  EvtVector4R p4b;
  p4b.set(p->mass(),0.0,0.0,0.0);

  EvtVector4R q;
  double q2;

  EvtComplex ep_meson_bb[5];
  double f,gf,ap,am;
  double fp,fm;
  double hf,kf,bp,bm;
  EvtVector4R pp,pm;
  EvtVector4C ep_meson_b[5];

  switch (bcurrent) {
  
  // D0
  case 1:
    q=p4b-p4[0];
    q2=q*q;
    nbcurrent=1;
    ffmodel.getscalarff(B0,D0,EvtPDL::getNominalMass(D0),
			EvtPDL::getNominalMass(getDaugs()[1]),&fp,&fm);
    jb[0]=EvtVector4C(fp*(p4b+p4[0])-fm*q);
    break;
    // D*
  case 2:
    q=p4b-p4[0];
    q2=q*q;
    nbcurrent=3;
    ffmodel.getvectorff(B0,DST0,EvtPDL::getNominalMass(DST0),q2,&f,&gf,&ap,&am);

    g.setdiag(1.0,-1.0,-1.0,-1.0);
    tds = -f*g 
      -ap*(directProd(p4b,p4b)+directProd(p4b,p4[0]))
      -gf*EvtComplex(0.0,1.0)*dual(directProd(p4[0]+p4b,p4b-p4[0]))
      -am*((directProd(p4b,p4b)-directProd(p4b,p4[0])));
    jb[0]=tds.cont1(p->getDaug(0)->epsParent(0).conj());
    jb[1]=tds.cont1(p->getDaug(0)->epsParent(1).conj());
    jb[2]=tds.cont1(p->getDaug(0)->epsParent(2).conj());
    break;
    // D1
  case 3:
    q=p4b-p4[0];
    q2=q*q;
    nbcurrent=3;
    ffmodel.getvectorff(B0,D3P10,EvtPDL::getNominalMass(D3P10),q2,&f,&gf,&ap,&am);

    g.setdiag(1.0,-1.0,-1.0,-1.0);
    tds = -f*g 
      -ap*(directProd(p4b,p4b)+directProd(p4b,p4[0]))
      -gf*EvtComplex(0.0,1.0)*dual(directProd(p4[0]+p4b,p4b-p4[0]))
      -am*((directProd(p4b,p4b)-directProd(p4b,p4[0])));
    jb[0]=tds.cont1(p->getDaug(0)->epsParent(0).conj());
    jb[1]=tds.cont1(p->getDaug(0)->epsParent(1).conj());
    jb[2]=tds.cont1(p->getDaug(0)->epsParent(2).conj());

    break;
    // D2*
  case 4:
    q=p4b-p4[0];
    q2=q*q;
    nbcurrent=5;
    ffmodel.gettensorff(B0,D3P20,EvtPDL::getNominalMass(D3P20),q2,&hf,&kf,&bp,&bm);
    g.setdiag(1.0,-1.0,-1.0,-1.0);
    
    ep_meson_b[0] = ((p->getDaug(0)->epsTensorParent(0)).cont2(p4b)).conj();
    ep_meson_b[1] = ((p->getDaug(0)->epsTensorParent(1)).cont2(p4b)).conj();
    ep_meson_b[2] = ((p->getDaug(0)->epsTensorParent(2)).cont2(p4b)).conj();
    ep_meson_b[3] = ((p->getDaug(0)->epsTensorParent(3)).cont2(p4b)).conj();
    ep_meson_b[4] = ((p->getDaug(0)->epsTensorParent(4)).cont2(p4b)).conj();
  
    pp=p4b+p4[0];
    pm=p4b-p4[0];
    
    ep_meson_bb[0]=ep_meson_b[0]*(p4b);
    ep_meson_bb[1]=ep_meson_b[1]*(p4b);
    ep_meson_bb[2]=ep_meson_b[2]*(p4b);
    ep_meson_bb[3]=ep_meson_b[3]*(p4b);
    ep_meson_bb[4]=ep_meson_b[4]*(p4b);
    
    jb[0]=EvtComplex(0.0,(1.0)*hf)*dual(directProd(pp,pm)).cont2(ep_meson_b[0])
       -kf*ep_meson_b[0]
       -bp*ep_meson_bb[0]*pp-bm*ep_meson_bb[0]*pm;
     
    jb[1]=EvtComplex(0.0,(1.0)*hf)*dual(directProd(pp,pm)).cont2(ep_meson_b[1])
       -kf*ep_meson_b[1]
       -bp*ep_meson_bb[1]*pp-bm*ep_meson_bb[1]*pm;
    
    jb[2]=EvtComplex(0.0,(1.0)*hf)*dual(directProd(pp,pm)).cont2(ep_meson_b[2])
       -kf*ep_meson_b[2]
       -bp*ep_meson_bb[2]*pp-bm*ep_meson_bb[2]*pm;
    
    jb[3]=EvtComplex(0.0,(1.0)*hf)*dual(directProd(pp,pm)).cont2(ep_meson_b[3])
       -kf*ep_meson_b[3]
       -bp*ep_meson_bb[3]*pp-bm*ep_meson_bb[3]*pm;
    
    jb[4]=EvtComplex(0.0,(1.0)*hf)*dual(directProd(pp,pm)).cont2(ep_meson_b[4])
       -kf*ep_meson_b[4]
       -bp*ep_meson_bb[4]*pp-bm*ep_meson_bb[4]*pm;
    break;
    // D_0*
  case 5:
    q=p4b-p4[0];
    q2=q*q;
    double f,gf,ap,am;
    nbcurrent=3;
    ffmodel.getvectorff(B0,D1P10,EvtPDL::getNominalMass(D1P10),q2,&f,&gf,&ap,&am);
    g.setdiag(1.0,-1.0,-1.0,-1.0);
    tds = -f*g 
      -ap*(directProd(p4b,p4b)+directProd(p4b,p4[0]))
      +gf*EvtComplex(0.0,1.0)*dual(directProd(p4[0]+p4b,p4b-p4[0]))
      -am*((directProd(p4b,p4b)-directProd(p4b,p4[0])));
    jb[0]=tds.cont1(p->getDaug(0)->epsParent(0).conj());
    jb[1]=tds.cont1(p->getDaug(0)->epsParent(1).conj());
    jb[2]=tds.cont1(p->getDaug(0)->epsParent(2).conj());
    break;
    // D_1
  case 6:
    q=p4b-p4[0];
    q2=q*q;
    nbcurrent=1;
    ffmodel.getscalarff(B0,D3P00,EvtPDL::getNominalMass(D3P00),q2,&fp,&fm);
    jb[0]=fp*(p4b+p4[0])+fm*q;
    break;
  default:
    report(ERROR,"EvtGen")<<"In EvtBHadronic, unknown hadronic current."<<std::endl;
    
  }  

  double norm;

  switch (wcurrent) {


  case 1: case 3: case 4:
    nwcurrent=1;
    jw[0]=p4[getNDaug()-1];
    break;

  case 2: case 5: case 6:
    nwcurrent=3;
    norm=1.0/sqrt(p4[1].get(0)*p4[1].get(0)/p4[1].mass2()-1.0);
    jw[0]=norm*p->getDaug(getNDaug()-1)->epsParent(0);
    jw[1]=norm*p->getDaug(getNDaug()-1)->epsParent(1);
    jw[2]=norm*p->getDaug(getNDaug()-1)->epsParent(2);
    break;


  default:
    report(ERROR,"EvtGen")<<"In EvtBHadronic, unknown W current."<<std::endl;

  }  

  if (nbcurrent==1&&nwcurrent==1){
    vertex(jb[0]*jw[0]);
    return;
  }

  if (nbcurrent==1){
    //report(INFO,"EvtGen") << "Amplitudes:";
    //EvtComplex a=0.0;
    for(j=0;j<nwcurrent;j++){
      //report(INFO,"EvtGen") << jb[0]*jw[j] << " ";
      //a+=(jb[0]*jw[j]*jb[0]*jw[j]);
      vertex(j,jb[0]*jw[j]);
    }
    //report(INFO,"EvtGen") << " prob:"<<a<<" mass:"<<p4[1].mass()<< std::endl;
    return;
  }

  if (nwcurrent==1){
    for(j=0;j<nbcurrent;j++){
      vertex(j,jb[j]*jw[0]);
    }
    return;
  }

  for(j=0;j<nbcurrent;j++){
    for(i=0;i<nwcurrent;i++){
      vertex(j,i,jb[j]*jw[i]);
    }
  }
  return;

}







