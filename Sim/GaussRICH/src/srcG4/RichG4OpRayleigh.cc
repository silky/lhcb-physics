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
// $Id: RichG4OpRayleigh.cc,v 1.3 2004-02-04 13:52:24 seaso Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
////////////////////////////////////////////////////////////////////////
// Optical Photon Rayleigh Scattering Class Implementation
////////////////////////////////////////////////////////////////////////
//
// File:        RichG4OpRayleigh.cc 
// Description: Discrete Process -- Rayleigh scattering of optical 
//		photons  
// Version:     1.0
// Created:     1996-05-31  
// Author:      Juliet Armstrong
// Updated:     2001-10-18 by Peter Gumplinger
//              eliminate unused variable warning on Linux (gcc-2.95.2)
// Updated:     2001-09-18 by mma
//		>numOfMaterials=G4Material::GetNumberOfMaterials() in BuildPhy
// Updated:     2001-01-30 by Peter Gumplinger
//              > allow for positiv and negative CosTheta and force the
//              > new momentum direction to be in the same plane as the
//              > new and old polarization vectors
//              2001-01-29 by Peter Gumplinger
//              > fix calculation of SinTheta (from CosTheta)
//              1997-04-09 by Peter Gumplinger
//              > new physics/tracking scheme
// mail:        gum@triumf.ca
//
////////////////////////////////////////////////////////////////////////

#include "G4ios.hh"
#include "RichG4OpRayleigh.hh"
// Add a flag to tag the photon as rayleigh scattered. SE Oct 2003.
// this can be done from userstep action, but that will take more
// cpu time. Here it is switched off for default running mode. 
#include "RichG4RayleighTag.h"

/////////////////////////
// Class Implementation
/////////////////////////

        //////////////
        // Operators
        //////////////

// RichG4OpRayleigh::operator=(const RichG4OpRayleigh &right)
// {
// }

        /////////////////
        // Constructors
        /////////////////

RichG4OpRayleigh::RichG4OpRayleigh(const G4String& processName)
  : G4VDiscreteProcess(processName),
    fRichVerboseInfoTag(false)
{

        thePhysicsTable = 0;

        if (verboseLevel>0) {
           G4cout << GetProcessName() << " is created " << G4endl;
        }

        BuildThePhysicsTable();
}

// RichG4OpRayleigh::RichG4OpRayleigh(const RichG4OpRayleigh &right)
// {
// }

        ////////////////
        // Destructors
        ////////////////

RichG4OpRayleigh::~RichG4OpRayleigh()
{
        if (thePhysicsTable!= 0) {
           thePhysicsTable->clearAndDestroy();
           delete thePhysicsTable;
        }
}

        ////////////
        // Methods
        ////////////

// PostStepDoIt
// -------------
//
G4VParticleChange* 
RichG4OpRayleigh::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{
        aParticleChange.Initialize(aTrack);

        const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();

        if (verboseLevel>0) {
		G4cout << "Scattering Photon!" << G4endl;
		G4cout << "Old Momentum Direction: "
	     	     << aParticle->GetMomentumDirection() << G4endl;
		G4cout << "Old Polarization: "
		     << aParticle->GetPolarization() << G4endl;
	}

	// find polar angle w.r.t. old polarization vector

	G4double rand = G4UniformRand();

	G4double CosTheta = pow(rand, 1./3.);
	G4double SinTheta = sqrt(1.-CosTheta*CosTheta);

        if(G4UniformRand() < 0.5)CosTheta = -CosTheta;
        // Addtions made by SE to tag the photon as
        // rayleighscattered photon  Oct 2003.
        // This is put under a switch which is
        // by default switched off.
        if( fRichVerboseInfoTag) {  
          RichG4RayleighTag(aTrack);
        }
        
	// Addtions made by SE to save cpu time.
        // Kill the photon when its wavelength too low
        // (energy too high) 
        // and its step length too low
        // and it is scattering backwards. Also
        // done for stepnumber very large just to avoid any photon
        // going through an in infinite loop through the aerogel.
 
         // G4double totEn = aParticle->GetTotalEnergy();
         // const G4double CurStepLen=  aStep.GetStepLength();
          const G4int CurStepNum=  aTrack.GetCurrentStepNumber() ;

          //          if((CurStepLen < 1.0 *mm)  ||( CurStepNum > 2000 ) ) {
            
          //            if( ( totEn > (4.0/eV)) || ( CurStepNum > 5000 )  ) {
            if(CurStepNum > 5000   ) {
              
              if(CosTheta < 0.0 ) {

            G4cout<<" Optical Photon killed in Rayleigh after 5000 steps" 
                  <<G4endl;
          aParticleChange.SetStatusChange(fStopAndKill);
          return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);       

          
              }
              
              
            }
            
            
            
            //     }
          
          
          
          

        // end of adddtions by SE


	// find azimuthal angle w.r.t old polarization vector 

	rand = G4UniformRand();

	G4double Phi = twopi*rand;
	G4double SinPhi = sin(Phi); 
	G4double CosPhi = cos(Phi); 
	
	G4double unit_x = SinTheta * CosPhi; 
	G4double unit_y = SinTheta * SinPhi;  
	G4double unit_z = CosTheta; 
	
        G4ThreeVector NewPolarization (unit_x,unit_y,unit_z);

        // Rotate new polarization direction into global reference system 

	G4ThreeVector OldPolarization = aParticle->GetPolarization();
        OldPolarization = OldPolarization.unit();

	NewPolarization.rotateUz(OldPolarization);
        NewPolarization = NewPolarization.unit();
	
        // -- new momentum direction is normal to the new
        // polarization vector and in the same plane as the
        // old and new polarization vectors --

        G4ThreeVector NewMomentumDirection = 
                              OldPolarization - NewPolarization * CosTheta;

        if(G4UniformRand() < 0.5)NewMomentumDirection = -NewMomentumDirection;
        NewMomentumDirection = NewMomentumDirection.unit();

	aParticleChange.SetPolarizationChange(NewPolarization);

	aParticleChange.SetMomentumChange(NewMomentumDirection);

        if (verboseLevel>0) {
		G4cout << "New Polarization: " 
		     << NewPolarization << G4endl;
		G4cout << "Polarization Change: "
		     << *(aParticleChange.GetPolarizationChange()) << G4endl;  
		G4cout << "New Momentum Direction: " 
		     << NewMomentumDirection << G4endl;
		G4cout << "Momentum Change: "
		     << *(aParticleChange.GetMomentumChange()) << G4endl; 
	}

        return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

// BuildThePhysicsTable for the Rayleigh Scattering process
// --------------------------------------------------------
//
void RichG4OpRayleigh::BuildThePhysicsTable()
{
//      Builds a table of scattering lengths for each material

        if (thePhysicsTable) return;

        const G4MaterialTable* theMaterialTable=
                               G4Material::GetMaterialTable();
        G4int numOfMaterials = G4Material::GetNumberOfMaterials();

        // create a new physics table

        thePhysicsTable = new G4PhysicsTable(numOfMaterials);

        // loop for materials

        for (G4int i=0 ; i < numOfMaterials; i++)
        {
                G4PhysicsOrderedFreeVector* ScatteringLengths =
                                new G4PhysicsOrderedFreeVector();

                if ((*theMaterialTable)[i]->GetName() == "Water")
                {
			G4MaterialPropertiesTable *MaterialPT =
			(*theMaterialTable)[i]->GetMaterialPropertiesTable();
			// Call utility routine to Generate
			// Rayleigh Scattering Lengths
			ScatteringLengths =
				RayleighAttenuationLengthGenerator(MaterialPT);
		}

		thePhysicsTable->insertAt(i,ScatteringLengths);
	} 
}

// GetMeanFreePath()
// -----------------
//
G4double RichG4OpRayleigh::GetMeanFreePath(const G4Track& aTrack,
                                     G4double ,
                                     G4ForceCondition* )
{
        const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
        const G4Material* aMaterial = aTrack.GetMaterial();

        G4double thePhotonMomentum = aParticle->GetTotalMomentum();

        G4double AttenuationLength = DBL_MAX;

        if (aMaterial->GetName() == "Water") {

           G4bool isOutRange;

           AttenuationLength =
                (*thePhysicsTable)(aMaterial->GetIndex())->
                           GetValue(thePhotonMomentum, isOutRange);
        }
        else {

           G4MaterialPropertiesTable* aMaterialPropertyTable =
                           aMaterial->GetMaterialPropertiesTable();

           if(aMaterialPropertyTable){
             G4MaterialPropertyVector* AttenuationLengthVector =
                   aMaterialPropertyTable->GetProperty("RAYLEIGH");
             if(AttenuationLengthVector){
               AttenuationLength = AttenuationLengthVector ->
                                    GetProperty(thePhotonMomentum);
             }
             else{
//               G4cout << "No Rayleigh scattering length specified" << G4endl;
             }
           }
           else{
//             G4cout << "No Rayleigh scattering length specified" << G4endl; 
           }
        }

        return AttenuationLength;
}

// RayleighAttenuationLengthGenerator()
// ------------------------------------
// Private method to compute Rayleigh Scattering Lengths (for water)
//
G4PhysicsOrderedFreeVector* 
RichG4OpRayleigh::RayleighAttenuationLengthGenerator
     (G4MaterialPropertiesTable *aMPT) 
{
        // Physical Constants

        // isothermal compressibility of water
        G4double betat = 7.658e-23*m3/MeV;

        // K Boltzman
        G4double kboltz = 8.61739e-11*MeV/kelvin;

        // Temperature of water is 10 degrees celsius
        // conversion to kelvin:
        // TCelsius = TKelvin - 273.15 => 273.15 + 10 = 283.15
        G4double temp = 283.15*kelvin;

        // Retrieve vectors for refraction index
        // and photon momentum from the material properties table

        G4MaterialPropertyVector* Rindex = aMPT->GetProperty("RINDEX");

        G4double refsq;
        G4double e;
        G4double xlambda;
        G4double c1, c2, c3, c4;
        G4double Dist;
        G4double refraction_index;

        G4PhysicsOrderedFreeVector *RayleighScatteringLengths = 
				new G4PhysicsOrderedFreeVector();
        Rindex->ResetIterator();

        while (++(*Rindex)) {

                e = (Rindex->GetPhotonMomentum());

                refraction_index = Rindex->GetProperty();
                refsq = refraction_index*refraction_index;
                xlambda = h_Planck*c_light/e;

	        if (verboseLevel>0) {
        	        G4cout << Rindex->GetPhotonMomentum() << " MeV\t";
                	G4cout << xlambda << " mm\t";
		}

                c1 = 1 / (6.0 * pi);
                c2 = pow((2.0 * pi / xlambda), 4);
                c3 = pow( ( (refsq - 1.0) * (refsq + 2.0) / 3.0 ), 2);
                c4 = betat * temp * kboltz;

                Dist = 1.0 / (c1*c2*c3*c4);

	        if (verboseLevel>0) {
	                G4cout << Dist << " mm" << G4endl;
		}
                RayleighScatteringLengths->
			InsertValues(Rindex->GetPhotonMomentum(), Dist);
        }

	return RayleighScatteringLengths;
}
