

#include "G4OmegabMinus.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      OmegabMinus                        ###
// ######################################################################

G4OmegabMinus * G4OmegabMinus::theInstance = 0 ;

G4OmegabMinus * G4OmegabMinus::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "omega_b-";//is it normal that it's lower case? 
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
      new G4ParticleDefinition( name , 6.054*GeV , 5.e-10*MeV , -1 ,
                                1,              +1,             0,
                                0,              0,             0,
                                "baryon", 0,  1, 5332,
                                false,     1.36e-3*ns,          NULL,
                                false, "omega_b-" );
  }
  theInstance = reinterpret_cast<G4OmegabMinus*>(anInstance);
  return theInstance;
}

G4OmegabMinus * G4OmegabMinus::OmegabMinusDefinition() {
  return Definition( ) ;
}

G4OmegabMinus * G4OmegabMinus::OmegabMinus() {
  return Definition( ) ;
}
