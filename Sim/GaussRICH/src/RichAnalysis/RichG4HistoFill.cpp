// $Id: RichG4HistoFill.cpp,v 1.1 2003-04-29 15:04:54 seaso Exp $
// Include files 



// local
#include "RichG4HistoFill.h"
// GiGa 
#include "GiGa/GiGaMACROs.h"
// local
#include "../SensDet/RichG4Hit.h"
//GEANT4
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "RichG4SvcLocator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HistoFill
//
// 2003-03-20 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4HistoFill::RichG4HistoFill(  ) {

}
RichG4HistoFill::~RichG4HistoFill(  ) { 
}

void RichG4HistoFill:: FillRichG4Histo( const G4Event* anEvent,
                      int NumRichColl,  std::vector<int> RichG4CollectionID ) 
{
  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
  // the following line does nto work since this is not a gaudi algorithm
  // and hence svcLoc is not automatically defined. 
  // Hence the line above is used instead.

  // StatusCode sc = svcLoc()->service( "HistogramSvc", CurrentHistoSvc, true );

  SmartDataPtr<IHistogram1D>hNumTotHitRich1(CurrentHistoSvc,"RICHG4HISTO/1");
  SmartDataPtr<IHistogram2D>hGlobalPEOriginXY(CurrentHistoSvc,"RICHG4HISTO/2");
  SmartDataPtr<IHistogram2D>hGlobalHitXY(CurrentHistoSvc,"RICHG4HISTO/3");
  
  G4HCofThisEvent * HCE;
  G4int nHitTotRich1=0;
  G4int NumRichCollection= NumRichColl;
  
  int Current_RichG4CollectionID=0;
  
  
  for (int ihcol=0; ihcol<NumRichCollection; ihcol++) {
    Current_RichG4CollectionID =RichG4CollectionID[ihcol];
    
    if(Current_RichG4CollectionID >=0 ) {
      HCE = anEvent->GetHCofThisEvent();
      RichG4HitsCollection* RHC=NULL;
      if(HCE){
        RHC = (RichG4HitsCollection*)(HCE->GetHC( Current_RichG4CollectionID));
      }
      if(RHC){
        G4int nHitInCurColl = RHC->entries();
        //        G4cout<<"EndEvAction      "<< nHitInCurColl
        //       <<"   are stored in RichHitCollection set   "<<ihcol<< G4endl;
        if(ihcol == 0 || ihcol == 1 ) {
          nHitTotRich1 += nHitInCurColl;

        }

      }
    }
  }
  if (hNumTotHitRich1) hNumTotHitRich1->fill(nHitTotRich1*1.0, 1.0) ;

   // Now fill the hit coordinates into a histogram
  
  for (int ihcola=0; ihcola<NumRichColl; ihcola++) {
    G4cout<<"Now filling Rich1 hits into histogram  "<<ihcola <<G4endl;
    if(RichG4CollectionID[ihcola] >=0 ) {
      HCE = anEvent->GetHCofThisEvent();
      RichG4HitsCollection* RHCA=NULL;
      if(HCE){
        RHCA = (RichG4HitsCollection*)
       (HCE->GetHC(RichG4CollectionID[ihcola]));
      }
      if(RHCA){
        G4int nHitHCa = RHCA->entries();
        for (G4int iha=0; iha<nHitHCa; iha++ ) {
          RichG4Hit* aHit = (*RHCA)[iha];
          G4double aHitGlobalX=aHit->GetGlobalPos().x();
          G4double aHitGlobalY=aHit->GetGlobalPos().y();
          G4double aHitGlobalZ=aHit->GetGlobalPos().z();
          G4double aHitGlobalPEOriginX=aHit->GetGlobalPEOriginPos().x();
          G4double aHitGlobalPEOriginY=aHit->GetGlobalPEOriginPos().y();
          G4double aHitGlobalPEOriginZ=aHit->GetGlobalPEOriginPos().z();
          if(hGlobalPEOriginXY)hGlobalPEOriginXY->
                           fill(aHitGlobalPEOriginX,aHitGlobalPEOriginY,1.0);
          if(hGlobalHitXY) hGlobalHitXY->fill( aHitGlobalX,aHitGlobalY,1.0);

        }

      }
    }
  }

}

//=============================================================================
