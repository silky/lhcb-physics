// $Id: HcalSensDet.cpp,v 1.1 2003-07-07 08:21:11 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
/// SRD & STD 
#include <algorithm>
#include <vector>
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaHashMap.h"
/// GaussTools 
#include "GaussTools/GaussTrackInformation.h"
/// Geant4 
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4EnergyLossTables.hh"
// GiGaCnv 
#include "GiGaCnv/GiGaVolumeUtils.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
/// local
#include "CaloHit.h"
#include "CaloSimHash.h"
#include "HcalSensDet.h"
///
#include "AIDA/IHistogram1D.h"

// ============================================================================
/** @file 
 * 
 *  Implementation of class HcalSensDet
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Patrick Robbe robbe@lal.in2p3.fr 
 *
 *  @date   23/01/2001 
 */
// ============================================================================

// ============================================================================
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( HcalSensDet );
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaSensDetBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
HcalSensDet::HcalSensDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : EHCalSensDet        ( type , name , parent ) 
  , G4VSensitiveDetector( name  )
{};
// ============================================================================


// ============================================================================
/** fill the hit with the concrete information about the energy and the time.
 *  The function is to be called from ProcessHits method.
 *
 *  @param hit        hit  to be filled with the correct information, 
 *                    the cellID information is accessible from the hit
 *  @param prePoint   PreStepPoint  for the given step 
 *  @param time       the global time of prestep point 
 *  @param deposit    raw energy deposition for the step 
 *  @param track      the actual G4Track pbject (decoded from G4Step)
 *  @param particle   the actual particle type  (decoded from G4Step/G4Track)  
 *  @param material   the actual material       (decoded from G4Step) 
 *  @param step       the step itself, the most important 
 *                    information from the step 
 *                    is already decoded into prePoint,globalTime,track,
 *                    particle definition,material etc for efficiency reasons. 
 *
 */
// ============================================================================
StatusCode    HcalSensDet::fillHitInfo 
( CaloSubHit*                    hit           ,
  const HepPoint3D&              prePoint      ,
  const double                   time          , 
  const double                   deposit       ,
  const G4Track*                 track         , 
  const G4ParticleDefinition*    particle      ,
  const G4Material*              material      ,
  const G4Step*                  step          ) const 
{
  
  if( 0 == hit || 0 == step ) { return StatusCode::FAILURE ; }
  
  // Birk's Law Correction
  const double energy = deposit * 
    birkCorrection ( particle                   , 
                     track->GetKineticEnergy () , 
                     material                   ) ;
  
  // get the cell 
  const CaloCellID& cellID = hit->cellID() ;
  
  // add current energy deposition to the sub-hit
  CaloSubHit::Time slot = 0   ;
  
  Fractions  fractions ;
  StatusCode sc = timing ( time , cellID , slot , fractions ) ;
  
  if ( sc.isFailure()    ) { return Error ( "Error from timing()" , sc ) ; }
  if ( fractions.empty() )
    { Warning("The empty vector of fractions()"); }

  for( Fractions::const_iterator ifr = fractions.begin() ; 
       fractions.end() != ifr ; ++ifr ) 
    {
      slot++ ;
      const double fr = *ifr ;
      if( fr > 1e-6 ) { hit->add( slot , energy * fr ) ; }
    }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
