// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/GiGaBase.cpp,v 1.10 2001-03-18 19:37:46 ibelyaev Exp $ 

//  STL
#include <string>
//  GaudiKernel
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISerialize.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/IChronoStatSvc.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/IMagneticFieldSvc.h" 
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IGiGaSvc.h"
#include "GaudiKernel/IGiGaSetUpSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
//  GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Stat.h"
//  from GiGa 
#include "GiGa/GiGaException.h"
#include "GiGa/GiGaBase.h" 

/////////////////////////////////////////////////////////////////////////////////////
GiGaBase::GiGaBase( const std::string& Name , ISvcLocator* svc )
  /// reference count
  : m_count      ( 0                     ) 
  /// name 
  , m_name       ( Name                  ) 
  ///
  , m_propMgr    ( 0                     )  
  ///
  , m_svcLoc     ( svc                   )   
  , m_init       ( false                 ) 
  , m_gigaName   ( "GiGaSvc"             ) 
  , m_setupName  ( "GiGaSvc"             ) 
  , m_msgName    ( "MessageSvc"          ) 
  , m_chronoName ( "ChronoStatSvc"       ) 
  , m_evtName    ( "EventDataSvc"        )
  , m_detName    ( "DetectorDataSvc"     )
  , m_incName    ( "IncidentSvc"         ) 
  , m_ppName     ( "ParticlePropertySvc" )
  , m_mfName     ( "MagneticFieldSvc"    ) 
  , m_output     ( MSG::NIL              ) 
  ///
  , m_gigaSvc    ( 0                     ) 
  , m_setupSvc   ( 0                     ) 
  , m_msgSvc     ( 0                     ) 
  , m_chronoSvc  ( 0                     ) 
  , m_evtSvc     ( 0                     )
  , m_detSvc     ( 0                     ) 
  , m_incSvc     ( 0                     ) 
  , m_ppSvc      ( 0                     ) 
  , m_mfSvc      ( 0                     ) 
  ///
{
  ///
  if( 0 == svcLoc() ) { throw GiGaException("GiGaBase Constructor:: ISvcLocator* points to NULL!") ; }
  ///
  m_propMgr = new PropertyMgr();
  ///  
  declareProperty( "OutputLevel"               ,  m_output      );
  declareProperty( "GiGaService"               ,  m_gigaName    );
  declareProperty( "GiGaSetUpService"          ,  m_setupName   );
  declareProperty( "MessageService"            ,  m_msgName     );
  declareProperty( "ChronoStatService"         ,  m_chronoName  );
  declareProperty( "EventDataProvider"         ,  m_evtName     );
  declareProperty( "DetectorDataProvider"      ,  m_detName     );
  declareProperty( "IncidentService"           ,  m_incName     );
  declareProperty( "ParticlePropertyService"   ,  m_ppName      );
  declareProperty( "MagneticFieldService"      ,  m_mfName      );
  ///
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
GiGaBase::~GiGaBase() 
{ 
  if( m_init         ) { finalize() ; } 
  if( 0 != m_propMgr ) { delete m_propMgr ; m_propMgr = 0 ; } 
} 
///////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaBase::queryInterface(const InterfaceID& riid , void** ppI )
{
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if      ( IProperty::         interfaceID() == riid ) { *ppI = static_cast<IProperty*>  ( this ) ; } 
  else if ( ISerialize::        interfaceID() == riid ) { *ppI = static_cast<ISerialize*> ( this ) ; } 
  else if ( IInterface::        interfaceID() == riid ) { *ppI = static_cast<IInterface*> ( this ) ; } 
  else if ( IIncidentListener:: interfaceID() == riid ) { *ppI = static_cast<IInterface*> ( this ) ; } 
  else                                                  { return StatusCode::FAILURE               ; }
  addRef();
  return StatusCode::SUCCESS;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaBase::initialize() 
{
  ///
  if( 0 != msgSvc    () ) { msgSvc    ()->release() ; m_msgSvc    = 0 ; } 
  if( 0 != chronoSvc () ) { chronoSvc ()->release() ; m_chronoSvc = 0 ; } 
  if( 0 != gigaSvc   () ) { gigaSvc   ()->release() ; m_gigaSvc   = 0 ; } 
  if( 0 != setupSvc  () ) { setupSvc  ()->release() ; m_setupSvc  = 0 ; } 
  if( 0 != evtSvc    () ) { evtSvc    ()->release() ; m_evtSvc    = 0 ; } 
  if( 0 != detSvc    () ) { detSvc    ()->release() ; m_detSvc    = 0 ; } 
  if( 0 != incSvc    () ) { incSvc    ()->release() ; m_incSvc    = 0 ; } 
  if( 0 != ppSvc     () ) { ppSvc     ()->release() ; m_ppSvc     = 0 ; } 
  if( 0 != mfSvc     () ) { mfSvc     ()->release() ; m_mfSvc     = 0 ; } 
  ///
  if( 0 == svcLoc()     ) { throw GiGaException("GiGaBase::initialize() ISvcLocator* points to NULL!");}
  ///
  {
    StatusCode st = setProperties(); 
    if( st.isFailure() ) { return Error("GiGaBase::initialize could not set own properties") ; }
  }
  ///
  if( !m_msgName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_msgName , m_msgSvc );  
      if( sc.isFailure() ) { return Error("Could not locate IMessageSvc="+ m_msgName , sc ) ; }
      if( 0 == msgSvc () ) { return Error("Could not locate IMessageSvc="+ m_msgName      ) ; }    
      msgSvc()->addRef() ; 
      m_output = 
        m_output < (int) MSG::NIL   ? (int) MSG::NIL   : 
        m_output > (int) MSG::FATAL ? (int) MSG::FATAL : m_output ; 
      msgSvc()->setOutputLevel( name(), m_output );
    }
  else { Warning("Message Service is not requested to be located"); }
  ///
  if( !m_chronoName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_chronoName , m_chronoSvc );  
      if( sc.isFailure()    ) { return Error("Could not locate IChronoStatSvc="+ m_chronoName , sc ) ; }
      if( 0 == chronoSvc () ) { return Error("Could not locate IChronoStatSvc="+ m_chronoName      ) ; }    
      chronoSvc()->addRef() ; 
    }
  else { Warning("Chrono & Stat Service is not requested to be located"); }
  ///
  if( !m_gigaName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_gigaName ,  m_gigaSvc  ); 
      if( sc.isFailure () ) { return Error("Could not locate IGiGaSvc="+ m_gigaName , sc ) ; }
      if( 0 == gigaSvc () ) { return Error("Could not locate IGiGaSvc="+ m_gigaName      ) ; }
      gigaSvc()->addRef() ; 
    }
  else { Warning("GiGa Service is not requestedto be located"); }
  ///
  if( !m_setupName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_setupName , m_setupSvc ); 
      if( sc.isFailure   () ) { return Error("Could not locate IGiGaSetUpSvc="+ m_setupName , sc ) ; }
      if( 0 == setupSvc  () ) { return Error("Could not locate IGiGaSetUpSvc="+ m_setupName      ) ; }
      setupSvc()->addRef() ; 
    }
  else { Warning("GiGaSetUp Service is not requested to be located"); }
  ///
  if( !m_evtName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_evtName , m_evtSvc ); 
      if( sc.isFailure   () ) { return Error("Could not locate IDataProviderSvc="+ m_evtName , sc ) ; }
      if( 0 == evtSvc    () ) { return Error("Could not locate IDataProviderSvc="+ m_evtName      ) ; }
      evtSvc()->addRef() ; 
    }
  else { Warning("Event Data  Service is not requested to be located"); }
  ///
  if( !m_detName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_detName , m_detSvc ); 
      if( sc.isFailure   () ) { return Error("Could not locate IDataProviderSvc="+ m_detName , sc ) ; }
      if( 0 == detSvc    () ) { return Error("Could not locate IDataProviderSvc="+ m_detName      ) ; }
      detSvc()->addRef() ; 
    }
  else { Warning("Detector Data  Service is not requested to be located"); }
  ///
  if( !m_incName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_incName , m_incSvc ); 
      if( sc.isFailure   () ) { return Error("Could not locate IIncidentSvc="+ m_incName , sc ) ; }
      if( 0 == incSvc    () ) { return Error("Could not locate IIncidentSvc="+ m_incName      ) ; }
      incSvc()->addRef() ; 
    }
  else { Warning("Incident Service is not requested to be located"); }
  ///
  if( !m_ppName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_ppName , m_ppSvc ); 
      if( sc.isFailure   () ) { return Error("Could not locate IParticlePropertySvc="+ m_ppName , sc ) ; }
      if( 0 == ppSvc     () ) { return Error("Could not locate IParticlePropertySvc="+ m_ppName      ) ; }
      ppSvc()->addRef() ; 
    }
  else { Warning("Particle Properties Service is not required to be located"); }
  ///
  if( !m_mfName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_mfName , m_mfSvc ); 
      if( sc.isFailure   () ) { return Error("Could not locate IMagneticFieldSvc="+ m_mfName , sc ) ; }
      if( 0 == mfSvc     () ) { return Error("Could not locate IMagneticFieldSvc="+ m_mfName      ) ; }
      mfSvc()->addRef() ; 
    }
  else { Warning("Magnetic Field Service is not required to be located"); }
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaBase::finalize()
{
  ///
  if ( m_init )
    {
      /// reverse order !!!
      if( 0 != mfSvc     () ) { mfSvc     ()->release() ; m_mfSvc     = 0 ; } 
      if( 0 != ppSvc     () ) { ppSvc     ()->release() ; m_ppSvc     = 0 ; } 
      if( 0 != incSvc    () ) { incSvc    ()->release() ; m_incSvc    = 0 ; } 
      if( 0 != detSvc    () ) { detSvc    ()->release() ; m_detSvc    = 0 ; } 
      if( 0 != evtSvc    () ) { evtSvc    ()->release() ; m_evtSvc    = 0 ; } 
      if( 0 != setupSvc  () ) { setupSvc  ()->release() ; m_setupSvc  = 0 ; } 
      if( 0 != gigaSvc   () ) { gigaSvc   ()->release() ; m_gigaSvc   = 0 ; } 
      if( 0 != chronoSvc () ) { chronoSvc ()->release() ; m_chronoSvc = 0 ; } 
      if( 0 != msgSvc    () ) { msgSvc    ()->release() ; m_msgSvc    = 0 ; } 
      ///
    }
  ///
  m_init = false ;
  ///
  return StatusCode::SUCCESS;
  ///
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaBase::setProperties() 
{
  ///
  if( 0 == svcLoc()    ) { throw GiGaException("GiGaBase::setProperties() ICvsLocatort* point to NULL!") ; } 
  ///
  {
    IService* pS = 0 ; 
    StatusCode sc = svcLoc()->getService( "JobOptionsSvc" , pS );
    if( sc.isFailure() ) { return Error("Could not locate JobOptionsSvc", sc          ) ; } 
    if( 0 == pS        ) { return Error("Could not locate JobOptionsSvc"              ) ; }
    IJobOptionsSvc* jos = dynamic_cast<IJobOptionsSvc*> (pS); 
    if( 0 == jos       ) { return Error("Could not cast IService* to JobOptionsSvc"   ) ; }
    jos->addRef  () ;
    jos->setMyProperties( name(), this );
    jos->release () ; 
  }
  ///
  return StatusCode::SUCCESS;
};
/// Print message and return status code //////////////////////////////////////////////////////
StatusCode GiGaBase::Error( const std::string& Message , 
			    const StatusCode & Status ) const 
{
  Stat stat( chronoSvc() , name()+":Error" ); 
  return Print( Message , Status , MSG::ERROR ); 
};  
/// Print message and return status code ///////////////////////////////////////////////////
StatusCode GiGaBase::Warning( const std::string& Message , 
			      const StatusCode & Status ) const 
{
  Stat stat( chronoSvc() , name()+":Warning" ); 
  return  Print( Message , Status , MSG::WARNING );
};  
/// Print message and return status code ///////////////////////////////////////////////////
StatusCode GiGaBase::Print( const std::string& Message , 
			    const StatusCode & Status  , 
			    const MSG::Level & level   ) const 
{
  MsgStream log( msgSvc() , name() ); 
  log << level << System::typeinfoName( typeid( *this ) ) << " " <<Message << endreq ; 
  return  Status;
};  
/////////////////////////////////////////////////////////////////////////////////////
StatusCode                    GiGaBase::setProperty  ( const Property    & p ) 
{ return m_propMgr->setProperty( p ) ; };
/////////////////////////////////////////////////////////////////////////////////////
StatusCode                    GiGaBase::setProperty  ( std::istream      & s )  
{ return m_propMgr->setProperty( s ) ; }
/////////////////////////////////////////////////////////////////////////////////////
StatusCode                    GiGaBase::setProperty  ( const std::string & n ,
                                                       const std::string & v )       
{ return m_propMgr->setProperty( n , v ) ; } ;
/////////////////////////////////////////////////////////////////////////////////////
StatusCode                    GiGaBase::getProperty  (       Property    * p ) const 
{ return m_propMgr->getProperty( p ) ; };
/////////////////////////////////////////////////////////////////////////////////////
const Property&               GiGaBase::getProperty  ( const std::string & N ) const 
{ return m_propMgr->getProperty( N ) ; };
/////////////////////////////////////////////////////////////////////////////////////
StatusCode                    GiGaBase::getProperty  ( const std::string & n ,
                                                             std::string & v ) const 
{ return m_propMgr->getProperty( n , v ) ; } ;
/////////////////////////////////////////////////////////////////////////////////////
const std::vector<Property*>& GiGaBase::getProperties()                        const 
{ return m_propMgr->getProperties()  ; };   
///////////////////////// serialize object for reading //////////////////////////////
StreamBuffer& GiGaBase::serialize( StreamBuffer& S )       
{ 
  ///
  if( 0 == m_propMgr ) { m_propMgr = new PropertyMgr() ;} 
  ///x
  S >> m_name  
    >> m_gigaName 
    >> m_setupName
    >> m_msgName   
    >> m_evtName   
    >> m_detName    
    >> m_incName    
    >> m_ppName    
    >> m_mfName    
    >> m_output ; 
  ///
  m_init = false; 
  ///
  return S;       
};  
//////////////////// serialize object for writing ///////////////////////
StreamBuffer& GiGaBase::serialize( StreamBuffer& S ) const 
{
  return S << m_name  
           << m_gigaName 
           << m_setupName
           << m_msgName   
           << m_evtName   
           << m_detName    
           << m_incName    
           << m_ppName    
           << m_mfName    
           << m_output ;   
}; 
///////////////////////////////////////////////////////////////////////////
void  GiGaBase::handle( const Incident& incident ) 
{ Print("Incident='"+incident.type()+"'"+" \tsource='"+incident.source()+"'" ); };
///////////////////////////////////////////////////////////////////////////












