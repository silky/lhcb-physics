#include "G4EMModelBuilder.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4ProcessManager.hh"

#include "globals.hh"
#include "G4ios.hh"

G4EMModelBuilder::
G4EMModelBuilder() : wasActivated(false) {}

G4EMModelBuilder::
~G4EMModelBuilder() 
{
  if(wasActivated)
  {
  G4ProcessManager * pManager = 0;
  pManager = G4Gamma::Gamma()->GetProcessManager();
  if(pManager)
  {
  pManager->RemoveProcess(&thePhotoEffect);
  pManager->RemoveProcess(&theComptonEffect);
  pManager->RemoveProcess(&thePairProduction);
  }

  pManager = G4Electron::Electron()->GetProcessManager();
  if(pManager)
  {
  pManager->RemoveProcess(&theElectronBremsStrahlung);  
  pManager->RemoveProcess(&theElectronIonisation);
  pManager->RemoveProcess(&theElectronMultipleScattering);
  }

  pManager = G4Positron::Positron()->GetProcessManager();
  if(pManager)
  {
  pManager->RemoveProcess(&thePositronBremsStrahlung);
  pManager->RemoveProcess(&theAnnihilation);
  pManager->RemoveProcess(&thePositronIonisation);
  pManager->RemoveProcess(&thePositronMultipleScattering);
  }
  }
}

void G4EMModelBuilder::Build()
{
  G4ProcessManager * pManager = 0;
  wasActivated=true;
  
  pManager = G4Gamma::Gamma()->GetProcessManager();
  pManager->AddDiscreteProcess(&thePhotoEffect);
  pManager->AddDiscreteProcess(&theComptonEffect);
  pManager->AddDiscreteProcess(&thePairProduction);

  pManager = G4Electron::Electron()->GetProcessManager();
  pManager->AddDiscreteProcess(&theElectronBremsStrahlung);  
  pManager->AddProcess(&theElectronIonisation, ordInActive,2, 2);
  pManager->AddProcess(&theElectronMultipleScattering);
  pManager->SetProcessOrdering(&theElectronMultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&theElectronMultipleScattering, idxPostStep,  1);

  pManager = G4Positron::Positron()->GetProcessManager();
  pManager->AddDiscreteProcess(&thePositronBremsStrahlung);
  pManager->AddDiscreteProcess(&theAnnihilation);
  pManager->AddRestProcess(&theAnnihilation);
  pManager->AddProcess(&thePositronIonisation, ordInActive,2, 2);
  pManager->AddProcess(&thePositronMultipleScattering);
  pManager->SetProcessOrdering(&thePositronMultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&thePositronMultipleScattering, idxPostStep,  1);

}
// 2002 by J.P. Wellisch
