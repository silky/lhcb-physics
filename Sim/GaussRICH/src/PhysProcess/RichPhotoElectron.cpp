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
//
// $Id: RichPhotoElectron.cpp,v 1.4 2004-02-04 13:53:53 seaso Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
// 
// ----------------------------------------------------------------------
//      GEANT 4 class implementation file
//
//      History: first implementation, based on object model of
//      4th April 1996, G.Cosmo
// **********************************************************************
//  Added particle definitions, H.Kurashige, 19 April 1996
//  Added SetCuts implementation, L.Urban, 30 May 1996
//  Revised, G.Cosmo, 6 June 1996
//  Code uses operators (+=, *=, ++, -> etc.) correctly, P. Urban, 26/6/96
//  Add ElectronDefinition() H.Kurashige 4 July 1996
// ----------------------------------------------------------------------
// RichPhotoelectron created by SE 10-3-2003.
//
//#include "g4std/fstream"
//#include "g4std/iomanip"
    
#include "G4Electron.hh"
#include "RichPhotoElectron.h"
// ######################################################################
// ###                         RICHPHOTOELECTRON                      ###
// ######################################################################

RichPhotoElectron::RichPhotoElectron(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,   
       G4int               iSpin,        G4int               iParity,    
       G4int               iConjugation, G4int               iIsospin,   
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,      
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable )
 : G4VLepton( aName,mass,width,charge,iSpin,iParity,
	      iConjugation,iIsospin,iIsospin3,gParity,pType,
              lepton,baryon,encoding,stable,lifetime,decaytable )
{
  SetParticleSubType("pe");
}

// ......................................................................
// ...                 static member definitions                      ...
// ......................................................................
//     
//    Arguments for constructor are as follows
//               name             mass          width         charge
//             2*spin           parity  C-conjugation
//          2*Isospin       2*Isospin3       G-parity
//               type    lepton number  baryon number   PDG encoding
//             stable         lifetime    decay table 

RichPhotoElectron RichPhotoElectron::theRichPhotoElectron(
		 "pe-",  0.51099906*MeV,       0.0*MeV,    -1.*eplus, 
		    1,               0,             0,          
		    0,               0,             0,             
	     "lepton",               1,             0,        9000011,
		 true,              -1,          NULL
);

RichPhotoElectron* RichPhotoElectron::PhotoElectronDefinition()
                       {return &theRichPhotoElectron;}

RichPhotoElectron* RichPhotoElectron::PhotoElectron()
{  
  return &theRichPhotoElectron; 
}
 





