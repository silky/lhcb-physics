// ============================================================================
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// Geant4 
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4ios.hh"
/// local
#include "GiGaSensDetVelo.h"

// ============================================================================
IMPLEMENT_GiGaFactory( GiGaSensDetVelo );
// ============================================================================


GiGaSensDetVelo::GiGaSensDetVelo
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaSensDetBase     ( type , name , parent ) 
  , G4VSensitiveDetector( name  )
{  
  G4String HCname;
  collectionName.insert(HCname="Hits");
};

// ============================================================================

GiGaSensDetVelo::~GiGaSensDetVelo(){};

// ============================================================================

void GiGaSensDetVelo::Initialize(G4HCofThisEvent*HCE)
{
  //static 
  int HCID;
  
  veloCol= new VeloHitsCollection
    (SensitiveDetectorName,collectionName[0]);
  
  HCID=G4SDManager::GetSDMpointer()
    ->GetCollectionID(SensitiveDetectorName + "/" + collectionName[0]);      
  
  HCE->AddHitsCollection(HCID,veloCol);
}


bool GiGaSensDetVelo::ProcessHits( G4Step* step , 
                                    G4TouchableHistory* /* history */ ) 
{
  if( 0 == step ) { return false ; } 
  
  G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();

  if(charge!=0.0)
    {
      double timeof = step->GetTrack()-> GetGlobalTime();
      double edep = step->GetTotalEnergyDeposit();
      HepPoint3D postpos  = step->GetPostStepPoint()->GetPosition();
      HepPoint3D prepos  = step->GetPreStepPoint()->GetPosition();
      int trid = step->GetTrack()->GetTrackID();

      // temp  
      G4TouchableHistory* TT =  
      (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
      G4VPhysicalVolume*  PV =   TT->GetVolume();
      G4LogicalVolume*    LV =   PV->GetLogicalVolume();

      G4TouchableHistory* postTT =  
      (G4TouchableHistory*)(step->GetPostStepPoint()->GetTouchable());
      G4VPhysicalVolume*  postPV =   postTT->GetVolume();
      G4LogicalVolume*    postLV =   postPV->GetLogicalVolume();

      MsgStream log( msgSvc() , name() );
      
      log << MSG::DEBUG << "Processing VeloHit:" << " edep="  
      << edep << endreq;

      std::string pvname=PV->GetName();
      G4TouchableHistory* MotherTT = TT;
      MotherTT ->MoveUpHistory(1);

      std::string stname=MotherTT -> GetVolume()->GetName();
      
      int numsens;      
      int puornot=0;
      
      // mod for new xml

      if(SensitiveDetectorName=="VeloPuSDet") puornot=1;

      // old version:
      //      if(stname.substr(25,1)=="P") puornot=1;

      // to find the position of "Station" in vol name

      int stpos=stname.rfind("Station")+7;

      if(puornot==0)
        {
          // normal sensor
          std::string stnumb=stname.substr(stpos,2);
          std::string lorr=stname.substr(stpos,1);
          std::string phiorr=pvname.substr(36,2);
          
          int ishift1=0;
          int ishift2=0;
          
          if(lorr=="L") ishift1=2;
          if(phiorr=="UP" || phiorr=="DR") ishift2=1;
          numsens=4*atoi(stnumb.c_str())+ishift1+ishift2;
        }
      else
        {
          // PuVeto sensor
          std::string stnumb=stname.substr(stpos,1);
          std::string lorr=stname.substr(stpos,1);
          
          int ishift=0;
          
          if(lorr=="L") ishift=1;
          numsens=98+2*atoi(stnumb.c_str())+ishift;
        }

      
      log << MSG::DEBUG << "Mother: " << stname << " sensor number: " 
          << numsens << endreq;      

      log << MSG::DEBUG 
      << " PrePos=("  << prepos.x() << "," << prepos.y() << "," << prepos.z() 
      << ")" 
      << " PrePV="    << pvname  
      << " PreLV="    << LV->GetName() << endreq;

      log << MSG::DEBUG 
      << " PostPos=("
      << postpos.x() << "," << postpos.y() << "," << postpos.z() << ")" 
      << " PostPV="    << postPV->GetName()  
      << " PostLV="    << postLV->GetName() << endreq;
      // end of temp
  
      ///
      VeloHit* newHit = new VeloHit();
      newHit->SetEdep( edep );
      newHit->SetEntryPos( prepos );
      newHit->SetExitPos( postpos );
      newHit->SetTimeOfFlight( timeof );  
      newHit->SetTrackID( trid );
      newHit->SetSensor(numsens);
      ///

      //  newHit->Print();
      veloCol->insert( newHit );
    }
     return false;
     
};
// ============================================================================


