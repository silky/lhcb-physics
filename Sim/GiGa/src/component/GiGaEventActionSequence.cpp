// $Id: GiGaEventActionSequence.cpp,v 1.7 2002-05-07 12:21:34 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
/// STD & STL 
#include<functional> 
#include<algorithm>
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaUtil.h"
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaMACROs.h"
/// local 
#include "GiGaEventActionSequence.h"

// ============================================================================
/** @file
 *
 *  implementation of GiGaEventActionSequence
 *
 *  @author Vanya  Belyaev
 *  @date 24/07/2001 
 */
// ============================================================================

// ============================================================================
// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaEventActionSequence ) ;
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaEventActionBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaEventActionSequence::GiGaEventActionSequence
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaEventActionBase ( type , name  , parent ) 
  , m_members      ()  ///< default empty list
  , m_actions      ()
{ declareProperty( "Members" , m_members ); };
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaEventActionSequence::~GiGaEventActionSequence(){};
// ============================================================================

// ============================================================================
/** initialize the event action object
 *  @see GiGaEventActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionSequence::initialize() 
{
  /// initialize the base class 
  StatusCode sc = GiGaEventActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  if( m_members.empty() ) { Warning("The sequence is empty!"); }
  // instantiate members 
  std::string Type, Name;
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member )
    {
      sc = GiGaUtil::SplitTypeAndName( *member , Type , Name );
      if( sc.isFailure() )
        { return Error("Member Type/Name '"+(*member)+"' is unparsable",sc);}
      IGiGaEventAction* action = 0         ;
      sc = toolSvc()->retrieveTool( Type , Name , action , gigaSvc() );
      if( sc.isFailure() ) 
        { return Error("Could not create IGiGaEventAction '" 
                       + Type + "'/'" + Name + "'" , sc  ) ; }
      if( 0 == action    ) 
        { return Error("Could not create IGiGaEventAction '" 
                       + Type + "'/'" + Name + "'"       ) ; }
      action->addRef();
      m_actions.push_back( action );
    }       
  //
  return Print("Initialized successfully" , 
               StatusCode::SUCCESS        , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalize the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionSequence::finalize() 
{
  Print("Finalization", StatusCode::SUCCESS , MSG::VERBOSE );
  // release  all members 
  std::for_each  ( m_actions.begin () , 
                   m_actions.end   () ,
                   std::mem_fun(&IInterface::release) );
  //
  return GiGaEventActionBase::finalize();
};
// ============================================================================

// ============================================================================
/** perform begin-of-event action
 *  @param event pointer to Geant4 event 
 */
// ============================================================================
void GiGaEventActionSequence::BeginOfEventAction ( const G4Event* event )
{
  // stepping actions of all members  
  std::for_each ( m_actions.begin () , 
                  m_actions.end   () ,
                  std::bind2nd( std::mem_fun1(&IGiGaEventAction::
                                              BeginOfEventAction) , event ) );
};
// ============================================================================

// ============================================================================
/** perform end-of-event action
 *  @param event pointer to Geant4 event 
 */
// ============================================================================
void GiGaEventActionSequence::EndOfEventAction ( const G4Event* event )
{
  // stepping actions of all members  
  std::for_each ( m_actions.begin () , 
                  m_actions.end   () ,
                  std::bind2nd( std::mem_fun1(&IGiGaEventAction::
                                              EndOfEventAction) , event ) );
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
