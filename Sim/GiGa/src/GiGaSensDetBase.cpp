// from STL
#include <vector>
// GaudiKernel 
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/StreamBuffer.h"
// DetDesc 
#include "DetDesc/IDetectorElement.h"
// GiGa
#include "GiGa/GiGaSensDetBase.h" 


////////////////////////////////////////////////////////////////////////////////////////////////
GiGaSensDetBase::GiGaSensDetBase( const std::string& nick , ISvcLocator* svc )
  : G4VSensitiveDetector( nick                 ) /// ATTENTION !!! this name ill be overwritten later!!!!
  ///
  , GiGaBase            ( nick     ,   svc     ) 
  /// initilization flag 
  , m_init              ( false                )
  /// Active Flag 
  , m_active            ( true                 ) 
  /// name of (Gaudi) Detector Element to which the SD is attached 
  , m_detName           ( "/dd/Structure/LHCb" )
  /// Detector Element 
  , m_det               ( 0                    ) 
{ 
  declareProperty( "DetectorElement"       , m_detName  );
  declareProperty( "Active"                , m_active   );
};
/// virtual destructor /////////////////////////////////////////////////////////////////////////
GiGaSensDetBase::~GiGaSensDetBase(){};
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSensDetBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  {
    SmartDataPtr<IDetectorElement> de( detSvc() , m_detName );
    if( !de        ) { return Error("Could not locate DetectorElement="+m_detName ); }
    m_det = de ;
    if( 0 == det() ) { return Error("Could not locate DetectorElement="+m_detName ); }
  }
  /// Correct the names!
  {
    ///
    std::string tmp( m_detName + "/" + name() ); 
    std::string::size_type pos = tmp.find("//") ; 
    while( std::string::npos != pos ) { tmp.erase( pos , 1 ) ; pos = tmp.find("//") ; }
    /// attention!!! direct usage of G4VSensitiveDetector members!!!! ATTENTION!!! ATTENTION!!!
    pos = tmp.find_last_of('/') ; 
    if( std::string::npos == pos )
      { 
        G4VSensitiveDetector::SensitiveDetectorName = tmp              ;  /// ATTENTION !!!
        G4VSensitiveDetector::thePathName           = "/"              ;  /// ATTENTION !!! 
      }
    else 
      {
        G4VSensitiveDetector::SensitiveDetectorName = tmp              ; /// ATTENTION !!!
        G4VSensitiveDetector::SensitiveDetectorName.remove(0,pos+1)    ; /// ATTENTION !!!
        G4VSensitiveDetector::thePathName           = tmp              ; /// ATTENTION !!! 
        G4VSensitiveDetector::thePathName.remove(pos+1,tmp.length()-1) ; /// ATTENTION !!! 
        if( '/' != G4VSensitiveDetector::thePathName[(unsigned int)(0)] ) 
          { G4VSensitiveDetector::thePathName.insert(0,'/'); }            /// ATTENTION !!! 
      } 
    ///
    G4VSensitiveDetector::fullPathName =                                  /// ATTENTION !!!
      G4VSensitiveDetector::thePathName +                                 /// ATTENTION !!!
      G4VSensitiveDetector::SensitiveDetectorName                       ; /// ATTENTION !!! 
    ///
    MsgStream log( msgSvc() , name() ) ;
    log << MSG::DEBUG 
        << System::typeinfoName( typeid( *this ) ) << "::initialize() Names for G4VSensitiveDetector are: "  
        << " Path="          << G4VSensitiveDetector::GetPathName()   
        << " Name="          << G4VSensitiveDetector::GetName() 
        << " FullPathName="  << G4VSensitiveDetector::GetFullPathName() << endreq;  
    ///
  }
  ///
  {
    Activate( m_active );                                                 /// ATTENTION !!!   
    MsgStream log( msgSvc() , name() ); 
    log << ( !isActive() ? MSG::FATAL : MSG::DEBUG )                      /// ATTENTION !!!
        << System::typeinfoName( typeid (*this) )  
        << "::initialize() Sensitive Detector is " 
        << ( !isActive() ? "*NOT* activated " : "activated" ) <<  endreq; /// ATTENTION !!! 
  }
  ///
  m_init = true;
  ///
  return StatusCode::SUCCESS;
}; 
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSensDetBase::finalize() { return GiGaBase::finalize();  };
///////////////////////////////////////////////////////////////////////////
StatusCode GiGaSensDetBase::queryInterface( const InterfaceID& iid , void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if      ( IID_IGiGaSensDet == iid ) { *ppI = static_cast<IGiGaSensDet*>        ( this ) ; } 
  else                           { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};
/// serialize object for reading ///////////////////////////////////////////////////////////////
StreamBuffer& GiGaSensDetBase::serialize( StreamBuffer& S )       
{
  GiGaBase::serialize( S ) ; 
  m_init = false ; 
  m_det  = 0     ;
  int    tmp     ; 
  S >> m_detName >> tmp ; 
  ///
  m_active = tmp ? true : false ;   
  ///
  return S;       
};  
/// serialize object for writing /////////////////////////////////////////////////////////////////
StreamBuffer& GiGaSensDetBase::serialize( StreamBuffer& S ) const 
{
  GiGaBase::serialize( S ) ; 
  return S << m_detName << (int) m_active ;
}; 
//////////////////////////////////////////////////////////////////////////////////////////////////













