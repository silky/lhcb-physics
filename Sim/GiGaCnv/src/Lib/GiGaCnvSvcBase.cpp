// $Id: GiGaCnvSvcBase.cpp,v 1.15 2003-12-10 17:25:46 ranjard Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.14  2003/07/07 16:45:30  ranjard
// v12r3 - fix for gcc3.2
//
// Revision 1.13  2003/01/23 09:20:37  ibelyaev
//  few fixes for Win2K platform
//
// Revision 1.12  2002/12/13 14:25:21  ibelyaev
//  few trivial bug fixes
//
// Revision 1.11  2002/12/07 14:36:25  ibelyaev
//  see $GIGACNVROOT/doc/release.notes
//
// Revision 1.10  2002/12/04 16:25:18  ibelyaev
//  remove extra calls for 'addRef'
//
// Revision 1.9  2002/08/23 08:19:41  witoldp
// Hits converters removed, bug fix
//
// Revision 1.8  2002/05/07 12:24:50  ibelyaev
//  see $GIGACNVROOT/doc/release.notes 7 May 2002
//
// ===========================================================================
#define GIGACNV_GIGACNVSVCBASE_CPP 1  
// ============================================================================
// STL & STL 
#include <string> 
#include <vector> 
#include <map> 
// Gaudi
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/IIncidentListener.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Stat.h"
/// GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGa/IGiGaSetUpSvc.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaException.h"
/// GiGaCnv
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/GiGaCnvSvcBase.h"

// ============================================================================
/** @file 
 *
 *   implementation of 
 *   base conversion service  for converting of Gaudi 
 *   structures into Geant4 structures and vice versa  
 *   
 *   @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *   @date    07/08/2000 
 */
// ============================================================================

namespace GiGaCnvSvcBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaCnvSvcBase> s_Counter ;
#endif   
};

// ============================================================================
/** standard constructor
 *  @param ServiceName     service name 
 *  @param ServiceLocator  pointer to Service Locator 
 *  @param StorageType     storage type identifier
 */
// ============================================================================
GiGaCnvSvcBase::GiGaCnvSvcBase( const std::string&   ServiceName       , 
                                ISvcLocator*         ServiceLocator    ,
                                const unsigned int   StorageType       )
  : ConversionSvc( ServiceName , ServiceLocator , StorageType )  
  //
  , m_dpName      ( "UndefinedName"       )
  , m_dpSvc       (     0                 )
  // 
  , m_evtName     ( "EventDataSvc"        )
  , m_evtSvc      (     0                 )
  //
  , m_detName     ( "DetectorDataSvc"     )
  , m_detSvc      (     0                 )
  //
  , m_gigaName    ( "GiGa"                ) 
  , m_gigaSvc     (     0                 ) 
  //
  , m_setupName   ( "GiGa"                ) 
  , m_setupSvc    (     0                 ) 
  //
  , m_chronoName  ( "ChronoStatSvc"       )  
  , m_chronoSvc   (     0                 ) 
  //
  , m_toolName    ( "ToolSvc"             )  
  , m_toolSvc     (     0                 ) 
  //
  , m_inName      ( "IncidentSvc"         )  
  , m_incSvc      (     0                 ) 
  //
  , m_errors      ()
  , m_warnings    ()
  , m_exceptions  ()
{ 
  declareProperty   ( "EventDataProviderService"        , m_evtName     );
  declareProperty   ( "DetectorDataProviderService"     , m_detName     );
  declareProperty   ( "GiGaService"                     , m_gigaName    ); 
  declareProperty   ( "GiGaSetUpService"                , m_setupName   ); 
  declareProperty   ( "ChronoStatService"               , m_chronoName  );
  declareProperty   ( "ToolService"                     , m_toolName    );
  declareProperty   ( "IncidentService"                 , m_inName      );
  
#ifdef GIGA_DEBUG
  GiGaCnvSvcBaseLocal::s_Counter.increment () ;
#endif
};

// ============================================================================
// virtual destructor 
// ============================================================================
GiGaCnvSvcBase::~GiGaCnvSvcBase()
{
#ifdef GIGA_DEBUG
  GiGaCnvSvcBaseLocal::s_Counter.decrement () ;
#endif  
};

// ============================================================================
/** service initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::initialize()
{
  ///
  StatusCode st = ConversionSvc::initialize() ; 
  if( st.isFailure()     )  
    { return Error("Could not initialize base class!", st); } 
  ///
  { /// print ALL properties 
    typedef std::vector<Property*> Properties;
    const Properties& properties = getProperties() ;
    MsgStream log( msgSvc() , name ()  );
    log << MSG::DEBUG 
        << " List of ALL properties of "
        << System::typeinfoName( typeid( *this ) ) << "/" 
        << name ()           << "   #properties = " 
        << properties.size() << endreq ;
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
    const int   buffer_size  = 256 ;
    char buffer[buffer_size]       ;
#endif
    for( Properties::const_reverse_iterator property 
           = properties.rbegin() ;
         properties.rend() != property ; ++property )  
      {
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
        std::fill( buffer , buffer + buffer_size , 0 );
        std::ostrstream ost ( buffer , buffer_size );
        (*property)->nameAndValueAsStream( ost );
        ost.freeze();
#else
        std::ostringstream ost;
        (*property)->nameAndValueAsStream( ost );
#endif
        log << MSG::DEBUG
            << "Property ['Name': Value] = " 
            << ost.str() << endreq ;
      }
  }
  ///
  if( !m_dpName.empty() ) 
    {
      StatusCode status = 
        serviceLocator()->service( m_dpName ,  m_dpSvc  , true );
      if( status.isFailure() ) 
        { return Error("Initialize::Could not locate IDataProvider=" + 
                       m_dpName, status );}      
      if( 0 == dpSvc()       ) 
        { return Error("Initialize::Could not locate IDataProvider=" + 
                       m_dpName         );}
      setDataProvider( dpSvc() ); 
      dpSvc() -> addRef () ;
      Print( " Located DataProvider="+m_dpName, MSG::VERBOSE ); 
    } 
  else { return Error(" IDataProvider is not requested to be located!") ;} 
  ///
  if( !m_gigaName.empty() ) 
    {
      StatusCode status = 
        serviceLocator()->service( m_gigaName , m_gigaSvc , true ) ;
      if( status.isFailure() ) 
        { return Error("Initialize::Could not locate IGiGaSvc=" + 
                       m_gigaName, status );}      
      if( 0 == gigaSvc()     ) 
        { return Error("Initialize::Could not locate IGiGaSvc=" + 
                       m_gigaName         );}
      Print( " Located GiGa Service="+m_gigaName, MSG::VERBOSE ); 
    } 
  else { return Error(" IGiGaSvc is not requested to be located!") ;} 
  ///
  if( !m_setupName.empty() ) 
    {
      StatusCode status = 
        serviceLocator()->service( m_setupName , m_setupSvc , true ) ;
      if( status.isFailure() ) 
        { return Error("Initialize::Could not locate IGiGaSetUpSvc=" + 
                       m_setupName, status );}      
      if( 0 == setupSvc()    ) 
        { return Error("Initialize::Could not locate IGiGaSetUpSvc=" + 
                       m_setupName         );}
      Print( " Located GiGa SetUp Service="+m_setupName, MSG::VERBOSE ); 
    } 
  else { return Error(" IGiGaSetUpSvc is not requested to be located!") ;} 
  ///
  if( !m_evtName.empty() ) 
    {
      StatusCode status = 
        serviceLocator()->service( m_evtName , m_evtSvc  , true  ) ; 
      if( status.isFailure() ) 
        { return Error("Initialize::Could not locate IDataProvider=" + 
                       m_evtName, status );}      
      if( 0 == evtSvc()      ) 
        { return Error("Initialize::Could not locate IDataProvider=" + 
                       m_evtName         );}
      Print( " Located Event Data  Service="+m_evtName, MSG::VERBOSE ); 
    } 
  else { Warning(" Event Data Service is not requested to be located!") ;} 
  ///
  if( !m_detName.empty() ) 
    {
      StatusCode status = 
        serviceLocator()->service( m_detName , m_detSvc  , true ) ; 
      if( status.isFailure() ) 
        { return Error("Initialize::Could not locate IDataProvider=" + 
                       m_detName, status );}      
      if( 0 == detSvc()      ) 
        { return Error("Initialize::Could not locate IDataProvider=" + 
                       m_detName         );}
      Print( " Located Detector Data  Service="+m_detName, MSG::VERBOSE ); 
    } 
  else { Warning(" Detector Data Service is not requested to be located!") ;} 
  ///
  if( !m_chronoName.empty() ) 
    {
      StatusCode status = 
        serviceLocator()->service( m_chronoName , m_chronoSvc  , true ) ; 
      if( status.isFailure() ) 
        { return Error("Initialize::Could not locate IChronoStatSvc=" + 
                       m_chronoName, status );}      
      if( 0 == chronoSvc()   ) 
        { return Error("Initialize::Could not locate IChronoStatSvc=" + 
                       m_chronoName         );}
      Print( " Located Chrono & Stat Service="+m_chronoName, MSG::VERBOSE ); 
    } 
  else { Warning(" Chrono & Stat Service is not requested to be located!") ;} 
  ///
  {
    StatusCode status = 
      serviceLocator()->service( m_toolName , m_toolSvc  , true ) ; 
    if( status.isFailure() ) 
      { return Error("Initialize::Could not locate IToolSvc=" + 
                     m_toolName, status );}      
    if( 0 == toolSvc()   ) 
      { return Error("Initialize::Could not locate IToolSvc=" + 
                     m_toolName         );}
    Print( " Located Tool  Service=" + m_toolName, MSG::VERBOSE ); 
  } 
  ///
  if( !m_inName.empty() ) 
    {
      StatusCode status = 
        serviceLocator()->service( m_inName , m_incSvc  , true ) ; 
      if( status.isFailure() ) 
        { return Error("Initialize::Could not locate IIncidentSvc=" + 
                       m_inName, status );}      
      if( 0 == incSvc ()      ) 
        { return Error("Initialize::Could not locate IIncidentSvc=" + 
                       m_inName         );}
      Print( " Located Incident Service  "+m_inName, MSG::VERBOSE ); 
      incSvc()->addListener( this , "BeginEvent" );
    } 
  else { return Error(" Incident Service is not requested to be located!") ;} 
  ///
  ///
  setAddressCreator( this );
  
  StatusCode sc = locateOwnCnvs();
  if( sc.isFailure() ) { return Error(" Cannot locate own converters ",sc);}
  
  return Print("GiGaCnvSvcBase initialized successfully" , 
               MSG::VERBOSE , StatusCode::SUCCESS ) ;
  ///
};

// ============================================================================
/** locate all own converters 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::locateOwnCnvs() 
{  
  /// here we need to locate all converter factories and converters 
  if( 0 == cnvManager() ) 
    { return Error("locateOwnCnvs():ICnvManager* is not available!") ; }
  for( ICnvManager::CnvIterator it = cnvManager()->cnvBegin() ; 
       cnvManager()->cnvEnd() != it ; ++it )
    {
      if( 0 == *it || ( repSvcType() != (*it)->repSvcType()  ) ) { continue ; }
      StatusCode st = addConverter( (*it)->objType() ); 
      if( st.isFailure() )
        { return Error("locateOwnCnvs():Could not add converter=" + 
                       (*it)->typeName() , st ) ; }
      Print(" The converter for '" + (*it)->typeName() + 
            "' is found and added " , MSG::DEBUG , StatusCode::SUCCESS ); 
    } 
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** service initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::finalize()
{
  ///
  Print("GiGaCnvSvcBase Finalization" , MSG::VERBOSE , StatusCode::SUCCESS ) ;
  
  if ( 0 != dpSvc     () ) { dpSvc     () -> release() ; m_dpSvc     = 0 ; } 
  if ( 0 != evtSvc    () ) { evtSvc    () -> release() ; m_evtSvc    = 0 ; } 
  if ( 0 != detSvc    () ) { detSvc    () -> release() ; m_detSvc    = 0 ; } 
  if ( 0 != gigaSvc   () ) { gigaSvc   () -> release() ; m_gigaSvc   = 0 ; } 
  if ( 0 != setupSvc  () ) { setupSvc  () -> release() ; m_setupSvc  = 0 ; } 
  if ( 0 != chronoSvc () ) { chronoSvc () -> release() ; m_chronoSvc = 0 ; } 
  ///
  m_leaves.clear();
  ///  
  /// error printout 
  if( 0 != m_errors     .size() || 
      0 != m_warnings   .size() || 
      0 != m_exceptions .size()   ) 
    {      
      MsgStream log( msgSvc() , name() );
      // format printout 
      log << MSG::ALWAYS 
          << " Exceptions/Errors/Warnings statistics:  " 
          << m_exceptions .size () << "/"
          << m_errors     .size () << "/"
          << m_warnings   .size () << endreq ; 
      // print exceptions counter 
      for( Counter::const_iterator excp = m_exceptions.begin() ;
           excp != m_exceptions.end() ; ++excp )
        {
          log << MSG::ALWAYS 
              << " #EXCEPTIONS= " << excp->second  
              << " Message='"     << excp->first    << "'" << endreq ; 
        }  
      // print errors counter 
      for( Counter::const_iterator error = m_errors.begin() ;
           error != m_errors.end() ; ++error )
        {
          log << MSG::ALWAYS 
              << " #ERRORS    = " << error->second  
              << " Message='"     << error->first    << "'" << endreq ; 
        }  
      // print warnings
      for( Counter::const_iterator warning = m_warnings.begin() ;
           warning != m_warnings.end() ; ++warning )
        {
          log << MSG::ALWAYS 
              << " #WARNINGS  = " << warning->second  
              << " Message='"     << warning->first  << "'" << endreq ; 
        }  
    }
  m_errors      .clear();
  m_warnings    .clear();
  m_exceptions  .clear();
  ///
  return ConversionSvc::finalize() ;
};

// ============================================================================
/** Retrieve the interface
 *  @param IID   unique interface identifier 
 *  @param ppI   placeholder for returned interface 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::queryInterface( const IID& iid , void** ppI )
{ 
  ///
  if( 0 == ppI                ) { return StatusCode::FAILURE ;}
  ///
  *ppI = 0 ;
  ///
  if     ( IGiGaCnvSvc       ::interfaceID()  == iid ) 
    { *ppI = static_cast<IGiGaCnvSvc*>        ( this )     ; } 
  else if( IIncidentListener ::interfaceID()  == iid ) 
    { *ppI = static_cast<IIncidentListener*>  ( this )     ; } 
  else if( IConversionSvc    ::interfaceID()  == iid ) 
    { *ppI = static_cast<IConversionSvc*>     ( this )     ; } 
  else if( IService          ::interfaceID()  == iid ) 
    { *ppI = static_cast<IService*>           ( this )     ; } 
  else if( IInterface        ::interfaceID()  == iid ) 
    { *ppI = static_cast<IInterface*>         ( this )     ; } 
  else                                            
    { return ConversionSvc::queryInterface( iid , ppI ) ; } 
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;  
};

// ============================================================================
/** declare the object 
 *  @param Leaf object description 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::declareObject( const GiGaLeaf& leaf )
{
  m_leaves.push_back( leaf );
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** handle the incident
 *  @param reference to incident 
 */
// ============================================================================
void       GiGaCnvSvcBase::handle         ( const Incident& inc )
{};

// ============================================================================
/** print error message and return status code
 *  @param msg error message 
 *  @param sc  status code
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::Error
( const std::string& Message , 
  const StatusCode & Status  ) const 
{  
  Stat stat( chronoSvc() ,  name() + ":Error" );
  // increase counter of errrors 
  m_errors [ Message ] += 1 ;
  return  Print( Message , MSG::ERROR  , Status  ) ; 
};  

// ============================================================================
/** print warning message and return status code
 *  @param msg warning message 
 *  @param sc  status code
 *  @return statsu code 
   */
// ============================================================================
StatusCode GiGaCnvSvcBase::Warning
( const std::string& Message , 
  const StatusCode & Status  ) const 
{
  Stat stat( chronoSvc() ,  name() + ":Warning" );
  // increase counter of errrors 
  m_warnings [ Message ] += 1 ;
  return  Print( Message , MSG::WARNING , Status ) ; 
};

// ============================================================================
/** print the  message and return status code
 *  @param msg error message 
 *  @param lvl print level 
 *  @param sc  status code
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::Print
( const std::string& Message , 
  const MSG::Level & level   , 
  const StatusCode & Status  ) const 
{ 
  MsgStream log( msgSvc() , name() ); 
  log << level << Message << endreq   ; 
  return  Status; 
};

// ============================================================================
/** re-throw the exception 
 *  @param msg  message 
 *  @param exc  previous exception 
 *  @param lvl  print level 
 *  @param sc   status code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::Exception
( const std::string    & Message , 
  const GaudiException & Excp    ,
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Stat stat( chronoSvc() , Excp.tag() );
  Print( "GaudiExceptions: catch and re-throw " + Message ,  level , Status );
  // increase counter of errrors 
  m_exceptions [ Message ] += 1 ;
  throw GiGaException( name() + "::" + Message , Excp , Status );
  return  Status;
};

// ============================================================================
/** re-throw the exception 
 *  @param msg  message 
 *  @param exc  previous exception 
 *  @param lvl  print level 
 *  @param sc   status code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::Exception
( const std::string    & Message , 
  const std::exception & Excp    ,
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Stat stat( chronoSvc() , "std::exception" );
  Print( "std::exception: catch and re-thrwo " + Message ,  level , Status );
  // increase counter of errrors 
  m_exceptions [ Message ] += 1 ;
  throw GiGaException( name() + "::" + Message + " (" + 
                       Excp.what() + ")", Status );
  return  Status;
};

// ============================================================================
/** throw the exception 
 *  @param msg  message 
 *  @param lvl  print level 
 *  @param sc   status code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::Exception
( const std::string    & Message , 
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Stat stat( chronoSvc() , "*UNKNOWN Exception*" );
  Print( "GiGaException throw " + Message ,  level , Status );
  // increase counter of errrors 
  m_exceptions [ Message ] += 1 ;
  throw GiGaException( name() + "::" + Message , Status );
  return  Status;
};

// ============================================================================
/** register all declared leaves 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::registerGiGaLeaves()
{
  ///
  SmartIF<IDataManagerSvc> dataMgr( dataProvider() );
  if( !dataMgr )
    { return Error("Could not obtain IDataManagerSvc interface!");}
  ///
  StatusCode sc = StatusCode::SUCCESS ;
  for( Leaves::const_iterator leaf = m_leaves.begin() ;
       m_leaves.end() != leaf ; ++leaf )
    { 
      /// (1) create IOpaqueAddress 
      IOpaqueAddress* Address = 0 ;

      sc = createAddress( repSvcType () , 
                          leaf->clid () , 
                          leaf->par  () , 
                          leaf->ipar () , 
                          Address       );
      if( 0 == Address || sc.isFailure() )
        { return Error( "Could not create the address for the leaf '" 
                        + leaf->path() + "'"); }
      /// (2) register the created address 
      sc = dataMgr->registerAddress ( leaf->path() , Address );
      if( sc.isFailure() ) 
        { return Error( "Could not register the address for the leaf '" 
                        + leaf->path() + "'"); }
      Print( " The leaf '" + leaf->path()     + 
             "' is registered successfully "  , 
             MSG::DEBUG , StatusCode::SUCCESS ) ;
    };
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** Create a Generic address using explicit arguments to 
 *                              identify a single object.
 *  @param      svc_type    Technology identifier encapsulated 
 *                          in this address.
 *  @param      clid        Class identifier of the DataObject 
 *                          represented by the opaque address
 *  @param      par         Array of strings needed to 
 *                          construct the opaque address.
 *  @param      ipar        Array of integers needed to 
 *                          construct the opaque address.
 *  @param      address     Reference to pointer to the address 
 *                          where the created Address should be stored.
 *  @return     Status code indicating success or failure.
 */
// ============================================================================
StatusCode GiGaCnvSvcBase::createAddress
( long                  svc_type ,
  const CLID&           clid     ,
  const std::string*    par      , 
  const unsigned long*  ipar     ,
  IOpaqueAddress*&      address  )    
{
  ///
  if      ( 0 != par && 0 != ipar ) 
    { address = new GenericAddress( svc_type , 
                                    clid     , 
                                    par[0]   , 
                                    par[1]   , 
                                    ipar[0]  , 
                                    ipar[1]  ); }
  else if ( 0 !=  par             )
    { address = new GenericAddress( svc_type , 
                                    clid     , 
                                    par[0]   , 
                                    par[1]   ) ; }
  else if ( 0 != ipar             )
    { address = new GenericAddress( svc_type , 
                                    clid     , 
                                    ""       , 
                                    ""       ,
                                    ipar[0]  , 
                                    ipar[1]  ) ; }
  else
    { address = new GenericAddress( svc_type , 
                                    clid     ); }
  ///                            
  return StatusCode::SUCCESS;
}

// ============================================================================
// End 
// ============================================================================







