// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// local
#include "IonPhysics.h"

/** @file 
 * 
 *  implementation of class IonPhysics
 *
 *  @author Witek Pokorski Witold.Pokorski@cern.ch
 */

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( IonPhysics) ;
// ============================================================================

IonPhysics::IonPhysics
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
IonPhysics::~IonPhysics(){};
// ============================================================================

// ============================================================================
// ============================================================================
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
// Nuclei
#include "G4IonConstructor.hh"

void IonPhysics::ConstructParticle()
{
  //  Construct light ions
  G4IonConstructor pConstructor;
  pConstructor.ConstructParticle(); 
};

// ============================================================================
// ============================================================================
#include "G4ProcessManager.hh"

void IonPhysics::ConstructProcess()
{
  G4ProcessManager * pManager = 0;
  
  // Elastic Process
  theElasticModel = new G4LElastic();
  theIonElasticProcess.RegisterMe(theElasticModel);
  theDElasticProcess.RegisterMe(theElasticModel);
  theTElasticProcess.RegisterMe(theElasticModel);
  theAElasticProcess.RegisterMe(theElasticModel);
  theHe3ElasticProcess.RegisterMe(theElasticModel);

  // Generic Ion
  pManager = G4GenericIon::GenericIon()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theIonElasticProcess);

  pManager->AddProcess(&fIonIonisation, ordInActive, 2, 2);

  //  pManager->AddProcess(&fIonMultipleScattering);
  //  pManager->SetProcessOrdering(&fIonMultipleScattering, idxAlongStep,  1);
  //  pManager->SetProcessOrdering(&fIonMultipleScattering, idxPostStep,  1);

  // Deuteron 
  pManager = G4Deuteron::Deuteron()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theDElasticProcess);

  fDeuteronModel = new G4LEDeuteronInelastic();
  fDeuteronProcess.RegisterMe(fDeuteronModel);
  pManager->AddDiscreteProcess(&fDeuteronProcess);

  pManager->AddProcess(&fDeuteronIonisation, ordInActive, 2, 2);

  pManager->AddProcess(&fDeuteronMultipleScattering);
  pManager->SetProcessOrdering(&fDeuteronMultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&fDeuteronMultipleScattering, idxPostStep,  1);
 
  // Triton 
  pManager = G4Triton::Triton()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theTElasticProcess);

  fTritonModel = new G4LETritonInelastic();
  fTritonProcess.RegisterMe(fTritonModel);
  pManager->AddDiscreteProcess(&fTritonProcess);

  pManager->AddProcess(&fTritonIonisation, ordInActive, 2, 2);

  pManager->AddProcess(&fTritonMultipleScattering);
  pManager->SetProcessOrdering(&fTritonMultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&fTritonMultipleScattering, idxPostStep,  1);
 
  // Alpha 
  pManager = G4Alpha::Alpha()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theAElasticProcess);

  fAlphaModel = new G4LEAlphaInelastic();
  fAlphaProcess.RegisterMe(fAlphaModel);
  pManager->AddDiscreteProcess(&fAlphaProcess);

  pManager->AddProcess(&fAlphaIonisation, ordInActive, 2, 2);

  pManager->AddProcess(&fAlphaMultipleScattering);
  pManager->SetProcessOrdering(&fAlphaMultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&fAlphaMultipleScattering, idxPostStep,  1);
  
  // He3
  pManager = G4He3::He3()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theHe3ElasticProcess);
  
  pManager->AddProcess(&fHe3Ionisation, ordInActive, 2, 2);
  
  pManager->AddProcess(&fHe3MultipleScattering);
  pManager->SetProcessOrdering(&fHe3MultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&fHe3MultipleScattering, idxPostStep,  1);
};

// ============================================================================
// The END 
// ============================================================================
