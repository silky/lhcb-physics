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
#ifndef G4EMBuilder_h
#define G4EMBuilder_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4MultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4EMTailorer.hh"
#include "G4SynchrotronRadiation.hh"
#include "G4ElectroNuclearBuilder.hh"

class G4EMBuilder 
{
  public: 
    G4EMBuilder();
    virtual ~G4EMBuilder();

  public: 
    void Build();
    
    void Synch(G4String & aState);
    void GammaNuclear(G4String & aState);

  protected:
    G4PhotoElectricEffect thePhotoEffect;
    G4ComptonScattering theComptonEffect;
    G4GammaConversion thePairProduction;
  
    G4MultipleScattering theElectronMultipleScattering;
    G4eIonisation theElectronIonisation;
    G4eBremsstrahlung theElectronBremsStrahlung;
  
    G4MultipleScattering thePositronMultipleScattering;
    G4eIonisation thePositronIonisation; 
    G4eBremsstrahlung thePositronBremsStrahlung;  
    G4eplusAnnihilation theAnnihilation;
    G4bool wasActivated;
    
    G4EMTailorer * theT;
    G4SynchrotronRadiation theElectronSynch;
    G4SynchrotronRadiation thePositronSynch;
    G4bool synchOn;
    G4ElectroNuclearBuilder theGNPhysics;
    G4bool gammNucOn;
};
// 2002 by J.P. Wellisch
#endif





