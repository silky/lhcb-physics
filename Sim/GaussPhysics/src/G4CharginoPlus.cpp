// $Id: G4CharginoPlus.cpp,v 1.1 2008-11-27 16:02:08 robbep Exp $

#include <fstream>
#include <iomanip>

#include "G4CharginoPlus.h"
#include "G4ParticleTable.hh"

// ######################################################################
// ###                      CharginoPlus                              ###
// ######################################################################

G4CharginoPlus * G4CharginoPlus::theInstance = 0 ;

G4CharginoPlus * G4CharginoPlus::Definition()
{

  if (theInstance !=0) return theInstance;
  const G4String name = "s_chi_plus_1";
  // search in particle table]
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
      new G4ParticleDefinition( name , 110.00*GeV, 2.73e-13*GeV, +1.*eplus, 
                                1,              0,             0,
                                0,              0,             0,
                                "supersymmetric", 0,  0,  1000024,
                                false,     6.68e-1*ns,      NULL,
                                true, "CharginoPlus" );
  }
  theInstance = reinterpret_cast<G4CharginoPlus*>(anInstance);
  return theInstance;
}

G4CharginoPlus*  G4CharginoPlus::CharginoPlusDefinition(){return Definition();}
G4CharginoPlus*  G4CharginoPlus::CharginoPlus(){return Definition();}

