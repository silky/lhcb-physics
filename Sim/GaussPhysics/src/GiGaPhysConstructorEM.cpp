// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// G4 
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4ios.hh"
#include "g4std/iomanip"                
// local
#include "GiGaPhysConstructorEM.h"

/** @file 
 * 
 *  implementation of class GiGaPhysConstructorEM
 *
 *  @author Witek Pokorski Witold.Pokorski@cern.ch
 */

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaPhysConstructorEM) ;
// ============================================================================

GiGaPhysConstructorEM::GiGaPhysConstructorEM
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase (type, name, parent)
{
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysConstructorEM::~GiGaPhysConstructorEM(){};
// ============================================================================

// ============================================================================
// ============================================================================
void GiGaPhysConstructorEM::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  ConstructBosons();
  ConstructLeptons();
  ConstructMesons();
  ConstructBaryons();

};

// ============================================================================
// ============================================================================
void GiGaPhysConstructorEM::ConstructProcess()
{
  ConstructEM();
};


// ============================================================================
// ============================================================================
void GiGaPhysConstructorEM::ConstructBosons()
{
  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();

  // gamma
  G4Gamma::GammaDefinition();
  //optical photon
  G4OpticalPhoton::OpticalPhotonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// ============================================================================
// ============================================================================
void GiGaPhysConstructorEM::ConstructLeptons()
{
  // leptons
  //  e+/-
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  // mu+/-
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();
  // nu_e
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  // nu_mu
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
}

// ============================================================================
// ============================================================================
void GiGaPhysConstructorEM::ConstructMesons()
{
  //  mesons
  //    light mesons
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4PionZero::PionZeroDefinition();
  G4Eta::EtaDefinition();
  G4EtaPrime::EtaPrimeDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();
  G4KaonZero::KaonZeroDefinition();
  G4AntiKaonZero::AntiKaonZeroDefinition();
  G4KaonZeroLong::KaonZeroLongDefinition();
  G4KaonZeroShort::KaonZeroShortDefinition();
}

// ============================================================================
// ============================================================================
void GiGaPhysConstructorEM::ConstructBaryons()
{
  //  barions
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();

  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();
}


// ============================================================================
// ============================================================================
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4MultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"

///< #include "G4UserSpecialCuts.hh"

// ============================================================================
// ============================================================================
void GiGaPhysConstructorEM::ConstructEM()
{
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
     
    if (particleName == "gamma") {
    // gamma
      pmanager->AddDiscreteProcess(new G4GammaConversion());
      pmanager->AddDiscreteProcess(new G4ComptonScattering());      
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());
      
    } else if (particleName == "e-") {
    //electron
      G4VProcess* theeminusMultipleScattering = new G4MultipleScattering();
      G4VProcess* theeminusIonisation         = new G4eIonisation();
      G4VProcess* theeminusBremsstrahlung     = new G4eBremsstrahlung();
      //
      // add processes
      pmanager->AddProcess(theeminusMultipleScattering);
      pmanager->AddProcess(theeminusIonisation);
      pmanager->AddProcess(theeminusBremsstrahlung);
      //      
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(theeminusMultipleScattering, idxAlongStep,1);
      pmanager->SetProcessOrdering(theeminusIonisation,         idxAlongStep,2);
      //
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(theeminusMultipleScattering, idxPostStep,1);
      pmanager->SetProcessOrdering(theeminusIonisation,         idxPostStep,2);
      pmanager->SetProcessOrdering(theeminusBremsstrahlung,     idxPostStep,3);

    } else if (particleName == "e+") {
    //positron
      G4VProcess* theeplusMultipleScattering = new G4MultipleScattering();
      G4VProcess* theeplusIonisation         = new G4eIonisation();
      G4VProcess* theeplusBremsstrahlung     = new G4eBremsstrahlung();
      G4VProcess* theeplusAnnihilation       = new G4eplusAnnihilation();
      //
      // add processes
      pmanager->AddProcess(theeplusMultipleScattering);
      pmanager->AddProcess(theeplusIonisation);
      pmanager->AddProcess(theeplusBremsstrahlung);
      pmanager->AddProcess(theeplusAnnihilation);
      //
      // set ordering for AtRestDoIt
      pmanager->SetProcessOrderingToFirst(theeplusAnnihilation, idxAtRest);
      //
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(theeplusMultipleScattering, idxAlongStep,1);
      pmanager->SetProcessOrdering(theeplusIonisation,         idxAlongStep,2);
      //
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(theeplusMultipleScattering, idxPostStep,1);
      pmanager->SetProcessOrdering(theeplusIonisation,         idxPostStep,2);
      pmanager->SetProcessOrdering(theeplusBremsstrahlung,     idxPostStep,3);
      pmanager->SetProcessOrdering(theeplusAnnihilation,       idxPostStep,4);
  
    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
    //muon  
      G4VProcess* aMultipleScattering = new G4MultipleScattering();
      G4VProcess* aBremsstrahlung     = new G4MuBremsstrahlung();
      G4VProcess* aPairProduction     = new G4MuPairProduction();
      G4VProcess* anIonisation        = new G4MuIonisation();
      //
      // add processes
      pmanager->AddProcess(anIonisation);
      pmanager->AddProcess(aMultipleScattering);
      pmanager->AddProcess(aBremsstrahlung);
      pmanager->AddProcess(aPairProduction);
      //
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,1);
      pmanager->SetProcessOrdering(anIonisation,        idxAlongStep,2);
      //
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep,1);
      pmanager->SetProcessOrdering(anIonisation,        idxPostStep,2);
      pmanager->SetProcessOrdering(aBremsstrahlung,     idxPostStep,3);
      pmanager->SetProcessOrdering(aPairProduction,     idxPostStep,4);

    } else if ((!particle->IsShortLived()) &&
               (particle->GetPDGCharge() != 0.0) && 
               (particle->GetParticleName() != "chargedgeantino")) {
     // all others charged particles except geantino
     G4VProcess* aMultipleScattering = new G4MultipleScattering();
     G4VProcess* anIonisation        = new G4hIonisation();
     
     ////G4VProcess*  theUserCuts = new G4UserSpecialCuts();
     //
     // add processes
     pmanager->AddProcess(anIonisation);
     pmanager->AddProcess(aMultipleScattering);
     
     /// pmanager->AddProcess(theUserCuts);
     
     //
     // set ordering for AtRestDoIt
     ////pmanager->SetProcessOrderingToFirst(theUserCuts,idxAtRest);
     
     //
     // set ordering for AlongStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,1);
     pmanager->SetProcessOrdering(anIonisation,        idxAlongStep,2);
     
     //
     // set ordering for PostStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep,1);
     pmanager->SetProcessOrdering(anIonisation,        idxPostStep,2);
     ////pmanager->SetProcessOrdering(theUserCuts,     idxPostStep,3);
    }
  }
}
  

// ============================================================================
// The END 
// ============================================================================
