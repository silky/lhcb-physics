//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
#include "G4StoppingHadronBuilder.hh"

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4PionMinus.hh"
#include "G4KaonMinus.hh"
#include "G4AntiProton.hh"
#include "G4AntiNeutron.hh"

G4StoppingHadronBuilder::
G4StoppingHadronBuilder(): wasActivated(false) {}

G4StoppingHadronBuilder::
~G4StoppingHadronBuilder() 
{
  if(wasActivated)
  {
  G4ProcessManager * aProcMan = 0;
  aProcMan = G4PionMinus::PionMinusDefinition()->GetProcessManager();
  if(aProcMan) aProcMan->RemoveProcess(&thePionMinusAbsorption);
  aProcMan = G4KaonMinus::KaonMinusDefinition()->GetProcessManager();
  if(aProcMan) aProcMan->RemoveProcess(&theKaonMinusAbsorption);
  aProcMan = G4AntiProton::AntiProtonDefinition()->GetProcessManager();
  if(aProcMan) aProcMan->RemoveProcess(&theAntiProtonAnnihilation);
  aProcMan = G4AntiNeutron::AntiNeutronDefinition()->GetProcessManager();
  if(aProcMan) aProcMan->RemoveProcess(&theAntiNeutronAnnihilation);
  }
}

void G4StoppingHadronBuilder::Build()
{
  G4ProcessManager * aProcMan = 0;
  wasActivated=true;
  
  // PionMinus
  aProcMan = G4PionMinus::PionMinusDefinition()->GetProcessManager();
  aProcMan->AddRestProcess(&thePionMinusAbsorption, ordDefault);

  // KaonMinus
  aProcMan = G4KaonMinus::KaonMinusDefinition()->GetProcessManager();
  aProcMan->AddRestProcess(&theKaonMinusAbsorption, ordDefault);

  // anti-Proton
  aProcMan = G4AntiProton::AntiProtonDefinition()->GetProcessManager();
  aProcMan->AddRestProcess(&theAntiProtonAnnihilation);

  // AntiNeutron
  aProcMan = G4AntiNeutron::AntiNeutronDefinition()->GetProcessManager();
  aProcMan->AddRestProcess(&theAntiNeutronAnnihilation);

}





// 2002 by J.P. Wellisch
