// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// local
#include "HadronPhysicsQGSP.h"
// G4
#include "globals.hh"
#include "G4ios.hh"
#include "g4std/iomanip" 

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( HadronPhysicsQGSP) ;
// ============================================================================

HadronPhysicsQGSP::HadronPhysicsQGSP
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase (type, name, parent)
{
  theNeutrons.RegisterMe(&theQGSPNeutron);
  theNeutrons.RegisterMe(&theLEPNeutron);
  theLEPNeutron.SetMaxEnergy(25*GeV);

  thePro.RegisterMe(&theQGSPPro);
  thePro.RegisterMe(&theLEPPro);
  theLEPPro.SetMaxEnergy(25*GeV);
  
  thePiK.RegisterMe(&theQGSPPiK);
  thePiK.RegisterMe(&theLEPPiK);
  theLEPPiK.SetMaxEnergy(25*GeV);
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
HadronPhysicsQGSP::~HadronPhysicsQGSP(){};
// ============================================================================

void HadronPhysicsQGSP::ConstructParticle()
{
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  
};

// ============================================================================
// ============================================================================
#include "G4ProcessManager.hh"

void HadronPhysicsQGSP::ConstructProcess()
{
  theNeutrons.Build();
  thePro.Build();
  thePiK.Build();
  theMiscLHEP.Build();
  theStoppingHadron.Build();
  theHadronQED.Build();
};
  

// ============================================================================
// The END 
// ============================================================================
