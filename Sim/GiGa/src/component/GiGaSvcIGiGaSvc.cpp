// ================================`============================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ================================`============================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/23 13:12:29  ibelyaev
/// the package restructurisation(II)
///
/// Revision 1.4  2001/07/15 20:54:36  ibelyaev
/// package restructurisation
///
// ============================================================================
#define GIGA_GIGASVCIGIGASVC_CPP 1 
// ============================================================================
/// includes
/// STD & STL 
#include <string>
#include <list>
#include <vector> 
#include <algorithm>
/// GaudiKernel
#include    "GaudiKernel/ISvcLocator.h"
#include    "GaudiKernel/IMessageSvc.h"
#include    "GaudiKernel/IChronoStatSvc.h"
#include    "GaudiKernel/IObjManager.h"
#include    "GaudiKernel/SvcFactory.h"
#include    "GaudiKernel/MsgStream.h"
#include    "GaudiKernel/ParticleProperty.h"
/// GiGa 
#include    "GiGa/GiGaException.h"
#include    "GiGa/GiGaRunManager.h"
/// local 
#include    "GiGaSvc.h"

// ============================================================================
/**  Implementation of class GiGaSvc  
 *   all methods from abstract interface IGiGaSvc 
 *
 *   @author: Vanya Belyaev 
 *   @date xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** add  primary vertex into GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param  vertex pointer to primary vertex 
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc&   GiGaSvc::operator <<         ( G4PrimaryVertex * vertex   )
{
  ///
  StatusCode sc = prepareTheEvent( vertex ) ; 
  Assert( sc.isSuccess(), " operator<<(G4PrimaryVertex*) " , sc );   
  ///
  return *this; 
  ///
}; 

// ============================================================================
/** get the whole event  object from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param   event pointer to whole event  
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc& GiGaSvc::operator >> ( const G4Event*         & event        )    
{
  ///
  StatusCode sc = retrieveTheEvent( event ) ; 
  Assert( sc.isSuccess(), "operator>>(G4Event*)" , sc );   
  return *this;
}; 

// ============================================================================
/** get the all hit collections from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param   collections  pointer to all hit collections  
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc& GiGaSvc::operator >> ( G4HCofThisEvent*       & collections  )
{
  ///
  const G4Event* event = 0 ; 
  *this >> event           ; 
  collections = 
    ( 0 != event )  ? event->GetHCofThisEvent() : 0 ; 
  ///
  return *this;   
};

// ============================================================================
/** get the concrete hit collection from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param   collection  reference to collection pair   
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc& GiGaSvc::operator >> ( CollectionPair         & collection   )
{
  G4HCofThisEvent* collections = 0 ; 
  *this >> collections             ; 
  collection.second = 
    ( 0 != collections)  ? collections->GetHC( collection.first ) : 0 ; 
  ///
  return *this ;  
}; 

// ============================================================================
/** get all trajectories(trajectory container) from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param   trajectories  pointer to trajectory conatiner   
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc& GiGaSvc::operator >> ( G4TrajectoryContainer* & trajectories )
{
  ///
  const G4Event* event = 0 ; 
  *this >> event ;
  trajectories = 
    ( 0 != event ) ? event->GetTrajectoryContainer() : 0 ; 
  /// 
  return *this ; 
};

// ============================================================================
/** add  primary vertex into GiGa/G4  
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  @param  vertex pointer to primary vertex 
 *  @return status code  
 */
// ============================================================================
StatusCode  GiGaSvc::addPrimaryKinematics( G4PrimaryVertex  * vertex   ) 
{
  ///
  StatusCode sc(StatusCode::FAILURE);
  ///
  ___GIGA_TRY___
    { *this << vertex           ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"addVtx",msgSvc(),chronoSvc(),sc ) ; 
  ///
  return StatusCode::SUCCESS;  
};

// ============================================================================
/** get the whole event  object from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  @param  event pointer to whole event 
 *  @return status code  
 */
// ============================================================================
StatusCode GiGaSvc::retrieveEvent  ( const G4Event*          & event )
{ 
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> event            ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"getEvt",msgSvc(),chronoSvc(),sc ) ; 
  ///
  return StatusCode::SUCCESS ;  
};

// ============================================================================
/** get the all hit collections from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 * 
 *  @param   collections  pointer to all hit collections  
 *  @return  status code  
 */
// ============================================================================
StatusCode GiGaSvc::retrieveHitCollections  ( G4HCofThisEvent* & collections  )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> collections      ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"getHits",msgSvc(),chronoSvc(),sc); 
  ///
  return StatusCode::SUCCESS; 
};

// ============================================================================
/** get the concrete hit collection from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  @param   collection  reference to collection pair   
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaSvc::retrieveHitCollection  ( CollectionPair & collection   )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> collection       ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"getHits",msgSvc(),chronoSvc(),sc); 
  ///
  return StatusCode::SUCCESS; 
};

// ============================================================================
/** get all trajectories(trajectory container) from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported throw exception
 * 
 *  @param   trajectories  pointer to trajectory conatiner   
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
StatusCode GiGaSvc::retrieveTrajectories( G4TrajectoryContainer*& trajectories)
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> trajectories     ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"getTrjs",msgSvc(),chronoSvc(),sc); 
  ///
  return StatusCode::SUCCESS; 
};

// ============================================================================
















