// $Id: G4AntiXiccMinusMinus.cpp,v 1.1 2014-02-10 14:04:24 Liang Zhong Exp $

#include "G4AntiXiccMinusMinus.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      AntiXiccMinusMinus                        ###
// ######################################################################

G4AntiXiccMinusMinus * G4AntiXiccMinusMinus::theInstance = 0 ;

G4AntiXiccMinusMinus * G4AntiXiccMinusMinus::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "anti-xi_cc--";
  // search in particle table
  G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* anInstance = pTable->FindParticle(name);
  if (anInstance ==0)
  {
  // create particle
  //
  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table
  //             shortlived      subType    anti_encoding
    anInstance = 
      new G4ParticleDefinition( name ,      3.620*GeV ,  5.e-10*MeV ,  -2.*eplus ,
                                1,          -1,          0,
                                1,          -1,          0,
                                "baryon",   0,           -1,           -4422,
                                false,      0.450e-3*ns, NULL,
                                false,      "xi_cc" );
  }
  theInstance = reinterpret_cast<G4AntiXiccMinusMinus*>(anInstance);
  return theInstance;
}

G4AntiXiccMinusMinus * G4AntiXiccMinusMinus::AntiXiccMinusMinusDefinition() {
  return Definition( ) ;
}

G4AntiXiccMinusMinus * G4AntiXiccMinusMinus::AntiXiccMinusMinus() {
  return Definition( ) ;
}
