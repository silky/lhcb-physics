// $Id: GiGaKine.cpp,v 1.1 2002-12-07 14:36:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================

/// from Gaudi 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IParticlePropertySvc.h"
/// from GiGa 
#include "GiGaCnv/GiGaCnvSvcBase.h" 
// local
#include "GiGaKine.h" 

// ============================================================================
/// service factory 
// ============================================================================
static const  SvcFactory<GiGaKine> s_Factory ; 
const        ISvcFactory&GiGaKineFactory = s_Factory ;

// ============================================================================
/** standard constructor
 *  @param name  name of the service 
 *  @param loc   pointer to service locator 
 */
// ============================================================================
GiGaKine::GiGaKine( const std::string&   ServiceName          , 
                                ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvcBase(                                  ServiceName          , 
                                                     ServiceLocator       , 
                                                     GiGaKine_StorageType )
  , m_ppSvcName ("ParticlePropertySvc")
  , m_ppSvc     ( 0 )                
  ///
  , m_table     (   )
{
  setNameOfDataProviderSvc("EventDataSvc");
  ///
  declareProperty("ParticlePropertyService" , m_ppSvcName );
  ///
};

// ============================================================================
/// virtual destructor
// ============================================================================
GiGaKine::~GiGaKine()
{ 
  /// clear the reference table 
  m_table.clear(); 
};

// ============================================================================
/** initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaKine::initialize() 
{ 
  StatusCode sc = GiGaCnvSvcBase::initialize(); 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class!", sc ); }
  ///
  sc = svcLoc()->service( m_ppSvcName , m_ppSvc , true );
  if( sc.isFailure() ) 
    { return Error("Could not locate ParticlePropertyService!", sc);}
  if( 0 == ppSvc  ()  )
    { return Error("IParticlePropertySvc* points to NULL!");}
  ///
  return StatusCode::SUCCESS;
};  

// ============================================================================
/** finalization 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaKine::finalize()   
{ 
  /// clear the reference table 
  m_table.clear();
  /// release particle property service
  if( 0 != ppSvc() ) { ppSvc()->release() ; m_ppSvc = 0 ; }
  ///
  return GiGaCnvSvcBase::finalize(); 
};  

// ============================================================================
/** query the interface
 *  @param ID unique interface identifier 
 *  @param II placeholder for interface 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaKine::queryInterface( const InterfaceID& ID , 
                                           void**             II ) 
{
  if( 0 == II ) { return StatusCode::FAILURE; }
  /// reset interface
  *II = 0;
  if     ( IGiGaKineCnvSvc ::interfaceID () == ID )
    { *II = static_cast<IGiGaKineCnvSvc*>  ( this ) ; }
  else if( IGiGaCnvSvc     ::interfaceID () == ID )
    { *II = static_cast<IGiGaCnvSvc*>      ( this ) ; }
  else if( IConversionSvc  ::interfaceID () == ID )
    { *II = static_cast<IConversionSvc*>   ( this ) ; }
  else if( IService        ::interfaceID () == ID )
    { *II = static_cast<IService*>         ( this ) ; }
  else if( IInterface      ::interfaceID () == ID )
    { *II = static_cast<IInterface*>       ( this ) ; }
  else 
    { return GiGaCnvSvcBase::queryInterface( ID , II ) ; }
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================




















