#include "RichHpdSiEnergyLoss.h"
#include "G4Material.hh"
#include "G4ParticleDefinition.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4Electron.hh"
#include "RichPhotoElectron.h"
#include "RichPEInfoAttach.h"
#include "RichG4AnalysisPhotElec.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4GaussPathNames.h"
#include "Randomize.hh"
#include <algorithm>
#include <math.h>
#include <vector>
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "RichHpdProperties.h"

RichHpdSiEnergyLoss::RichHpdSiEnergyLoss(const G4String& processName,
                                        G4ProcessType   aType )

  : G4VEnergyLoss(processName, aType ),
    MinKineticEnergy(1.*keV),
    MipEnergyHpdSiEloss(1.0*GeV),
    finalRangeforSiDetStep(0.15*mm),
    PhElectronMaxEnergy(25.0*keV) 
{
  
  // The following three initializations moved to GiGaPhysConstructorHpd and InitializeHpdProcParam so that they can be
  // set through the options files. The defaults are as indicated below. SE 5-10-2007
  // the pixelchipefficiency is set to 1 for now. the  SiHitDetGlobalEff is the product of the other two efficiencies.
  //    SiHitDetGlobalEff(0.85), 
  //     m_HpdSiDetEff(0.85)
  //     _SiPixelChipEff(1.0)
  //  PeBackScaProb(0.005823) /*RWL change 8th Nov 06*/ {


  const G4String materialName="/dd/Materials/RichMaterials/RichHpdSilicon";
  const G4String EnvelopeMaterialName="/dd/Materials/RichMaterials/Kovar";



  static const G4MaterialTable* theMaterialTable =
    G4Material::GetMaterialTable();
  HpdSiElossMaterialName= materialName;
  HpdEnvelopeMaterialName =  EnvelopeMaterialName;
  G4int numberOfMat= theMaterialTable->size() ;
  G4int iMat, iMatk;
  for(iMat=0;iMat<numberOfMat;++iMat) {
    //    G4cout<<"G4mat name list "<<(*theMaterialTable)[iMat]->GetName()<<G4endl;

    if ( materialName == (*theMaterialTable)[iMat]->GetName()){
      fMatIndexHpdSiEloss=(*theMaterialTable)[iMat]->GetIndex();
      //        G4cout<<"Hpd Si energy Loss construt Material "<<materialName
      //    <<"   "<< fMatIndexHpdSiEloss<<G4endl;
      break;
    }

  }
  if(iMat >= numberOfMat ) {
    G4Exception("Invalid material Name in RichHpdSiEnergyLoss constructor" );
  }

  for(iMatk=0;iMatk<numberOfMat;++iMatk) {
    //    G4cout<<"G4mat name list "<<(*theMaterialTable)[iMatk]->GetName()<<G4endl;

    if ( EnvelopeMaterialName == (*theMaterialTable)[iMatk]->GetName()){
      fMatIndexHpdEnvelopeKovar=(*theMaterialTable)[iMatk]->GetIndex();
      //              G4cout<<"Hpd Si energy Loss construt Material "<<materialName
      //           <<"   "<< fMatIndexHpdEnvelopeKovar<<G4endl;
      break;
    }

  }
  if(iMatk >= numberOfMat ) {
    G4Exception("Invalid material Name in RichHpdSiEnergyLoss constructor" );
  }
   
  //     G4cout<<GetProcessName() <<" is created "<<G4endl;

}
RichHpdSiEnergyLoss::~RichHpdSiEnergyLoss() {; }

void RichHpdSiEnergyLoss::InitializeHpdProcParam(){

    RichHpdProperties*  m_HpdProperty = RichHpdProperties::getRichHpdPropertiesInstance();
    m_HpdProperty->InitializeSiDetParam();
    m_siliconDetXSize = (G4double) (m_HpdProperty->siDetXSize());
    m_siliconDetYSize = (G4double) (m_HpdProperty->siDetYSize());
    m_siliconDetZSize = (G4double) (m_HpdProperty->siDetZSize());  

    SiHitDetGlobalEff= m_HpdSiDetEff*m_SiPixelChipEff;


      //input value is measured back-hit fraction
      //convert to BScatter probability using efficiency,
      //also can do using sum of GS in inverse
 
    G4double HPDBSTotalProbSum = PeBackScaProb*SiHitDetGlobalEff;
    G4double HPDBSProbSum = HPDBSTotalProbSum/(1-SiHitDetGlobalEff);
    PeBackScaProbCorrected = HPDBSProbSum/
                            (SiHitDetGlobalEff+
                             HPDBSProbSum*(1-SiHitDetGlobalEff));

    G4cout<<"Rich Hpd SiHitEfficiency  BackScatterCorrectedProb "<< SiHitDetGlobalEff <<"   "<<PeBackScaProbCorrected<<G4endl;

}


G4bool RichHpdSiEnergyLoss::IsApplicable(const G4ParticleDefinition&
                                         aParticleType) {

   return(aParticleType.GetPDGCharge()!= 0.);
   //  return(( aParticleType.GetPDGCharge()!= 0.) &&
   //      ( (aParticleType.GetParticleName() == "e-") ||
   //       (aParticleType.GetParticleName() == "pe-")) );
  // return(( aParticleType.GetPDGCharge()!= 0.) &&
  //       (aParticleType.GetParticleName() == "e-")) ;

}

G4double RichHpdSiEnergyLoss::GetContinuousStepLimit(const G4Track& track,
                                                     G4double /*previousStepSize*/,
                                                     G4double /*currentMinimumStep*/,
                                                     G4double& /*currentSafety*/ ) {

  G4double  RangeForStep =  finalRangeforSiDetStep;

  if( fMatIndexHpdSiEloss != (G4int) track.GetMaterial() -> GetIndex() &&
      fMatIndexHpdEnvelopeKovar != (G4int) track.GetMaterial() -> GetIndex() ) {
    RangeForStep = DBL_MAX;
  }


  return RangeForStep;

}

G4double  RichHpdSiEnergyLoss::GetMeanFreePath(const G4Track& /*track*/,
                                               G4double /*previousStepSize*/,
                                               G4ForceCondition* condition) {
  // return infinity so that it does nothing.
  *condition = NotForced;
  return DBL_MAX;

}
G4VParticleChange*  RichHpdSiEnergyLoss::PostStepDoIt(const G4Track& aTrack,
                                                      const G4Step& aStep) {
  // Do nothing
  aParticleChange.Initialize(aTrack) ;
  return G4VContinuousDiscreteProcess::PostStepDoIt(aTrack,aStep);

}
G4VParticleChange* RichHpdSiEnergyLoss::AlongStepDoIt(const G4Track& aTrack,
                                                      const G4Step& aStep) {

  aParticleChange.Initialize(aTrack);



  G4int aMaterialIndex = aTrack.GetMaterial()->GetIndex();
  if(fMatIndexHpdSiEloss !=  aMaterialIndex  &&
     fMatIndexHpdEnvelopeKovar != aMaterialIndex  ) {
    return &aParticleChange;
  }
  //  if(fMatIndexHpdSiEloss !=  (G4int) aTrack.GetMaterial()->GetIndex() &&
  //  fMatIndexHpdEnvelopeKovar != (G4int) aTrack.GetMaterial() -> GetIndex() ) {
  //  return &aParticleChange;
  // }

  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  G4double aKinEnergyInit = aParticle->GetKineticEnergy();
  G4String  aCreatorProcessName= "NullProcess";
  const G4VProcess* aProcess = aTrack.GetCreatorProcess();
  if(aProcess) aCreatorProcessName =  aProcess->GetProcessName();




  //    G4cout<<"Now particle  "<<aParticle->GetDefinition()->GetParticleName()
  //       << "in HpdEnergyloss KE= "<<aKinEnergyInit <<G4endl;
  // if the Photoelectron hits the kovar of the Hpd envelope
  // endcap or barrel,  then the kill the photoelectron.
  // G4ParticleDefinition* aParticleDef = aParticle-> GetDefinition();
      // start of test printout
  // if( fMatIndexHpdEnvelopeKovar == 
  //        (G4int) aTrack.GetMaterial() -> GetIndex()) {
  //
  //        G4String aparticleName= aParticle-> GetDefinition()->GetParticleName();

	//	if(  aCreatorProcessName != "RichHpdPhotoelectricProcess") {
	  //        G4cout<<" Hpd energyloss Particle name: creatorProc energy "
	  //    << aparticleName<<" in  "<< aTrack.GetMaterial() ->GetName()
	  //    <<"   from " << aCreatorProcessName
          //    << "   "<<aKinEnergyInit<< G4endl;
	//	}
     //   G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
     //   G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
     //   const G4ThreeVector prePos= pPreStepPoint->GetPosition();
     //   const G4ThreeVector postPos= pPostStepPoint->GetPosition();
     //   G4Navigator* theNavigator =
     //    G4TransportationManager::GetTransportationManager()->
     //        GetNavigatorForTracking();
     //    G4ThreeVector aLocalPostPos =
     //     theNavigator-> GetGlobalToLocalTransform().
     //                    TransformPoint(postPos);
     //    G4ThreeVector aLocalPrePos =
     //    theNavigator-> GetGlobalToLocalTransform().
     //                  TransformPoint(prePos);
     //    G4ThreeVector aGlobalVertexPos= aTrack.GetVertexPosition();
    
     //    G4ThreeVector aLocalVertexPos =  theNavigator-> 
     //    GetGlobalToLocalTransform(). TransformPoint( aGlobalVertexPos);
     //  
     //    G4ThreeVector aLocalDirUnorm =  (aLocalPrePos- aLocalVertexPos);
     //    G4ThreeVector aLocalDir =  aLocalDirUnorm.unit();
     // 
     //   G4cout<<" pe Global pre post pos "<< prePos.x()<<"   "<< prePos.y()<<"   "
     //      << prePos.z()<<"   "<< postPos.x()<<"   "<< postPos.y()
     //      <<"   "<<postPos.z()<<G4endl;
     //   G4cout<<" pe Local pre post pos "<<aLocalPrePos.x()
     //         <<"  "<<aLocalPrePos.y()
     //         <<"   "<<aLocalPrePos.z()<<"   "<<aLocalPostPos.x()<<"   "
     //         <<aLocalPostPos.y()<<"   "<<aLocalPostPos.z()<<G4endl;
     //   G4cout<<" pe vertex pos unnorm local dir "<<  aLocalVertexPos.x()<<"   "
     //     << aLocalVertexPos.y()<<"   "<<aLocalVertexPos.z()<<"   "
     //     << aLocalDirUnorm.x()<<"   "<<aLocalDirUnorm.y()<<"    "
     //     <<  aLocalDirUnorm.z()<<G4endl;
     // 
     //  G4cout<<"pe vertex local dir normalized "<< aLocalDir.x()<<"   "
     //        << aLocalDir.y()<<"   "<<aLocalDir.z()<<G4endl;
     // }
   
   
  
      // end of test printout.
   //  G4String  aCreatorProcessName= "NullProcess";
   //const G4VProcess* aProcess = aTrack.GetCreatorProcess();
   //if(aProcess) aCreatorProcessName =  aProcess->GetProcessName();
  if( fMatIndexHpdEnvelopeKovar == (G4int) aTrack.GetMaterial() -> GetIndex()) {
    //  G4cout<<"particle in HPD Kovar "<<G4endl;
    if(aCreatorProcessName == "RichHpdPhotoelectricProcess" ) {
      aParticleChange.ProposeTrackStatus(fStopAndKill);
    }
    return &aParticleChange;
  }


  // if any charged particle (which can be a photoelectron or any other
  // charged particle) hits the silicon detector of the hpd, then store some
  // energy in the Silicon detector of the hpd.
  G4double Eloss, aKinEnergyFinal;
  // the following is just an initial fast approximation of the
  // energy loss in Silicon. This need to be modified in the future.
  // The following does not yet simulate the 1/beta**2 dependence of
  // the energy loss with respect to the incident energy.
  // The following is sufficient for the normal photoelectron
  // incidence on the Silicon detector.
  //
  // temporary fix until the magnetic shielding is implemented in
  //Gauss. SE 14-3-2003. The energy of the photoelectron is made to
  // be 2 GeV just to avoid the problem of field in richhpdphotoelectric effect.
  // once the shielding is implemented, this will be put back to 20 keV.


  // The following factor is  a temporary fix to distringuish
  // between the energy depostied by other proceseses and this
  // process. The other processes typicaly deposit about 60 KeV
  // So this process deposits 20*10 = 200 keV.
  // SE June 2004. now using RichPhotoelectron where no further deposit of
  // energy. Hence the energy deposited on the hit is 20 keV.
  G4double EnegydepositMultFactor=1.0;

  if(aCreatorProcessName == "RichHpdPhotoelectricProcess" ) {
    if(aKinEnergyInit > 1500 ) {
      aKinEnergyInit= aKinEnergyInit/100000;
      // EnegydepositMultFactor=10.0;
    }    
  } else {
    //    G4cout<<" Now a non pe particle in Hpd Si Det" <<G4endl;
  }
    
  //end of temporary fix.
  

  if(aKinEnergyInit < MinKineticEnergy ) {  Eloss=0.0 ; }
  else if( aKinEnergyInit <= PhElectronMaxEnergy ) {Eloss= aKinEnergyInit ;}
  else { Eloss = std::min(aKinEnergyInit,MipEnergyHpdSiEloss); }

  aKinEnergyFinal=aKinEnergyInit-Eloss;

  // Now for the hit detection effeciency and backscattering etc.

  //    aParticleChange.SetLocalEnergyDeposit(Eloss);

  //const G4double aEnergyAlreadyDeposit = aStep.GetTotalEnergyDeposit();
  //   cout<<"Hpd sidet Energy already deposited  "
  //             <<aEnergyAlreadyDeposit<<endl;
  //  G4cout<<"HpdEnergyloss: pe Energy before transfer "<<Eloss<<G4endl;
  
  G4double EnergyTransfer=
    EnegydepositMultFactor* RichHpdSiEnergyDeposit(Eloss);
  //     G4cout<<"EnergyTransfer in sidetEloss " << EnergyTransfer<<G4endl;


  if(   EnergyTransfer > 0.0 ) {

    aParticleChange.ProposeLocalEnergyDeposit(EnergyTransfer);
  }else if (aKinEnergyInit >= MipEnergyHpdSiEloss  ) {
    
    aParticleChange.ProposeLocalEnergyDeposit(Eloss);
  }else {
      //RWL change 8th Nov 2006, add backscatter basic model    
      //backscatter calculation
      G4double Randreflect =  G4UniformRand();
      //new rand # to ensure incoherant effects
      
      //input value is measured back-hit fraction
      //convert to BScatter probability using efficiency,
      //also can do using sum of GS in inverse

      if(Randreflect <= PeBackScaProbCorrected)
        {

	  //create new photoelectron, kill old photoelectron

          //work out new direction for pe, straight down

          G4double RandXposition =  G4UniformRand() -0.5;
          G4double RandYposition =  G4UniformRand() -0.5;
          //new rand # to ensure incoherant effects

          G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
          G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
 

          G4ThreeVector LocalElectronDirection( 0,0 ,-1 );
          //1mm above Si detector, heading straight down

          G4ThreeVector LocalElectronOrigin( RandXposition*m_siliconDetXSize ,
                                             RandYposition*m_siliconDetYSize , 0.5*m_siliconDetZSize+1.0);

          
      	  
         //transform to global co-ordinates
         G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

         G4ThreeVector GlobalElectronOrigin = theNavigator->GetLocalToGlobalTransform().TransformPoint(LocalElectronOrigin);

         const G4ThreeVector GlobalElectronDirection = theNavigator->
                                GetLocalToGlobalTransform().TransformAxis(LocalElectronDirection);
  
          //create new pe with same energy

          G4double aPElectronTime= aTrack.GetGlobalTime();

 
          //G4DynamicParticle* aElectron= 
          //   new G4DynamicParticle (RichPhotoElectron::PhotoElectron(),
          //                      GlobalElectronDirection, aKinEnergyInit) ;
          // for now use the g4electron with the current G4.. SE Oct4-2007
          G4DynamicParticle* aElectron= 
             new G4DynamicParticle (G4Electron::Electron(),
                                GlobalElectronDirection, aKinEnergyInit) ;

          aParticleChange.SetNumberOfSecondaries(1) ;
	  
          G4Track * aSecPETrack =
	             new G4Track(aElectron,aPElectronTime,GlobalElectronOrigin);
	    
          aSecPETrack->SetTouchableHandle((G4VTouchable*)0);
          aSecPETrack->SetParentID(aTrack.GetTrackID());
          aSecPETrack->SetGoodForTrackingFlag(true);
          //check this info attach
          //G4Track* aTaggedSecPETrack = RichPEInfoAttach(aTrack,aSecPETrack);

           G4Track* aTaggedSecPETrack =  RichPEBackScatAttach(aTrack,aSecPETrack);             
           aParticleChange.AddSecondary(aTaggedSecPETrack);


          //kill old pe
          aParticleChange.ProposeTrackStatus(fStopAndKill);
          aParticleChange.ProposeEnergy(0.0);

          aKinEnergyFinal=0.0;

          
        }
      
      
      
      
  }
  
  
  
  
  //end of change by RWL 8th Nov 2006




   if ( (aKinEnergyFinal <= MinKineticEnergy) || 
      (aCreatorProcessName == "RichHpdPhotoelectricProcess") ||  (aCreatorProcessName =="RichHpdSiEnergyLossProcess")) {
       aParticleChange.ProposeTrackStatus(fStopAndKill);
       aParticleChange.ProposeEnergy(0.0);

   }else {
       aParticleChange.ProposeEnergy(aKinEnergyFinal);
 
   }



  
  return &aParticleChange;

}
G4double RichHpdSiEnergyLoss::RichHpdSiEnergyDeposit(G4double   ElossInput)
{
  G4double NetEnergyTransfer =    ElossInput;

  G4double Effrandom =  G4UniformRand();
  //  cout<<"SiEnergy dep  EFFR GloablEFF  "
  //    << Effrandom << "  "<< SiHitDetGlobalEff<<endl;

  if( Effrandom >  SiHitDetGlobalEff ) {

    NetEnergyTransfer= 0.0;

  }


  //  G4double EnergyFromPe =  ElossInput;

  // G4double EnergyTransfer = RichHpdSiBackScatter(  EnergyFromPe)
  //  NetEnergyTransfer =  EnergyTransfer;


  return NetEnergyTransfer;

}








