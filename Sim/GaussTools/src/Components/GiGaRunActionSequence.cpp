// $Id: GiGaRunActionSequence.cpp,v 1.2 2003-01-23 09:36:56 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/12/12 15:19:32  witoldp
// major repackaging
//
// Revision 1.2  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
//
// Revision 1.1  2002/09/26 18:10:52  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.7  2002/05/07 12:21:35  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
/// STD & STL 
#include <algorithm>
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaUtil.h"
/// local 
#include "GiGaRunActionSequence.h"

// ============================================================================
/** @file
 *
 * Implementation file for class GiGaRunActionSequence
 * 
 * @author Vanya  Belyaev Ivan.Belyaev@itep.ru
 * @date 26/07/2001 
 */
// ============================================================================

// ============================================================================
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaRunActionSequence ) ;

// ============================================================================
/** standard constructor 
 *  @see GiGaPhysListBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaRunActionSequence::GiGaRunActionSequence
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaRunActionBase( type , name , parent )
  , m_members ( ) ///< empty default lst!
  , m_actions ( ) 
{ declareProperty( "Members" , m_members ); };
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaRunActionSequence::~GiGaRunActionSequence() 
{
  m_members.clear () ;
  m_actions.clear () ;
};
// ============================================================================

// ============================================================================
/** initialization method 
 *  @see GiGaRunActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionSequence::initialize  ()
{
  /// initialize the base class 
  StatusCode sc = GiGaRunActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  /// instantiate all members using the creator 
  std::string Type ; // Member Type 
  std::string Name ; // Member Name
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member ) 
    {
      IGiGaRunAction* action = tool( *member , action , this );
      if( 0 == action    ) { return Error("Could not create IGiGaRunAction '" 
                                          + *member + "'"       ) ; }
      m_actions.push_back( action );
    }       
  ///
  return Print("initialized successfully" , 
               StatusCode::SUCCESS , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionSequence::finalize()
{
  Print("Finalization", StatusCode::SUCCESS , MSG::VERBOSE );
  // release all members 
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IInterface*  action = *iaction ;
      if( 0 != action ) { action->release() ; }
      *iaction = 0 ;
    }
  m_actions.clear();
  // finalize teh base class 
  return GiGaRunActionBase::finalize();
};

// ============================================================================
/** perform begin-of-run action
 *  @param run  pointer to Geant4 Run object
 */
// ============================================================================
void GiGaRunActionSequence::BeginOfRunAction ( const G4Run* run )
{
  // run actions of all members  
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IGiGaRunAction* action = *iaction ;
      if( 0 != action ) { action->BeginOfRunAction( run ) ; }
    }
};
// ============================================================================

// ============================================================================
/** perform end-of-run action
 *  @param run  pointer to Geant4 Run object
 */
// ============================================================================
void GiGaRunActionSequence::EndOfRunAction ( const G4Run* run )
{
  // run actions of all members  
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IGiGaRunAction* action = *iaction ;
      if( 0 != action ) { action->BeginOfRunAction( run ) ; }
    }
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


