// $Id: CherenkovG4HistoFillSet4.cpp,v 1.6 2009-07-03 11:59:49 seaso Exp $
// Include files



// local
#include "CherenkovG4HistoFillSet4.h"
#include "RichG4Counters.h"
// local
#include "CkvG4Hit.h"
#include "RichG4ReconResult.h"

//GEANT4
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
//#include "AIDA/IProfile.h"
#include "RichG4SvcLocator.h"


//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4HistoFillSet4
//
// 2003-09-22 : Sajan EASO
//-----------------------------------------------------------------------------
//

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4HistoFillSet4::CherenkovG4HistoFillSet4(  ) {

}
CherenkovG4HistoFillSet4::~CherenkovG4HistoFillSet4(  ) {
}
void CherenkovG4HistoFillSet4::FillRichG4HistoSet4(CkvG4Hit* acHit,
                                              RichG4ReconResult* aReconResult)
{
  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();

  SmartDataPtr<IHistogram1D>hCkvRich1GasD3E1 (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1500");
  SmartDataPtr<IHistogram1D>hCkvRich1GasD3E1A (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1500A");
  SmartDataPtr<IHistogram1D>hCkvRich1GasD4E1 (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1512");
  SmartDataPtr<IHistogram1D>hCkvRich1GasQwPh (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1514");

  SmartDataPtr<IHistogram1D>hCkvRich1GasD1E4 (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1501");
  SmartDataPtr<IHistogram1D>hCkvRich1GasGen (CurrentHistoSvc,
                                             "RICHG4HISTOSET4/1504");
  SmartDataPtr<IHistogram1D>hCkvRich1GasRes (CurrentHistoSvc,
                                             "RICHG4HISTOSET4/1510");
  SmartDataPtr<IHistogram1D>hCkvRich1GasResConst (CurrentHistoSvc,
                                                  "RICHG4HISTOSET4/1511");
  SmartDataPtr<IHistogram1D>hCkvRich1AgelD3E3 (CurrentHistoSvc,
                                               "RICHG4HISTOSET4/1600");
  SmartDataPtr<IHistogram1D>hCkvRich1AgelD1E4 (CurrentHistoSvc,
                                               "RICHG4HISTOSET4/1601");
  SmartDataPtr<IHistogram1D>hCkvRich1AgelGen (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1604");
  SmartDataPtr<IHistogram1D>hCkvRich1AgelRes (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1610");
  SmartDataPtr<IHistogram1D>hCkvRich1AgelResConst (CurrentHistoSvc,
                                                   "RICHG4HISTOSET4/1611");
  SmartDataPtr<IHistogram1D>hCkvRich2GasD3E1 (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1700");
  SmartDataPtr<IHistogram1D>hCkvRich2GasD3E1A (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1700A");
  SmartDataPtr<IHistogram1D>hCkvRich2GasD4E1 (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1712");
  SmartDataPtr<IHistogram1D>hCkvRich2GasQwPh (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1714");
  SmartDataPtr<IHistogram1D>hCkvRich2GasD1E4 (CurrentHistoSvc,
                                              "RICHG4HISTOSET4/1701");
  SmartDataPtr<IHistogram1D>hCkvRich2GasGen (CurrentHistoSvc,
                                             "RICHG4HISTOSET4/1704");
  SmartDataPtr<IHistogram1D>hCkvRich2GasRes (CurrentHistoSvc,
                                             "RICHG4HISTOSET4/1710");
  SmartDataPtr<IHistogram1D>hCkvRich2GasResConst (CurrentHistoSvc,
                                                  "RICHG4HISTOSET4/1711");


  SmartDataPtr<IHistogram2D>hCkvRich1GasCkvPhi(CurrentHistoSvc,
                                               "RICHG4HISTOSET4/1520");
  SmartDataPtr<IHistogram2D>hCkvRich1AgelCkvPhi( CurrentHistoSvc,
                                                 "RICHG4HISTOSET4/1620");
  SmartDataPtr<IHistogram2D>hCkvRich2GasCkvPhi(CurrentHistoSvc,
                                               "RICHG4HISTOSET4/1720");

  //  SmartDataPtr<IProfile>hCkvRich1GasCkvResPhiProf(CurrentHistoSvc,
  //                                        "RICHG4HISTOSET4/1522");
  // SmartDataPtr<IProfile>hCkvRich1AgelCkvResPhiProf( CurrentHistoSvc,
  //                                         "RICHG4HISTOSET4/1622");
  // SmartDataPtr<IProfile>hCkvRich2GasCkvResPhiProf(CurrentHistoSvc,
  //                                         "RICHG4HISTOSET4/1722");


  SmartDataPtr<IHistogram1D>hCkvRich1GasResEmis (CurrentHistoSvc,
                                                 "RICHG4HISTOSET4/1530");
  SmartDataPtr<IHistogram1D>hCkvRich1AgelResEmis (CurrentHistoSvc,
                                                  "RICHG4HISTOSET4/1630");
  SmartDataPtr<IHistogram1D>hCkvRich2GasResEmis (CurrentHistoSvc,
                                                 "RICHG4HISTOSET4/1730");
  SmartDataPtr<IHistogram1D>hCkvRich1GasResPixel (CurrentHistoSvc,
                                                  "RICHG4HISTOSET4/1540");
  SmartDataPtr<IHistogram1D>hCkvRich1AgelResPixel (CurrentHistoSvc,
                                                   "RICHG4HISTOSET4/1640");
  SmartDataPtr<IHistogram1D>hCkvRich2GasResPixel (CurrentHistoSvc,
                                                  "RICHG4HISTOSET4/1740");



  SmartDataPtr<IHistogram1D>hCkvRich1GasResPsf (CurrentHistoSvc,
                                                "RICHG4HISTOSET4/1550");
  SmartDataPtr<IHistogram1D>hCkvRich1AgelResPsf (CurrentHistoSvc,
                                                 "RICHG4HISTOSET4/1650");
  SmartDataPtr<IHistogram1D>hCkvRich2GasResPsf (CurrentHistoSvc,
                                                "RICHG4HISTOSET4/1750");

  SmartDataPtr<IHistogram1D>hCkvRich1GasResTotal (CurrentHistoSvc,
                                                  "RICHG4HISTOSET4/1560");
  SmartDataPtr<IHistogram1D>hCkvRich1AgelResTotal (CurrentHistoSvc,
                                                   "RICHG4HISTOSET4/1660");
  SmartDataPtr<IHistogram1D>hCkvRich2GasResTotal (CurrentHistoSvc,
                                                  "RICHG4HISTOSET4/1760");

  SmartDataPtr<IHistogram1D>hCkvRich1AgelExitResRefraction (CurrentHistoSvc,
                                                            "RICHG4HISTOSET4/1635");


  RichG4RadiatorMaterialIdValues* aRMIdValues =
      RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();

  int curRdet =   acHit-> GetCurRichDetNum();
  int curRadiator= acHit-> GetRadiatorNumber();
  double genckv =   acHit-> ThetaCkvAtProd();
  double genckvphi = acHit->  PhiCkvAtProd();

  //  std::cout<<" Histofill set4 rdet radia genckv "
  //         << curRdet<<"  "<< curRadiator<<"  "
  //         << genckv <<std::endl;


  // now the reconstructed results.

  double aCkvRecD1E1 = aReconResult-> ckvAngleD1E1();
  double aCkvRecD2E1 = aReconResult-> ckvAngleD2E1();
  double aCkvRecD3E1 = aReconResult-> ckvAngleD3E1();


  //  double aCkvRecD3E2 = aReconResult-> ckvAngleD3E2();
  //double aCkvRecD1E2 = aReconResult-> ckvAngleD1E2();
  double aCkvRecD3E4 = aReconResult-> ckvAngleD3E4();
  double aCkvRecD1E4 = aReconResult-> ckvAngleD1E4();

  double aCkvRecD1E3 = aReconResult-> ckvAngleD1E3();
  double aCkvRecD2E3 = aReconResult-> ckvAngleD2E3();
  double aCkvRecD3E3 = aReconResult-> ckvAngleD3E3();

  double aCkvRecD4E1 = aReconResult-> ckvAngleD4E1();
  double aCkvRecD4E4 = aReconResult-> ckvAngleD4E4();


  if( (curRdet == 0) &&  (aRMIdValues-> IsRich1AerogelAnyTileRad(curRadiator))) {
    //    std::cout<< "histo id "<<hCkvRich1Agel<<"   "
    // <<  hCkvRich1Gas<<std::endl;

    if(hCkvRich1AgelD3E3)hCkvRich1AgelD3E3->fill(aCkvRecD3E3) ;
    if(hCkvRich1AgelD1E4)hCkvRich1AgelD1E4->fill(aCkvRecD1E4) ;

    if(hCkvRich1AgelGen)hCkvRich1AgelGen->fill(genckv) ;

    if(hCkvRich1AgelRes) hCkvRich1AgelRes-> fill(aCkvRecD3E3-genckv) ;
    if(hCkvRich1AgelResEmis)hCkvRich1AgelResEmis->fill(aCkvRecD3E4-aCkvRecD3E3);
    if(  hCkvRich1AgelResPixel) hCkvRich1AgelResPixel->
                                  fill(aCkvRecD1E3 - aCkvRecD2E3);
    if(  hCkvRich1AgelResPsf)   hCkvRich1AgelResPsf->
                                  fill(aCkvRecD2E3-aCkvRecD3E3);


    if(hCkvRich1AgelResTotal) hCkvRich1AgelResTotal->
                                fill(aCkvRecD1E4-genckv);

    if(  hCkvRich1AgelExitResRefraction)  hCkvRich1AgelExitResRefraction->
                                            fill(aCkvRecD3E1-aCkvRecD3E3);

    if(genckv > 0.255 && genckv < 0.257 ) {

      if(hCkvRich1AgelResConst) hCkvRich1AgelResConst->
                                  fill(aCkvRecD3E3-genckv) ;
      if(hCkvRich1AgelCkvPhi) hCkvRich1AgelCkvPhi->
                                fill(genckvphi,aCkvRecD3E3-genckv);


      //   if(hCkvRich1AgelCkvResPhiProf) hCkvRich1AgelCkvResPhiProf->
      //            fill(genckvphi,aCkvRecD3E3-genckv);

    }


    //    if(hCkvRich1AgelCkvPhi) hCkvRich1AgelCkvPhi->
    //             fill(aCkvRecD3E3-genckv,genckvphi);

  }
  if((curRdet == 0) &&   (curRadiator== (aRMIdValues-> Rich1GaseousCkvRadiatorNum()))) {
    if(hCkvRich1GasD3E1) hCkvRich1GasD3E1->fill(aCkvRecD3E1) ;
    //if(hCkvRich1GasD3E1A) hCkvRich1GasD3E1A->fill(aCkvRecD3E1) ;
    if(hCkvRich1GasD3E1A) hCkvRich1GasD3E1A->fill(aCkvRecD4E1) ;
    if(hCkvRich1GasD4E1) hCkvRich1GasD4E1->fill(aCkvRecD4E1) ;

    if(hCkvRich1GasD1E4) hCkvRich1GasD1E4->fill(aCkvRecD1E4) ;
    if(hCkvRich1GasGen) hCkvRich1GasGen->fill(genckv) ;
    if(hCkvRich1GasRes) hCkvRich1GasRes->fill(aCkvRecD3E1-genckv) ;
    //if(hCkvRich1GasResEmis)hCkvRich1GasResEmis->fill(aCkvRecD3E4-aCkvRecD3E1);
    if(hCkvRich1GasResEmis)hCkvRich1GasResEmis->fill(aCkvRecD4E4-aCkvRecD4E1);
    if( hCkvRich1GasResPixel) hCkvRich1GasResPixel->
                                fill(aCkvRecD1E1-aCkvRecD2E1);
    if(   hCkvRich1GasResPsf)  hCkvRich1GasResPsf->
                                 fill(aCkvRecD2E1-aCkvRecD3E1);

    if( hCkvRich1GasResTotal) hCkvRich1GasResTotal->
                                fill(aCkvRecD1E4-genckv);

   if(hCkvRich1GasQwPh)hCkvRich1GasQwPh->fill(aCkvRecD4E1- aCkvRecD3E1  );

    if(genckv > 0.050 && genckv < 0.052 ) {

      if(hCkvRich1GasResConst) hCkvRich1GasResConst->
                                 fill(aCkvRecD3E1-genckv) ;
      if(hCkvRich1GasCkvPhi)hCkvRich1GasCkvPhi->
                              fill(genckvphi,aCkvRecD3E1-genckv);
      //     if(hCkvRich1GasCkvResPhiProf)hCkvRich1GasCkvResPhiProf->
      //            fill(genckvphi,aCkvRecD3E1-genckv);
    }

    //   if(hCkvRich1GasCkvPhi)hCkvRich1GasCkvPhi->
    //             fill(aCkvRecD3E1-genckv,genckvphi);

  }
  if( (curRdet == 1) && (curRadiator== (aRMIdValues-> Rich2GaseousCkvRadiatorNum() ) ) ){

    // G4cout<<" Rich2 reconstructed Ckv Angle D3E1 D1E4 "<< aCkvRecD3E1<<"   "<<aCkvRecD1E4<<G4endl;
    
    if(hCkvRich2GasD3E1) hCkvRich2GasD3E1->fill(aCkvRecD3E1) ;
    if(hCkvRich2GasD4E1) hCkvRich2GasD4E1->fill(aCkvRecD4E1) ;
    //  if(hCkvRich2GasD3E1A) hCkvRich2GasD3E1A->fill(aCkvRecD3E1) ;
    if(hCkvRich2GasD3E1A) hCkvRich2GasD3E1A->fill(aCkvRecD4E1) ;
    if(hCkvRich2GasD1E4) hCkvRich2GasD1E4->fill(aCkvRecD1E4) ;
    if(hCkvRich2GasGen) hCkvRich2GasGen->fill(genckv) ;
    if(hCkvRich2GasRes) hCkvRich2GasRes->fill(aCkvRecD3E1-genckv) ;
    //if(hCkvRich2GasResEmis)hCkvRich2GasResEmis->fill(aCkvRecD3E4-aCkvRecD3E1);
    if(hCkvRich2GasResEmis)hCkvRich2GasResEmis->fill(aCkvRecD4E4-aCkvRecD4E1);
    if( hCkvRich2GasResPixel) hCkvRich2GasResPixel->
                                fill(aCkvRecD1E1-aCkvRecD2E1);

    if(   hCkvRich2GasResPsf)  hCkvRich2GasResPsf->
                                 fill(aCkvRecD2E1-aCkvRecD3E1);

    if( hCkvRich2GasResTotal) hCkvRich2GasResTotal->
                                fill(aCkvRecD1E4-genckv);
    if(hCkvRich2GasQwPh)hCkvRich2GasQwPh->fill(aCkvRecD4E1- aCkvRecD3E1  );

    if(genckv > 0.0295 && genckv < 0.030) {

      if(hCkvRich2GasResConst) hCkvRich2GasResConst->
                                 fill(aCkvRecD3E1-genckv) ;
      if(hCkvRich2GasCkvPhi)  hCkvRich2GasCkvPhi->
                                fill(genckvphi,aCkvRecD3E1-genckv);
      //    if(hCkvRich2GasCkvResPhiProf)  hCkvRich2GasCkvResPhiprof->
      //                         fill(genckvphi,aCkvRecD3E1-genckv);
    }

    //  if(hCkvRich2GasCkvPhi)  hCkvRich2GasCkvPhi->
    //                          fill(aCkvRecD3E1-genckv,genckvphi);

  }


}

//=============================================================================
