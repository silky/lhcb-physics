// $Id: GaussTrackActionGeneral.cpp,v 1.2 2004-03-22 16:51:05 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/02/20 19:35:27  ibelyaev
//  major update
// 
// ============================================================================
// Include files 
// ============================================================================
// CLHEP
// ============================================================================
#include "CLHEP/Units/SystemOfUnits.h"
// ============================================================================
// Geant4
// ============================================================================
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"
// ============================================================================
// GiGa
// ============================================================================
#include "GiGa/GiGaMACROs.h"
// ============================================================================
// GaussTools 
// ============================================================================
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
// ============================================================================
// local
// ============================================================================
#include "GaussTrackActionGeneral.h"
// ============================================================================

/** @file 
 *  Implementation file for class : GaussTrackActionGeneral
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
IMPLEMENT_GiGaFactory( GaussTrackActionGeneral );
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussTrackActionGeneral::GaussTrackActionGeneral
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionZ   ( type , name , parent ) 
  //
  , m_storeAll       ( false ) // do not pollute the tape 
  , m_storePrimaries ( true  ) // keep primaries 
  , m_storeDecays    ( true  ) // keep all G4 decays 
  , m_storePreDecays ( true  ) // keep preassigned decays 
  //
{
  declareProperty ( "StoreAll"       , m_storeAll       ) ;
  declareProperty ( "StorePrimaries" , m_storePrimaries ) ;
  declareProperty ( "StorePreDecays" , m_storePreDecays ) ;
  declareProperty ( "StoreDecays"    , m_storeDecays    ) ;
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionGeneral::~GaussTrackActionGeneral() {}
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionGeneral::PreUserTrackingAction  ( const G4Track* track ) 
{
  if ( !storeAll       () && !storePrimaries () && 
       !storePreDecays () && !storeDecays    ()  )        { return ; } // RETURN
  if ( 0 == trackMgr() ) 
  { Error ( "Pre..: G4TrackingManager* points to NULL!" ) ; return ; } // RETURN
  
  if ( 0 == track ) 
  { Error ( "Pre..: G4Track*           points to NULL!" ) ; return ; } // RETURN
  
  if      ( track -> GetVertexPosition().z() < zMin()   ) { return ; } // RETURN
  else if ( track -> GetVertexPosition().z() > zMax()   ) { return ; } // RETURN
  
  // get the trajectory 
  GaussTrajectory* tr = trajectory() ;
  
  // check the validity 
  if( 0 == tr ) 
  { Error ( "Pre...: GaussTrajectory*       points to NULL" ) ; return ; }
  
  // check the track information
  GaussTrackInformation*   info = trackInfo() ;
  if( 0 == info ) 
  { Error ( "Pre...: GaussTrackInformation* points to NULL" ) ; return ; }
  
  bool store = false ;
  
  if      ( trackMgr() ->GetStoreTrajectory()              ) { store = true ; }
  else if ( storeAll       ()                              ) { store = true ; }
  else if ( storePrimaries () && 0 == track->GetParentID() ) { store = true ; }
  else if ( storeDecays () && fDecay == track 
            -> GetCreatorProcess() -> GetProcessType()     ) { store = true ; }
  else if ( storePreDecays () && track -> GetDynamicParticle ()
            -> GetPreAssignedDecayProducts () ) 
  {
    store = true ;
    storeChildren() ;
  }
  
  // update the global flag 
  if ( store ) { trackMgr() -> SetStoreTrajectory( true ) ; }
  
  // update track info 
  if ( trackMgr() ->GetStoreTrajectory() ) { info->setToBeStored( true ) ; }
  
};
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionGeneral::PostUserTrackingAction  
( const G4Track* /* track */  ) {}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
