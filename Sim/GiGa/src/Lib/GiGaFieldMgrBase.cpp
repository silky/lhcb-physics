// $Id: GiGaFieldMgrBase.cpp,v 1.2 2003-06-05 08:20:54 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/06 18:49:47  ibelyaev
//  see $GIGAROOT/doc/release.notes
// 
// ============================================================================
// Include files
// Geant4//
#include "G4MagneticField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4TransportationManager.hh"
#include "G4ChordFinder.hh"
// 
#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4CashKarpRKF45.hh"
#include "G4RKG3_Stepper.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4HelixHeum.hh"
// GiGa
#include "GiGa/GiGaFieldMgrBase.h"
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file
 * 
 *  Implementation file for class : GiGaFieldMgrBase
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
// ============================================================================

// ============================================================================
namespace GiGaFieldMgrBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaFieldMgrBase> s_Counter ;
#endif   
};
// ============================================================================


// ============================================================================
/** Standard constructor
 *  @see GiGaBase
 *  @see  AlgTool 
 *  @param type   type of the object (?) 
 *  @param name   instance name 
 *  @param parent pointer to the parent 
 */
// ============================================================================
GiGaFieldMgrBase::GiGaFieldMgrBase 
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent )
  : GiGaBase        ( type , name , parent )
  , m_global        ( false                )
  , m_manager       ( 0                    )
  , m_minStep       ( 0.01*mm              )
  , m_stepperType   ( "UNDEFINED"          )
  , m_stepper       ( 0                    )
{
  declareInterface<IGiGaFieldMgr> ( this ) ;
  // 
  declareProperty ( "MinStep" , m_minStep      ) ;
  declareProperty ( "Stepper" , m_stepperType  ) ;
  declareProperty ( "Global"  , m_global       ) ;
  //
#ifdef GIGA_DEBUG
  //
  GiGaFieldMgrBaseLocal::s_Counter.increment () ;
  //
#endif
  //
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaFieldMgrBase::~GiGaFieldMgrBase()
{
  //
#ifdef GIGA_DEBUG
  //
  GiGaFieldMgrBaseLocal::s_Counter.decrement () ;
  //
#endif
  //
};
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   GiGaFieldMgrBase::initialize     ()
{ return GiGaBase::initialize () ; }
// ============================================================================

// ============================================================================
/** finalize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   GiGaFieldMgrBase::finalize       () 
{ return GiGaBase::finalize  () ; }
// ============================================================================

// ============================================================================
/** accessor to the stepper 
 *  @see G4MagIntegratorStepper
 *  @see IGiGaFieldMgr 
 *  @return pointer to the stepper  
 */
// ============================================================================
G4MagIntegratorStepper* GiGaFieldMgrBase::stepper  () const 
{
  if( 0 != m_stepper ) { return m_stepper ; }
  StatusCode sc = createStepper() ;
  if( sc.isFailure() ) { Error("stepper(): Stepper is not created!" , sc ) ; }
  return m_stepper ;
};
// ============================================================================

// ============================================================================
/** create the stepper 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaFieldMgrBase::createStepper () const 
{
  // clear existoing stepper 
  m_stepper = 0 ;
  
  // get magnetic field 
  G4MagneticField* mag = field() ;
  
  if( 0 == mag ) { Error("createStepper(): invalid magnetic field!" ) ; }
  
  G4Mag_UsualEqRhs* equation =  new G4Mag_UsualEqRhs ( mag );  
  
  typedef std::vector<std::string> Types ;
  Types types ;
  types.push_back( "ExplicitEuler"       ) ;
  types.push_back( "ImplicitEuler"       ) ;
  types.push_back( "SimpleRunge"         ) ;
  types.push_back( "SimpleHeum"          ) ;
  
  types.push_back( "ClassicalRK4"        ) ;
  types.push_back( "CashKarpRKF45"       ) ;
  types.push_back( "RKG3_Stepper"        ) ;
  
  types.push_back( "HelixExplicitEuler"  ) ;
  types.push_back( "HelixImplicitEuler"  ) ;
  types.push_back( "HelixSimpleRunge"    ) ;
  types.push_back( "HelixHeum"           ) ;
  
  MsgStream log ( msgSvc() , name() ) ;
  log << MSG::INFO 
      << " \t The available steppers are: " << endreq ;
  for( Types::const_iterator step = types.begin() ;
       types.end() != step ; ++step ) 
    { log << MSG::INFO << " \t\t (G4)" << *step << endreq ; }
  
  if      ( types  [0] == m_stepperType ) 
    { m_stepper = new G4ExplicitEuler      ( equation ) ; }
  else if ( types  [1] == m_stepperType ) 
    { m_stepper = new G4ImplicitEuler      ( equation ) ; }
  else if ( types  [2] == m_stepperType ) 
    { m_stepper = new G4SimpleRunge        ( equation ) ; }
  else if ( types  [3] == m_stepperType ) 
    { m_stepper = new G4SimpleHeum         ( equation ) ; }
  else if ( types  [4] == m_stepperType )
    { m_stepper = new G4ClassicalRK4       ( equation ) ; }
  else if ( types  [5] == m_stepperType ) 
    { m_stepper = new G4CashKarpRKF45      ( equation ) ; }
  else if ( types  [6] == m_stepperType ) 
    { m_stepper = new G4RKG3_Stepper       ( equation ) ; }
  else if ( types  [7] == m_stepperType ) 
    { m_stepper = new G4HelixExplicitEuler ( equation ) ; }
  else if ( types  [8] == m_stepperType ) 
    { m_stepper = new G4HelixImplicitEuler ( equation ) ; }
  else if ( types  [9] == m_stepperType ) 
    { m_stepper = new G4HelixSimpleRunge   ( equation ) ; }
  else if ( types [10] == m_stepperType ) 
    { m_stepper = new G4HelixHeum          ( equation ) ; }
  else 
    { return Error("createStepper(): Invalid stepper type '" + 
                   m_stepperType + "'" ) ; }
  
  log << MSG::INFO 
      << " Stepper of type '" + 
    GiGaUtil::ObjTypeName( m_stepper ) + "' is created " << endreq ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/** accessor to field  manager 
 *  @see G4FieldManager
 *  @return pointer to the field manager  
 */
// ============================================================================
G4FieldManager* GiGaFieldMgrBase::fieldMgr () const 
{
  if( 0 != m_manager ) { return m_manager ; }
  StatusCode sc = createFieldMgr() ;
  if( sc.isFailure() ) 
    { Error ( " fieldMgr(): error from createFieldManager () " , sc ) ; }
  return m_manager ;
};
// ============================================================================

// ============================================================================
/** create and configure Field Manager 
 *  @return status code
 */
// ============================================================================
StatusCode GiGaFieldMgrBase::createFieldMgr () const 
{
  // clear existing pointer 
  m_manager = 0 ;
  
  if( global () ) 
    {
      G4TransportationManager* mgr = 
        G4TransportationManager::GetTransportationManager () ;
      if( 0 == mgr ) { return Error("Invalid Transportation manager" ) ; }
      m_manager = mgr -> GetFieldManager() ;
      Warning ( "Global G4FieldManager will be (re)configured!" ) ;
    }
  else 
    { m_manager = new G4FieldManager() ; }
  
  if( 0 == m_manager ) 
    { return Error ( "createFieldMgr(): invalid manager!" ) ; }
  
  G4MagneticField* mag = field() ;
  if( 0 == mag )
    { Error ( "createFieldMgr(): MULL G4MagneticField "   ) ; }
  
  m_manager -> SetDetectorField( mag ) ;
  
  G4MagIntegratorStepper* step = stepper () ;
  if( 0 == step ) 
    { Error ( "createFieldMgr(): invalid stepper "        ) ; }
  
  G4ChordFinder* chordFinder = new G4ChordFinder( mag , m_minStep , step ) ;
  m_manager -> SetChordFinder( chordFinder );
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** flag for indication global/local characted of 
 *  the tool field manager  
 */
// ============================================================================
bool GiGaFieldMgrBase::global   () const { return m_global ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
