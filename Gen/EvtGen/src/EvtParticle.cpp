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
// Module: EvtParticle.cc
//
// Description: Class to describe all particles
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 
#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <iostream>
#include <math.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
#include <strstream>
#else
#include <sstream>
#endif
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRadCorr.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtDecayTable.hh"
#include "EvtGenBase/EvtDiracParticle.hh"
#include "EvtGenBase/EvtScalarParticle.hh"
#include "EvtGenBase/EvtVectorParticle.hh"
#include "EvtGenBase/EvtTensorParticle.hh"
#include "EvtGenBase/EvtPhotonParticle.hh"
#include "EvtGenBase/EvtNeutrinoParticle.hh"
#include "EvtGenBase/EvtStringParticle.hh"
#include "EvtGenBase/EvtStdHep.hh"
#include "EvtGenBase/EvtSecondary.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtCPUtil.hh"
#include "EvtGenBase/EvtParticleFactory.hh"


EvtParticle::~EvtParticle() {}

EvtParticle::EvtParticle() {
   _ndaug=0;
   _parent=0;
   _channel=-10;
   _t=0.0;
   _genlifetime=1;
   _first=1;
   _isInit=false;
   _validP4=false;
   _isDecayed=false;
   //   _mix=false;
}

void EvtParticle::setFirstOrNot() {
  _first=0;
}
void EvtParticle::resetFirstOrNot() {
  _first=1;
}

void EvtParticle::setChannel( int i ) { 
  _channel=i;
}

EvtParticle *EvtParticle::getDaug(int i) { return _daug[i]; }

EvtParticle *EvtParticle::getParent() { return _parent;}

void EvtParticle::setLifetime(double tau){
  _t=tau;
}

void EvtParticle::setLifetime(){
  if (_genlifetime){
    _t=-log(EvtRandom::Flat())*EvtPDL::getctau(getId());
  }
}

double EvtParticle::getLifetime(){

  return _t;
}

void EvtParticle::addDaug(EvtParticle *node) {
  node->_daug[node->_ndaug++]=this;
  _ndaug=0;
  _parent=node; 
}


int EvtParticle::firstornot() const { return _first;}

EvtId EvtParticle::getId() const { return _id;}

EvtSpinType::spintype EvtParticle::getSpinType() const 
      { return EvtPDL::getSpinType(_id);}

int EvtParticle::getSpinStates() const 
  { return EvtSpinType::getSpinStates(EvtPDL::getSpinType(_id));}

const EvtVector4R& EvtParticle::getP4() const { return _p;}

int EvtParticle::getChannel() const { return _channel;}

int EvtParticle::getNDaug() const { return _ndaug;}

double EvtParticle::mass() const {

     return _p.mass();
}


void EvtParticle::setDiagonalSpinDensity(){

  _rhoForward.SetDiag(getSpinStates());
}

void EvtParticle::setVectorSpinDensity(){

  if (getSpinStates()!=3) {
    report(ERROR,"EvtGen")<<"Error in EvtParticle::setVectorSpinDensity"
                          <<std::endl;
    report(ERROR,"EvtGen")<<"spin_states:"<<getSpinStates()<<std::endl;
    report(ERROR,"EvtGen")<<"particle:"<<EvtPDL::name(_id).c_str()<<std::endl;
    ::abort();
  }

  EvtSpinDensity rho;

  //Set helicity +1 and -1 to 1.
  rho.SetDiag(getSpinStates());
  rho.Set(1,1,EvtComplex(0.0,0.0));

  setSpinDensityForwardHelicityBasis(rho);

}


void EvtParticle::setSpinDensityForwardHelicityBasis(const EvtSpinDensity& rho)
{

  EvtSpinDensity R=rotateToHelicityBasis();

  assert(R.GetDim()==rho.GetDim());

  int n=rho.GetDim();

  _rhoForward.SetDim(n);

  int i,j,k,l;

  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      EvtComplex tmp=0.0;
      for(k=0;k<n;k++){
	for(l=0;l<n;l++){
	  tmp+=R.Get(l,i)*rho.Get(l,k)*conj(R.Get(k,j));
	}
      }
      _rhoForward.Set(i,j,tmp);
    }
  }

  //report(INFO,"EvtGen") << "_rhoForward:"<<_rhoForward<<std::endl;

}

void EvtParticle::setSpinDensityForwardHelicityBasis(const EvtSpinDensity& rho,
						     double alpha,
						     double beta,
						     double gamma){

  EvtSpinDensity R=rotateToHelicityBasis(alpha,beta,gamma);

  assert(R.GetDim()==rho.GetDim());

  int n=rho.GetDim();

  _rhoForward.SetDim(n);

  int i,j,k,l;

  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      EvtComplex tmp=0.0;
      for(k=0;k<n;k++){
	for(l=0;l<n;l++){
	  tmp+=R.Get(l,i)*rho.Get(l,k)*conj(R.Get(k,j));
	}
      }
      _rhoForward.Set(i,j,tmp);
    }
  }

  //report(INFO,"EvtGen") << "_rhoForward:"<<_rhoForward<<std::endl;

}

void EvtParticle::initDecay(bool useMinMass) {

  EvtParticle* p=this;
  // carefull - the parent mass might be fixed in stone..
  EvtParticle* par=p->getParent();
  double parMass=-1.;
  if ( par ) {
    if ( par->hasValidP4() ) parMass=par->mass();
    int i;
    for ( i=0;i<par->getNDaug();i++) {
      EvtParticle *tDaug=par->getDaug(i);
      if ( p != tDaug )
        parMass-=EvtPDL::getMinMass(tDaug->getId());
    }
  }

  if ( _isInit ) {
    //we have already been here - just reroll the masses!
    if ( _ndaug>0) {
      int ii;
      for(ii=0;ii<_ndaug;ii++){
        if ( EvtPDL::getWidth(p->getDaug(ii)->getId()) > 0.0000001)
          p->getDaug(ii)->initDecay(useMinMass);
        else p->getDaug(ii)->
               setMass(EvtPDL::getMeanMass(p->getDaug(ii)->getId()));
      }
    }
    
    int j;
    EvtId *dauId=0;
    double *dauMasses=0;
    if ( _ndaug > 0) {
      dauId=new EvtId[_ndaug];
      dauMasses=new double[_ndaug];
      for (j=0;j<_ndaug;j++) { 
        dauId[j]=p->getDaug(j)->getId();
        dauMasses[j]=p->getDaug(j)->mass();
      }
    }
    EvtId *parId=0;
    EvtId *othDauId=0;
    EvtParticle *tempPar=p->getParent();
    if (tempPar) {
      parId=new EvtId(tempPar->getId());
      if ( tempPar->getNDaug()==2 ) {
        if ( tempPar->getDaug(0) == this ) 
          othDauId=
            new EvtId(tempPar->getDaug(1)->getId());
        else othDauId=new EvtId(tempPar->getDaug(0)->getId());
      }
    }
    if ( p->getParent() && _validP4==false ) {
      if ( !useMinMass ) 
        p->setMass(EvtPDL::getRandMass(
                                       p->getId(),parId,_ndaug,dauId,
                                       othDauId,parMass,dauMasses));
      else p->setMass(EvtPDL::getMinMass(p->getId()));
    }
    if ( parId) delete parId;
    if ( othDauId) delete othDauId;
    if ( dauId) delete [] dauId;
    if ( dauMasses) delete [] dauMasses;
    return;
  }  

  EvtDecayBase *decayer;
  decayer = EvtDecayTable::GetDecayFunc( p );

  if ( decayer ) {
    p->makeDaughters( decayer->nRealDaughters() , decayer->getDaugs() ) ;
    //report(INFO,"EvtGen") << "has inited " << p << std::endl;
    //then loop over the daughters and init their decay
    int i ;
    for( i=0 ; i<_ndaug ; i++ ) {
      if ( EvtPDL::getWidth(p->getDaug(i)->getId()) > 0.0000001)
        p->getDaug(i)->initDecay(useMinMass);
      else p->getDaug(i)->setMass(EvtPDL::getMeanMass(p->getDaug(i)->getId()));
      //report(INFO,"EvtGen") << "has inited " << p->getDaug(i) << std::endl;
    }
  }
  //figure masses in separate step...
  //  if ( p->getParent() && _validP4==false ) EvtDecayBase::findMass(p);
  
  int j;
  EvtId *dauId=0;
  double *dauMasses=0;
  if ( _ndaug > 0) {
    dauId=new EvtId[_ndaug];
    dauMasses=new double[_ndaug];
    for (j=0;j<_ndaug;j++) { 
      dauId[j]=p->getDaug(j)->getId();
      dauMasses[j]=p->getDaug(j)->mass();
    }
  }
  
  EvtId *parId=0;
  EvtId *othDauId=0;
  EvtParticle *tempPar=p->getParent();
  if (tempPar) {
    parId=new EvtId(tempPar->getId());
    if ( tempPar->getNDaug()==2 ) {
      if ( tempPar->getDaug(0) == this ) 
        othDauId= new EvtId(tempPar->getDaug(1)->getId());
      else othDauId=new EvtId(tempPar->getDaug(0)->getId());
    }
  }
  if ( p->getParent() && _validP4==false ) {
    if ( !useMinMass ) p->setMass(EvtPDL::getRandMass(p->getId(),
                                                      parId,_ndaug,dauId,
                                                      othDauId,parMass,
                                                      dauMasses));
    else p->setMass(EvtPDL::getMinMass(p->getId()));
  }
  if ( parId) delete parId;
  if ( othDauId) delete othDauId;
  if ( dauId) delete [] dauId;
  if ( dauMasses) delete [] dauMasses;
  _isInit=true;
}


void EvtParticle::decay(){
  //P is particle to decay, typically 'this' but sometime
  //modified by mixing 
  EvtParticle* p=this;

  //Will include effects of mixing here
  //added by Lange Jan4,2000
  static EvtId BS0=EvtPDL::getId("B_s0");
  static EvtId BSB=EvtPDL::getId("anti-B_s0");  
  
  if ( ( getId()==BS0 || getId()==BSB ) && ( ! isBsMixed() ) ) {
    double t;
    int mix;
    EvtCPUtil::incoherentMix(getId(), t, mix);
    setLifetime(t);
    
    if (mix) {
      
      EvtScalarParticle* scalar_part;
      
      scalar_part=new EvtScalarParticle;
      if (getId()==BS0) {
        EvtVector4R p_init(EvtPDL::getMass(BSB),0.0,0.0,0.0);
        scalar_part->init(BSB,p_init);
      }
      else{
        EvtVector4R p_init(EvtPDL::getMass(BS0),0.0,0.0,0.0);
        scalar_part->init(BS0,p_init);
      }
      
      scalar_part->setLifetime(0);
      
      scalar_part->setDiagonalSpinDensity();      
      
      insertDaugPtr(0,scalar_part);
      
      _ndaug=1;
      
      p=scalar_part;      
    }
  }

  //Did it mix?
  //if ( p->getMixed() ) {
    //should take C(p) - this should only
    //happen the first time we call decay for this
    //particle
  //p->takeCConj();
  // p->setUnMixed();
  //}

  EvtDecayBase *decayer;
  decayer = EvtDecayTable::GetDecayFunc(p);
  
  //  if ( decayer ) {
  //    report(INFO,"EvtGen") << "calling decay for " 
  //<< EvtPDL::name(p->getId()) << " " << p->mass() << " " << p->getP4() 
  //<< " " << p->getNDaug() << " " << p << std::endl;
  //    report(INFO,"EvtGen") << "NDaug= " << decayer->getNDaug() << std::endl;
  //    int ti;
  //    for ( ti=0; ti<decayer->getNDaug(); ti++) 
  //      report(INFO,"EvtGen") << "Daug " << ti << " " 
  //<< EvtPDL::name(decayer->getDaug(ti)) << std::endl;
  //  }
  //if (p->_ndaug>0) {
  //      report(INFO,"EvtGen") 
  //<<"Is decaying particle with daughters!!!!!"<<std::endl;
  //     ::abort();
    //return;
    //call initdecay first - April 29,2002 - Lange
  //}

  //if there are already daughters, then this step is already done!
  // figure out the masses
  if ( p->getNDaug() == 0 ) p->generateMassTree();

  //now we have accepted a set of masses - time
  if ( decayer ) {
    decayer->makeDecay(p);
  }
  else{
    p->_rhoBackward.SetDiag(p->getSpinStates());
  }
  _isDecayed=true; p->_isDecayed = true ;
  return;

}

void EvtParticle::generateMassTree() {
  double massProb=1.;
  double ranNum=2.;
  int counter=0;
  EvtParticle *p=this;
  while (massProb<ranNum) {
    //check it out the first time.
    p->initDecay();
    //report(INFO,"EvtGen") << "calling massProb \n";
    massProb=p->compMassProb();
    ranNum=EvtRandom::Flat();
    //report(INFO,"EvtGen") << "end of iter " << massProb << std::endl;
    counter++;

    if ( counter > 10000 ) {
      if ( counter == 10001 ) {
	report(INFO,"EvtGen") 
    << "Too many iterations to determine the mass tree. Parent mass= "
    << p->mass() << " " << massProb <<std::endl;
	p->printTree();
	report(INFO,"EvtGen") << "will take next combo with non-zero likelihood\n"; 
      }
      if ( massProb>0. ) massProb=2.0;
      if ( counter > 20000 ) {
	// one last try - take the minimum masses
	p->initDecay(true);
	massProb=p->compMassProb();
	if ( massProb>0. ) {
	  massProb=2.0;
	  report(INFO,"EvtGen") 
      << "Taking the minimum mass of all particles in the chain\n";
	}
	else {
	  report(INFO,"EvtGen") 
      << "Sorry, no luck finding a valid set of masses. "
      << "This may be a pathological combo\n";
	  assert(0);
	}
      }
    }
  }
  //report(INFO,"EvtGen") << counter << std::endl;
    //p->printTree();
}

double EvtParticle::compMassProb() {

  EvtParticle *p=this;
  //report(INFO,"EvtGen") << "compMassProb " << std::endl;
  //p->printTree();
  double mass=p->mass();
  double parMass=0.;
  if ( p->getParent()) { 
    parMass=p->getParent()->mass();
  }

  int nDaug=p->getNDaug();
  double *dMasses=0;

  int i;
  if ( nDaug>0 ) {
    dMasses=new double[nDaug];
    for (i=0; i<nDaug; i++) dMasses[i]=p->getDaug(i)->mass();
  }

  double temp=1.0;
  temp=EvtPDL::getMassProb(p->getId(), mass, parMass, nDaug, dMasses);
  //report(INFO,"EvtGen") << temp << " " 
  //<< EvtPDL::name(p->getId()) << std::endl;
  //If the particle already has a mass, we dont need to include
  //it in the probability calculation
  if ( (!p->getParent() || _validP4 ) && temp>0.0 ) temp=1.; 

  delete [] dMasses;
  // if ( temp < 0.9999999 ) 
  for (i=0; i<nDaug; i++) {
    temp*=p->getDaug(i)->compMassProb();
  }
  return temp;
}

void EvtParticle::deleteDaughters(bool keepChannel){
  int i;

  for(i=0;i<_ndaug;i++){
    _daug[i]->deleteTree();
  }
  
  _ndaug=0;
  //if ( keepChannel ) report(INFO,"EvtGen") << "keeping \n";
  if ( !keepChannel) _channel=-10;
  //_t=0.0;
  //_genlifetime=1;
  _first=1;
  _isInit=false;
  //report(INFO,"EvtGen") << "calling deletedaughters " 
  //<< EvtPDL::name(this->getId()) <<std::endl;
}

void EvtParticle::deleteTree(){
  //  int i;

  this->deleteDaughters();
  
  delete this;
  
}

EvtVector4C EvtParticle::epsParent(int i) const {
  EvtVector4C temp;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th polarization vector."
			 <<" I.e. you thought it was a"
			 <<" vector particle!" << std::endl;
  ::abort();
  return temp;
}

EvtVector4C EvtParticle::eps(int i) const {
  EvtVector4C temp;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th polarization vector."
			 <<" I.e. you thought it was a"
			 <<" vector particle!" << std::endl;
  ::abort();
  return temp;
}

EvtVector4C EvtParticle::epsParentPhoton(int i){
  EvtVector4C temp;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th polarization vector of photon."
			 <<" I.e. you thought it was a"
			 <<" photon particle!" << std::endl;
  ::abort();
  return temp;
}

EvtVector4C EvtParticle::epsPhoton(int i){
  EvtVector4C temp;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th polarization vector of a photon."
			 <<" I.e. you thought it was a"
			 <<" photon particle!" << std::endl;
  ::abort();
  return temp;
}

EvtDiracSpinor EvtParticle::spParent(int i) const {
  EvtDiracSpinor tempD;
  int temp;
  temp = i;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th dirac spinor."
			 <<" I.e. you thought it was a"
			 <<" Dirac particle!" << std::endl;
  ::abort();
  return tempD;
}

EvtDiracSpinor EvtParticle::sp(int i) const {
  EvtDiracSpinor tempD;
  int temp;
  temp = i;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th dirac spinor."
			 <<" I.e. you thought it was a"
			 <<" Dirac particle!" << std::endl;
  ::abort();
  return tempD;
}

EvtDiracSpinor EvtParticle::spParentNeutrino() const {
  EvtDiracSpinor tempD;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the "
			 <<"dirac spinor."
			 <<" I.e. you thought it was a"
			 <<" neutrino particle!" << std::endl;
  ::abort();
  return tempD;
}

EvtDiracSpinor EvtParticle::spNeutrino() const {
  EvtDiracSpinor tempD;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the "
			 <<"dirac spinor."
			 <<" I.e. you thought it was a"
			 <<" neutrino particle!" << std::endl;
  ::abort();
  return tempD;
}

EvtTensor4C EvtParticle::epsTensorParent(int i) const {
  int temp;
  temp = i;
  EvtTensor4C tempC; 
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th tensor."
			 <<" I.e. you thought it was a"
			 <<" Tensor particle!" << std::endl;
  ::abort();
  return tempC;
}

EvtTensor4C EvtParticle::epsTensor(int i) const {
  int temp;
  temp = i;
  EvtTensor4C tempC; 
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th tensor."
			 <<" I.e. you thought it was a"
			 <<" Tensor particle!" << std::endl;
  ::abort();
  return tempC;
}

EvtVector4R EvtParticle::getP4Lab() {
  EvtVector4R temp,mom;
  EvtParticle *ptemp;
  
  temp=this->getP4();
  ptemp=this;
  
  while (ptemp->getParent()!=0) {
    ptemp=ptemp->getParent();
    mom=ptemp->getP4();
    temp=boostTo(temp,mom);   
  } 
  return temp;
}

EvtVector4R EvtParticle::getP4Restframe() {

  return EvtVector4R(mass(),0.0,0.0,0.0);

}

EvtVector4R EvtParticle::get4Pos() {

  EvtVector4R temp,mom;
  EvtParticle *ptemp;
  
  temp.set(0.0,0.0,0.0,0.0);
  ptemp=getParent();

  if (ptemp==0) return temp;

  temp=(ptemp->_t/ptemp->mass())*(ptemp->getP4());

  while (ptemp->getParent()!=0) {
    ptemp=ptemp->getParent();
    mom=ptemp->getP4();
    temp=boostTo(temp,mom);
    temp=temp+(ptemp->_t/ptemp->mass())*(ptemp->getP4());
  } 
  
  return temp;
}


EvtParticle * EvtParticle::nextIter(EvtParticle *rootOfTree) {

  EvtParticle *bpart;
  EvtParticle *current;

  current=this;
  int i;

  if (_ndaug!=0) return _daug[0];

  do{
    bpart=current->_parent;
    if (bpart==0) return 0;
    i=0;
    while (bpart->_daug[i]!=current) {i++;}

    if ( bpart==rootOfTree ) {
      if ( i== bpart->_ndaug-1 ) return 0;
    }

    i++;
    current=bpart;

  }while(i>=bpart->_ndaug);

  return bpart->_daug[i];

}


void EvtParticle::makeStdHep(EvtStdHep& stdhep,EvtSecondary& secondary,
			     EvtId *list_of_stable){

  //first add particle to the stdhep list;
  stdhep.createParticle(getP4Lab(),get4Pos(),-1,-1,
			EvtPDL::getStdHep(getId()));

  int ii=0;

  //lets see if this is a longlived particle and terminate the 
  //list building!
  
  while (list_of_stable[ii]!=EvtId(-1,-1)) {
    if (getId()==list_of_stable[ii]){
      secondary.createSecondary(0,this);
      return;
    }
    ii++;
  }




  int i;
  for(i=0;i<_ndaug;i++){
    stdhep.createParticle(_daug[i]->getP4Lab(),_daug[i]->get4Pos(),0,0,
			  EvtPDL::getStdHep(_daug[i]->getId()));
  }

  for(i=0;i<_ndaug;i++){
    _daug[i]->makeStdHepRec(1+i,1+i,stdhep,secondary,list_of_stable);
  }
  return;

}

void EvtParticle::makeStdHep(EvtStdHep& stdhep){

  //first add particle to the stdhep list;
  stdhep.createParticle(getP4Lab(),get4Pos(),-1,-1,
			EvtPDL::getStdHep(getId()));

  int i;
  for(i=0;i<_ndaug;i++){
    stdhep.createParticle(_daug[i]->getP4Lab(),_daug[i]->get4Pos(),0,0,
			  EvtPDL::getStdHep(_daug[i]->getId()));
  }

  for(i=0;i<_ndaug;i++){
    _daug[i]->makeStdHepRec(1+i,1+i,stdhep);
  }
  return;

}


void EvtParticle::makeStdHepRec(int firstparent,int lastparent,
				EvtStdHep& stdhep,
				EvtSecondary& secondary,
				EvtId *list_of_stable){


  int ii=0;

  //lets see if this is a longlived particle and terminate the 
  //list building!
  
  while (list_of_stable[ii]!=EvtId(-1,-1)) {
    if (getId()==list_of_stable[ii]){
      secondary.createSecondary(firstparent,this);
      return;
    }
    ii++;
  }



  int i;
  int parent_num=stdhep.getNPart();
  for(i=0;i<_ndaug;i++){
    stdhep.createParticle(_daug[i]->getP4Lab(),_daug[i]->get4Pos(),
			  firstparent,lastparent,
			  EvtPDL::getStdHep(_daug[i]->getId()));
  }

  for(i=0;i<_ndaug;i++){
    _daug[i]->makeStdHepRec(parent_num+i,parent_num+i,stdhep,
			   secondary,list_of_stable);
  }
  return;

}

void EvtParticle::makeStdHepRec(int firstparent,int lastparent,
				EvtStdHep& stdhep){

  int i;
  int parent_num=stdhep.getNPart();
  for(i=0;i<_ndaug;i++){
    stdhep.createParticle(_daug[i]->getP4Lab(),_daug[i]->get4Pos(),
			  firstparent,lastparent,
			  EvtPDL::getStdHep(_daug[i]->getId()));
  }

  for(i=0;i<_ndaug;i++){
    _daug[i]->makeStdHepRec(parent_num+i,parent_num+i,stdhep);
  }
  return;

}

void EvtParticle::printTreeRec(int level) const {

  int newlevel,i;
  newlevel = level +1;

  
  if (_ndaug!=0) {
    if ( level > 0 ) {
      for (i=0;i<(5*level);i++) {
	report(INFO,"") <<" ";
      }
    }
    report(INFO,"") << EvtPDL::name(_id).c_str();  
    report(INFO,"") << " -> ";
    for(i=0;i<_ndaug;i++){
      report(INFO,"") << EvtPDL::name(_daug[i]->getId()).c_str()<<" ";
    }
    report(INFO,"")<<std::endl;
    for(i=0;i<_ndaug;i++){
      _daug[i]->printTreeRec(newlevel);
    }
  }
}

void EvtParticle::printTree() const {
  
  report(INFO,"EvtGen") << "This is the current decay chain"<<std::endl;
  report(INFO,"") << "This top particle is "<<
    EvtPDL::name(_id).c_str()<<std::endl;  

  this->printTreeRec(0);
  report(INFO,"EvtGen") << "End of decay chain."<<std::endl;

}

void EvtParticle::printParticle() const {

  switch (EvtPDL::getSpinType(_id)){ 
  case EvtSpinType::SCALAR:
    report(INFO,"EvtGen") << "This is a scalar particle:"
                          <<EvtPDL::name(_id).c_str()<<"\n";
    break;     
  case EvtSpinType::VECTOR:
    report(INFO,"EvtGen") << "This is a vector particle:"
                          <<EvtPDL::name(_id).c_str()<<"\n";
    break;     
  case EvtSpinType::TENSOR:
    report(INFO,"EvtGen") << "This is a tensor particle:"
                          <<EvtPDL::name(_id).c_str()<<"\n";
    break;
  case EvtSpinType::DIRAC:
    report(INFO,"EvtGen") << "This is a dirac particle:"
                          <<EvtPDL::name(_id).c_str()<<"\n";
    break;
  case EvtSpinType::PHOTON:
    report(INFO,"EvtGen") << "This is a photon:"<<EvtPDL::name(_id).c_str()
                          <<"\n";
    break;
  case EvtSpinType::NEUTRINO:
    report(INFO,"EvtGen") << "This is a neutrino:"<<EvtPDL::name(_id).c_str()
                          <<"\n";
    break;
  case EvtSpinType::STRING:
    report(INFO,"EvtGen") << "This is a string:"<<EvtPDL::name(_id).c_str()
                          <<"\n";
    break;
  default:
    report(INFO,"EvtGen") 
      <<"Unknown particle type in EvtParticle::printParticle()"<<std::endl;
    break;
  }
  report(INFO,"EvtGen") << "Number of daughters:"<<_ndaug<<"\n";


}



void init_vector( EvtParticle **part ){
  *part = (EvtParticle *) new EvtVectorParticle;
} 


void init_scalar( EvtParticle **part ){
  *part = (EvtParticle *) new EvtScalarParticle;
} 

void init_tensor( EvtParticle **part ){
  *part = (EvtParticle *) new EvtTensorParticle;
} 

void init_dirac( EvtParticle **part ){
  *part = (EvtParticle *) new EvtDiracParticle;
} 

void init_photon( EvtParticle **part ){
  *part = (EvtParticle *) new EvtPhotonParticle;
} 

void init_neutrino( EvtParticle **part ){
  *part = (EvtParticle *) new EvtNeutrinoParticle;
} 

void init_string( EvtParticle **part ){
  *part = (EvtParticle *) new EvtStringParticle;
} 

double EvtParticle::initializePhaseSpace(
                   int numdaughter,EvtId *daughters, double poleSize,
                   int whichTwo1, int whichTwo2) {
  
  double m_b;
  int i;
  //lange
  //  this->makeDaughters(numdaughter,daughters);

  static EvtVector4R p4[100];
  static double mass[100];

  m_b = this->mass();

  //lange - Jan2,2002 - Need to check to see if the daughters of the parent
  // have changed. If so, delete them and start over.
  //report(INFO,"EvtGen") << "the parent is\n";
  //if ( this->getParent() ) {
  //  if ( this->getParent()->getParent() ) 
  //this->getParent()->getParent()->printTree();
    //    this->getParent()->printTree();
  //}
  //report(INFO,"EvtGen") << "and this is\n";
  //if ( this) this->printTree();
  bool resetDaughters=false;

  for (i=0; i<numdaughter;i++) {
    if ( this->getDaug(i)->getId() != daughters[i] ) resetDaughters=true;
    //report(INFO,"EvtGen") << this->getDaug(i)->getId() << " " 
    //<< daughters[i] << std::endl;
  }
  if ( resetDaughters ) {
    //    report(INFO,"EvtGen") << "reseting daughters\n";
    //for (i=0; i<numdaughter;i++) {
    //  report(INFO,"EvtGen") << "reset " <<i<< " "
    //<< EvtPDL::name(this->getDaug(i)->getId()) << " " 
    //<< EvtPDL::name(daughters[i]) << std::endl;
    //}
    bool t1=true;
    //but keep the decay channel of the parent.
    this->deleteDaughters(t1);
    this->makeDaughters(numdaughter,daughters);
    this->generateMassTree();
  }

  double weight=0.;
  //  EvtDecayBase::findMasses( this, numdaughter, daughters, mass );
  //get the list of masses
  //report(INFO,"EvtGen") << "mpar= " << m_b << " " << this <<std::endl;
  for (i=0; i<numdaughter;i++) {
    mass[i]=this->getDaug(i)->mass();
    //    report(INFO,"EvtGen") << "mass " << i << " " << mass[i] << " " 
    //<< this->getDaug(i) << std::endl;
  }

  if ( poleSize<-0.1) {
    EvtGenKine::PhaseSpace( numdaughter, mass, p4, m_b );
    for(i=0;i<numdaughter;i++){
      this->getDaug(i)->init(daughters[i],p4[i]);
    }
  }
  else  {
    if ( numdaughter != 3 ) {
      report(ERROR,"EvtGen") << "Only can generate pole phase space "
			     << "distributions for 3 body final states"
			     << std::endl<<"Will terminate."<<std::endl;
      ::abort();
    }
    bool ok=false;
    if ( (whichTwo1 == 1 && whichTwo2 == 0 ) ||
	 (whichTwo1 == 0 && whichTwo2 == 1 ) ) {
      weight=EvtGenKine::PhaseSpacePole( m_b, mass[0], mass[1], mass[2], 
					  poleSize, p4);
      //report(INFO,"EvtGen") << "here " << weight << " " << poleSize 
      //<< std::endl; 
      this->getDaug(0)->init(daughters[0],p4[0]);
      this->getDaug(1)->init(daughters[1],p4[1]);
      this->getDaug(2)->init(daughters[2],p4[2]);
      ok=true;
    }
    if ( (whichTwo1 == 1 && whichTwo2 == 2 ) ||
	 (whichTwo1 == 2 && whichTwo2 == 1 ) ) {
      weight=EvtGenKine::PhaseSpacePole( m_b, mass[2], mass[1], mass[0], 
					  poleSize, p4);
      this->getDaug(0)->init(daughters[0],p4[2]);
      this->getDaug(1)->init(daughters[1],p4[1]);
      this->getDaug(2)->init(daughters[2],p4[0]);
      ok=true;
    }
    if ( (whichTwo1 == 0 && whichTwo2 == 2 ) ||
	 (whichTwo1 == 2 && whichTwo2 == 0 ) ) {
      weight=EvtGenKine::PhaseSpacePole( m_b, mass[1], mass[0], mass[2], 
					  poleSize, p4);
      this->getDaug(0)->init(daughters[0],p4[1]);
      this->getDaug(1)->init(daughters[1],p4[0]);
      this->getDaug(2)->init(daughters[2],p4[2]);
      ok=true;
    }
    if ( !ok) {
      report(ERROR,"EvtGen") 
        << "Invalid pair of particle to generate a pole dist"
			     << whichTwo1 << " " << whichTwo2
			     << std::endl<<"Will terminate."<<std::endl;
      ::abort();
    }
  }

  return weight;
}

void EvtParticle::makeDaughters( int ndaugstore, EvtId *id){

  int i;
  if ( _channel < 0 ) {
    //report(INFO,"EvtGen") << "setting channel " 
    //<< EvtPDL::name(this->getId()) << std::endl;
    setChannel(0);
  }
  EvtParticle* pdaug;  
  if (_ndaug!=0 ){
    if (_ndaug!=ndaugstore){
      report(ERROR,"EvtGen") << "Asking to make a different number of "
			     << "daughters than what was previously created."
			     << std::endl<<"Will terminate."<<std::endl;
      ::abort();
    }
  } 
  else{
    for(i=0;i<ndaugstore;i++){
      pdaug=EvtParticleFactory::particleFactory(EvtPDL::getSpinType(id[i]));
      pdaug->setId(id[i]);
      pdaug->addDaug(this);	
    }

  } //else
} //makeDaughters

bool EvtParticle::isBsMixed ( ) 
{ 
  if ( ! ( getParent() ) ) return false ;
  
  static EvtId BS0=EvtPDL::getId("B_s0");
  static EvtId BSB=EvtPDL::getId("anti-B_s0");
  
  if ( ( getId() != BS0 ) && ( getId() != BSB ) ) return false ;
  
  if ( ( getParent()->getId() == BS0 ) ||
       ( getParent()->getId() == BSB ) ) return true ;
  
  return false ;
}

