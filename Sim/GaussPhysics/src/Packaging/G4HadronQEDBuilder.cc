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
#include "G4HadronQEDBuilder.hh"

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ProcessManager.hh"

#include "G4Proton.hh"
#include "G4AntiProton.hh"

#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4SigmaMinus.hh"
#include "G4AntiSigmaMinus.hh"
#include "G4SigmaPlus.hh"
#include "G4AntiSigmaPlus.hh"
#include "G4XiMinus.hh"
#include "G4AntiXiMinus.hh"
#include "G4OmegaMinus.hh"
#include "G4AntiOmegaMinus.hh"

G4HadronQEDBuilder::G4HadronQEDBuilder(): wasActivated(false) 
{
}
G4HadronQEDBuilder::~G4HadronQEDBuilder() 
{
  if(wasActivated)
  {
  RemoveOne(G4PionPlus::PionPlus()->GetProcessManager(), &thePionPlusMult, &thePionPlusIonisation);
  RemoveOne(G4PionMinus::PionMinus()->GetProcessManager(), &thePionMinusMult, &thePionMinusIonisation);
  RemoveOne(G4KaonPlus::KaonPlus()->GetProcessManager(), &theKaonPlusMult, &theKaonPlusIonisation);
  RemoveOne(G4KaonMinus::KaonMinus()->GetProcessManager(), &theKaonMinusMult, &theKaonMinusIonisation);
  RemoveOne(G4Proton::Proton()->GetProcessManager(), &theProtonMult, &theProtonIonisation);
  RemoveOne(G4AntiProton::AntiProton()->GetProcessManager(), &theAntiProtonMult, &theAntiProtonIonisation);
  RemoveOne(G4SigmaMinus::SigmaMinus()->GetProcessManager(), &theSigmaMinusMult, &theSigmaMinusIonisation);
  RemoveOne(G4AntiSigmaMinus::AntiSigmaMinus()->GetProcessManager(), &theAntiSigmaMinusMult, &theAntiSigmaMinusIonisation);
  RemoveOne(G4SigmaPlus::SigmaPlus()->GetProcessManager(), &theSigmaPlusMult, &theSigmaPlusIonisation);
  RemoveOne(G4AntiSigmaPlus::AntiSigmaPlus()->GetProcessManager(), &theAntiSigmaPlusMult, &theAntiSigmaPlusIonisation);
  RemoveOne(G4XiMinus::XiMinus()->GetProcessManager(), &theXiMinusMult, &theXiMinusIonisation);
  RemoveOne(G4AntiXiMinus::AntiXiMinus()->GetProcessManager(), &theAntiXiMinusMult, &theAntiXiMinusIonisation);
  RemoveOne(G4OmegaMinus::OmegaMinus()->GetProcessManager(), &theOmegaMinusMult, &theOmegaMinusIonisation);
  RemoveOne(G4AntiOmegaMinus::AntiOmegaMinus()->GetProcessManager(), &theAntiOmegaMinusMult, &theAntiOmegaMinusIonisation);
  }
}

void G4HadronQEDBuilder::
RegisterOne(G4ProcessManager* aP, G4MultipleScattering * aM, G4hIonisation* aI)
{
  aP->AddProcess(aI, ordInActive,2, 2);
  aP->AddProcess(aM);
  aP->SetProcessOrdering(aM, idxAlongStep, 1);
  aP->SetProcessOrdering(aM, idxPostStep, 1);
}

void G4HadronQEDBuilder::
RemoveOne(G4ProcessManager* aP, G4MultipleScattering * aM, G4hIonisation* aI)
{
  if(aP) aP->RemoveProcess(aI);
  if(aP) aP->RemoveProcess(aM);
}

void G4HadronQEDBuilder::Build()
{
  wasActivated = true;

  // PionPlus
  RegisterOne(G4PionPlus::PionPlus()->GetProcessManager(), &thePionPlusMult, &thePionPlusIonisation);

  // PionMinus
  RegisterOne(G4PionMinus::PionMinus()->GetProcessManager(), &thePionMinusMult, &thePionMinusIonisation);

  // KaonPlus
  RegisterOne(G4KaonPlus::KaonPlus()->GetProcessManager(), &theKaonPlusMult, &theKaonPlusIonisation);

  // KaonMinus
  RegisterOne(G4KaonMinus::KaonMinus()->GetProcessManager(), &theKaonMinusMult, &theKaonMinusIonisation);

  // Proton
  RegisterOne(G4Proton::Proton()->GetProcessManager(), &theProtonMult, &theProtonIonisation);

  // anti-Proton
  RegisterOne(G4AntiProton::AntiProton()->GetProcessManager(), &theAntiProtonMult, &theAntiProtonIonisation);
    
  // SigmaMinus
  RegisterOne(G4SigmaMinus::SigmaMinus()->GetProcessManager(), &theSigmaMinusMult, &theSigmaMinusIonisation);

  // anti-SigmaMinus
  RegisterOne(G4AntiSigmaMinus::AntiSigmaMinus()->GetProcessManager(), &theAntiSigmaMinusMult, &theAntiSigmaMinusIonisation);

  // SigmaPlus
  RegisterOne(G4SigmaPlus::SigmaPlus()->GetProcessManager(), &theSigmaPlusMult, &theSigmaPlusIonisation);

  // anti-SigmaPlus
  RegisterOne(G4AntiSigmaPlus::AntiSigmaPlus()->GetProcessManager(), &theAntiSigmaPlusMult, &theAntiSigmaPlusIonisation);

  // XiMinus
  RegisterOne(G4XiMinus::XiMinus()->GetProcessManager(), &theXiMinusMult, &theXiMinusIonisation);

  // anti-XiMinus
  RegisterOne(G4AntiXiMinus::AntiXiMinus()->GetProcessManager(), &theAntiXiMinusMult, &theAntiXiMinusIonisation);

  // OmegaMinus
  RegisterOne(G4OmegaMinus::OmegaMinus()->GetProcessManager(), &theOmegaMinusMult, &theOmegaMinusIonisation);

  // anti-OmegaMinus
  RegisterOne(G4AntiOmegaMinus::AntiOmegaMinus()->GetProcessManager(), &theAntiOmegaMinusMult, &theAntiOmegaMinusIonisation);
}





// 2002 by J.P. Wellisch
