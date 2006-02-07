// $Id: Inclusive.cpp,v 1.7 2006-02-07 00:15:32 robbep Exp $
// Include files 

// local
#include "Inclusive.h"

// standard
#include <cfloat>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/GenCounters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Inclusive
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<Inclusive>          s_factory ;
const        IToolFactory& InclusiveFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Inclusive::Inclusive( const std::string& type, const std::string& name,
                      const IInterface* parent )
  : ExternalGenerator  ( type, name , parent ) ,
    m_lightestMass     ( DBL_MAX ) ,
    m_lightestPID      ( 0 ) ,
    m_nEventsBeforeCut ( 0 ) , m_nEventsAfterCut ( 0 ) ,
    m_nInvertedEvents  ( 0 ) ,
    m_ccCounter        ( 0 ) , m_bbCounter( 0 ) ,
    m_ccCounterAccepted( 0 ) , m_bbCounterAccepted( 0 ) { 
    declareProperty( "InclusivePIDList" , m_pidVector ) ;
    m_bHadC.assign( 0 )     ;    m_bHadCAccepted.assign( 0 ) ;
    m_antibHadC.assign( 0 ) ;    m_antibHadCAccepted.assign( 0 ) ;
    m_cHadC.assign( 0 )     ;    m_cHadCAccepted.assign( 0 ) ;
    m_anticHadC.assign( 0 ) ;    m_anticHadCAccepted.assign( 0 ) ;

    m_bExcitedC.assign( 0 ) ;    m_bExcitedCAccepted.assign( 0 ) ;
    m_cExcitedC.assign( 0 ) ;    m_cExcitedCAccepted.assign( 0 ) ;


    GenCounters::setupBHadronCountersNames( m_bHadCNames , m_antibHadCNames ) ;
    GenCounters::setupDHadronCountersNames( m_cHadCNames , m_anticHadCNames ) ;
    
    GenCounters::setupExcitedCountersNames( m_bExcitedCNames , "B" ) ;
    GenCounters::setupExcitedCountersNames( m_cExcitedCNames , "D" ) ;
  }

//=============================================================================
// Destructor
//=============================================================================
Inclusive::~Inclusive( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode Inclusive::initialize( ) {
  StatusCode sc = ExternalGenerator::initialize( ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize base class !" ) ;

  if ( m_pidVector.empty() ) 
    return Error( "InclusivePIDList property is not set" ) ;

  // Transform vector into set
  for ( std::vector<int>::iterator it = m_pidVector.begin() ; 
        it != m_pidVector.end() ; ++it ) m_pids.insert( *it ) ;
  
  IParticlePropertySvc * ppSvc =
    svc< IParticlePropertySvc >( "ParticlePropertySvc" ) ;
  
  info() << "Generating Inclusive events of " ;
  PIDs::const_iterator it ;
  for ( it = m_pids.begin() ; it != m_pids.end() ; ++it ) {
    ParticleProperty * prop = ppSvc -> findByStdHepID( *it ) ;
    info() << prop -> particle() << " " ;
    if ( prop -> mass() < m_lightestMass ) {
      m_lightestMass = prop -> mass() ;
      m_lightestPID = prop -> pdgID() ;
    }
  }
  info() << endmsg ;  
  release( ppSvc ) ;

  return sc ;
}

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool Inclusive::generate( const unsigned int nPileUp , 
                          LHCb::HepMCEvents * theEvents , 
                          LHCb::GenCollisions * theCollisions ) {
  StatusCode sc ;
  bool result = false ;

  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;

  GenCounters::BHadronCounter thebHadC , theantibHadC ;
  GenCounters::DHadronCounter thecHadC , theanticHadC ;
  GenCounters::ExcitedCounter thebExcitedC , thecExcitedC ;
  unsigned int theccCounter , thebbCounter ;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent, 
                        theGenCollision ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! result ) {
      // Decay particles heavier than the particles to look at
      decayHeavyParticles( theGenEvent , m_lightestMass , m_lightestPID ) ;
      
      // Check if one particle of the requested list is present in event
      ParticleVector theParticleList ;
      if ( checkPresence( m_pids , theGenEvent , theParticleList ) ) {
        // Update counters
        thebHadC.assign( 0 )     ;    theantibHadC.assign( 0 ) ;
        thecHadC.assign( 0 )     ;    theanticHadC.assign( 0 ) ;
        thebExcitedC.assign( 0 ) ;    thecExcitedC.assign( 0 ) ;

        thebbCounter = 0         ;    theccCounter = 0 ;
          
        GenCounters::updateHadronCounters( theGenEvent , thebHadC , 
                                           theantibHadC , thecHadC , 
                                           theanticHadC , thebbCounter , 
                                           theccCounter ) ;
        GenCounters::updateExcitedStatesCounters( theGenEvent , thebExcitedC , 
                                                  thecExcitedC ) ;

        // Accumulate counters
        GenCounters::AddTo( m_bHadC , thebHadC ) ;
        GenCounters::AddTo( m_antibHadC , theantibHadC ) ;
        GenCounters::AddTo( m_cHadC , thecHadC ) ;
        GenCounters::AddTo( m_anticHadC , theanticHadC ) ;

        m_bbCounter += thebbCounter ;  m_ccCounter += theccCounter ;

        GenCounters::AddTo( m_bExcitedC , thebExcitedC ) ;
        GenCounters::AddTo( m_cExcitedC , thecExcitedC ) ;
        
        ++m_nEventsBeforeCut ;
        bool passCut = true ;
        if ( 0 != m_cutTool ) 
          passCut = m_cutTool -> applyCut( theParticleList , theGenEvent , 
                                           theGenCollision ) ;
        
        if ( passCut && ( ! theParticleList.empty() ) ) {
          ++m_nEventsAfterCut ;
          result = true ;

          if ( 0 == nPositivePz( theParticleList ) ) {
            revertEvent( theGenEvent ) ;
            ++m_nInvertedEvents ;
          }

          GenCounters::AddTo( m_bHadCAccepted , thebHadC ) ;
          GenCounters::AddTo( m_antibHadCAccepted , theantibHadC ) ;
          GenCounters::AddTo( m_cHadCAccepted , thecHadC ) ;
          GenCounters::AddTo( m_anticHadCAccepted , theanticHadC ) ;

          GenCounters::AddTo( m_bExcitedCAccepted , thebExcitedC ) ;
          GenCounters::AddTo( m_cExcitedCAccepted , thecExcitedC ) ;          
        }
      }
    }
  }  
  
  return result ;
}

//=============================================================================
// Print the counters
//=============================================================================
void Inclusive::printCounters( ) const {
  using namespace GenCounters ;
  
  info() << "************   Inclusive counters   **************" << std::endl ;
  
  printEfficiency( info() , "generator level cut" , 
                   m_nEventsAfterCut - m_nInvertedEvents , 
                   m_nEventsBeforeCut ) ;
  printCounter( info() , "z-inverted events" , m_nInvertedEvents ) ;
  info() << std::endl ;

  printArray( info() , m_bHadC , m_bHadCNames , "generated" ) ;
  printArray( info() , m_antibHadC , m_antibHadCNames , "generated" ) ;
  printCounter( info() , "generated (bb)" , m_bbCounter ) ;
  info() << std::endl  ;
  
  printArray( info() , m_cHadC , m_cHadCNames , "generated" ) ;
  printArray( info() , m_anticHadC , m_anticHadCNames , "generated" ) ;
  printCounter( info() , "generated (cc)" , m_ccCounter ) ;
  info() << std::endl ;

  printArray( info() , m_bHadCAccepted , m_bHadCNames , "accepted" ) ;
  printArray( info() , m_antibHadCAccepted , m_antibHadCNames , "accepted" ) ;
  printCounter( info() , "accepted (bb)" , m_bbCounterAccepted ) ;
  info() << std::endl ;
  
  printArray( info() , m_cHadCAccepted , m_cHadCNames , "accepted" ) ;
  printArray( info() , m_anticHadCAccepted , m_anticHadCNames , "accepted" ) ;
  printCounter( info() , "accepted (cc)" , m_ccCounterAccepted ) ;
  info() << std::endl ;

  printArray( info() , m_bExcitedC , m_bExcitedCNames , "generated" ) ;
  printArray( info() , m_bExcitedCAccepted , m_bExcitedCNames , "accepted" ) ;
  info() << std::endl ;

  printArray( info() , m_cExcitedC , m_cExcitedCNames , "generated" ) ;
  printArray( info() , m_cExcitedCAccepted , m_cExcitedCNames , "accepted" ) ;
  info() << endmsg ;
}

