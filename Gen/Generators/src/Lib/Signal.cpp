// $Id: Signal.cpp,v 1.20 2007-07-17 14:23:13 robbep Exp $
// Include files 

// local
#include "Generators/Signal.h"

// from Gaudi
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IRndmGenSvc.h"

// from Generators
#include "Generators/IDecayTool.h"
#include "Generators/GenCounters.h"
#include "Generators/HepMCUtils.h"

// Function to test if a HepMC::GenParticle is Particle (or antiParticle) 
struct isParticle : std::unary_function< const HepMC::GenParticle * , bool > {
  bool operator() ( const HepMC::GenParticle * part ) const {
    return ( part -> pdg_id() > 0 ) ; 
  }
};

// Functions to test if a HepMC::GenParticle goes forward
struct isForwardParticle : 
  std::unary_function< const HepMC::GenParticle * , bool > {
  bool operator() ( const HepMC::GenParticle * part ) const {
    return ( ( part -> pdg_id() > 0 ) && ( part -> momentum().pz() > 0 ) ) ; 
  }
};

struct isForwardAntiParticle : 
  std::unary_function< const HepMC::GenParticle * , bool > {
  bool operator() ( const HepMC::GenParticle * part ) const {
    return ( ( part -> pdg_id() < 0 ) && ( part -> momentum().pz() > 0 ) ) ; 
  }
};

//-----------------------------------------------------------------------------
// Implementation file for class : Signal
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Signal::Signal( const std::string& type,
                const std::string& name,
                const IInterface* parent )
  : ExternalGenerator( type, name , parent ) ,
    m_nEventsBeforeCut   ( 0 ) , m_nEventsAfterCut        ( 0 ) ,
    m_nParticlesBeforeCut( 0 ) , m_nAntiParticlesBeforeCut( 0 ) ,
    m_nParticlesAfterCut ( 0 ) , m_nAntiParticlesAfterCut ( 0 ) ,
    m_nInvertedEvents ( 0 ) ,
    m_signalPID       ( 0 ) ,
    m_bbCounter       ( 0 ) ,
    m_ccCounter       ( 0 ) ,
    m_nSig            ( 0 ) ,
    m_nSigBar         ( 0 ) ,
    m_sigName        ( "" ) ,
    m_sigBarName     ( "" ) ,
    m_cpMixture       ( true ) { 
    declareProperty( "SignalPIDList" , m_pidVector ) ;
    declareProperty( "Clean" , m_cleanEvents = false ) ;    
    
    m_bHadC.assign( 0 ) ;  m_antibHadC.assign( 0 ) ;
    m_cHadC.assign( 0 ) ;  m_anticHadC.assign( 0 ) ;
    
    m_bExcitedC.assign( 0 ) ;
    m_cExcitedC.assign( 0 ) ;
    
    GenCounters::setupBHadronCountersNames( m_bHadCNames , m_antibHadCNames ) ;
    GenCounters::setupDHadronCountersNames( m_cHadCNames , m_anticHadCNames ) ;

    GenCounters::setupExcitedCountersNames( m_bExcitedCNames , "B" ) ;
    GenCounters::setupExcitedCountersNames( m_cExcitedCNames , "D" ) ;    
  }

//=============================================================================
// Destructor
//=============================================================================
Signal::~Signal( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode Signal::initialize( ) {
  StatusCode sc = ExternalGenerator::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  if ( m_pidVector.empty() ) 
    return Error( "SignalPIDList property is not set" ) ;

  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;

  sc = m_flatGenerator.initialize( randSvc , Rndm::Flat( 0., 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize flat random number generator" ) ;
  
  release( randSvc ) ;

  // Transform vector into set
  for ( std::vector<int>::iterator it = m_pidVector.begin() ; 
        it != m_pidVector.end() ; ++it ) m_pids.insert( *it ) ;
  
  IParticlePropertySvc * ppSvc =
    svc< IParticlePropertySvc >( "ParticlePropertySvc" ) ;
  
  info() << "Generating Signal events of " ;
  PIDs::const_iterator it ;
  for ( it = m_pids.begin() ; it != m_pids.end() ; ++it ) {
    ParticleProperty * prop = ppSvc -> findByStdHepID( *it ) ;
    info() << prop -> particle() << " " ;

    LHCb::ParticleID pid( prop -> pdgID() ) ;
    if ( pid.hasCharm() ) m_signalQuark = LHCb::ParticleID::charm ;
    else if ( pid.hasBottom() ) m_signalQuark = LHCb::ParticleID::bottom ;
    else return Error( "This case is not implemented yet" ) ;
    
    m_signalPID  = abs( prop -> pdgID() ) ;
    if ( prop -> pdgID() > 0 ) m_sigName = prop -> particle() ;
    else m_sigBarName = prop -> particle() ;
  }

  m_cpMixture = false ;

  if ( 2 == m_pids.size() ) {
    if ( *m_pids.begin() == - *(--m_pids.end()) ) m_cpMixture = true ;
    else return Error( "Bad configuration in PID list" ) ;
  } else if ( m_pids.size() > 2 ) return Error( "Too many PIDs in list" ) ;

  m_decayTool -> setSignal( *m_pids.begin() ) ;

  m_signalBr = m_decayTool -> getSignalBr( ) ;

  info() << endmsg ;  
  release( ppSvc ) ;

  if ( 0. == m_signalBr ) 
    warning() 
      << "The signal decay mode is not defined in the main DECAY.DEC table"
      << std::endl << "Please add it there !" << endmsg ;
  else 
    info() << "The signal decay mode has visible branching fractions of :"
           << m_signalBr << endmsg ;

  return sc ;
}

//=============================================================================
// Print the counters
//=============================================================================
void Signal::printCounters( ) const {
  using namespace GenCounters ;

  info() << "*************   Signal counters   ****************" << std::endl ;

  printEfficiency( info() , "generator level cut" , m_nEventsAfterCut , 
                   m_nEventsBeforeCut ) ;
  printCounter( info() , "z-inverted events" , m_nInvertedEvents ) ;
  info() << std::endl ;

  printEfficiency( info() , "generator particle level cut" , 
                   m_nParticlesAfterCut ,  m_nParticlesBeforeCut ) ;
  printEfficiency( info() , "generator anti-particle level cut" ,
                   m_nAntiParticlesAfterCut , m_nAntiParticlesBeforeCut ) ;
  info() << std::endl ;

  if ( "" != m_sigName ) printFraction( info() , "signal " + m_sigName + 
                                        " in sample" , m_nSig , m_nSig + 
                                        m_nSigBar ) ;
  if ( "" != m_sigBarName ) printFraction( info() , "signal " + m_sigBarName + 
                                           " in sample" , m_nSigBar , m_nSig + 
                                           m_nSigBar ) ;  

  info() << std::endl ;

  printArray( info() , m_bHadC , m_bHadCNames , "accepted" ) ;
  printArray( info() , m_antibHadC , m_antibHadCNames , "accepted" ) ;
  printCounter( info() , "accepted (bb)" , m_bbCounter ) ;
  info() << std::endl ;
  
  printArray( info() , m_cHadC , m_cHadCNames , "accepted" ) ;
  printArray( info() , m_anticHadC , m_anticHadCNames , "accepted" ) ;
  printCounter( info() , "accepted (cc)" , m_ccCounter ) ;
  info() << std::endl ;
  
  printArray( info() , m_bExcitedC , m_bExcitedCNames , "accepted" ) ;
  info() << std::endl ;
  
  printArray( info() , m_cExcitedC , m_cExcitedCNames , "accepted" ) ;
  info() << endmsg ;
}

//=============================================================================
// Isolate signal to produce "clean" events
//=============================================================================
StatusCode Signal::isolateSignal( const HepMC::GenParticle * theSignal ) 
  const {

  StatusCode sc = StatusCode::SUCCESS ;
  // Create a new event to contain isolated signal decay tree
  LHCb::HepMCEvent * mcevt = new LHCb::HepMCEvent( ) ;
  mcevt -> setGeneratorName( m_hepMCName + "_clean" ) ;
  
  if ( 0 == theSignal -> production_vertex() ) 
    return Error( "Signal particle has no production vertex." ) ;
  
  // create a new vertex and a new HepMC Particle for the root particle
  // (a copy of which will be associated to the new HepMC event) 

  HepMC::GenVertex * newVertex =
    new HepMC::GenVertex( theSignal -> production_vertex() -> position() ) ;

  HepMC::GenEvent * hepMCevt = mcevt -> pGenEvt() ;

  hepMCevt -> add_vertex( newVertex ) ;
  
  HepMC::GenParticle * theNewParticle =
    new HepMC::GenParticle( theSignal -> momentum() , theSignal -> pdg_id() ,
                            theSignal -> status() ) ;
  
  newVertex -> add_particle_out( theNewParticle ) ;
  
  // Associate the new particle to the HepMC event
  // and copy all tree to the new HepMC event
  sc = fillHepMCEvent( theNewParticle , theSignal ) ;
  hepMCevt -> set_signal_process_vertex( theNewParticle -> end_vertex() ) ;
  
  if ( ! sc.isSuccess( ) ) 
    return Error( "Could not fill HepMC event for signal tree" , sc ) ;
                            
  // Check if container already exists
  if ( exist< LHCb::HepMCEvents >( LHCb::HepMCEventLocation::Signal ) ) 
    return Error( "SignalDecayTree container already exists !" ) ;
  
  LHCb::HepMCEvents * hepVect = new LHCb::HepMCEvents ;
  hepVect -> insert( mcevt ) ;
  
  // Register new location and store HepMC event
  put( hepVect , LHCb::HepMCEventLocation::Signal ) ;
  
  return sc ;
}

//=============================================================================
// Fill HepMC event from a HepMC tree
//=============================================================================
StatusCode Signal::fillHepMCEvent( HepMC::GenParticle * theNewParticle ,
                                   const HepMC::GenParticle * theOldParticle ) 
  const {
  StatusCode sc = StatusCode::SUCCESS ;
  //
  // Copy theOldParticle to theNewParticle in theEvent
  // theNewParticle already exist and is created outside this function
  HepMC::GenVertex * oVertex = theOldParticle -> end_vertex() ;
  if ( 0 != oVertex ) {
    // Create decay vertex and associate it to theNewParticle
    HepMC::GenVertex * newVertex =
      new HepMC::GenVertex( oVertex -> position() ) ;
    newVertex -> add_particle_in( theNewParticle ) ;
    theNewParticle -> parent_event() -> add_vertex( newVertex ) ;

    // loop over child particle of this vertex after sorting them
    std::list< const HepMC::GenParticle * > outParticles ;
    for ( HepMC::GenVertex::particles_out_const_iterator itP = 
            oVertex -> particles_out_const_begin() ; 
          itP != oVertex -> particles_out_const_end() ; ++itP )
      outParticles.push_back( (*itP ) ) ;

    outParticles.sort( HepMCUtils::compareHepMCParticles ) ;

    std::list< const HepMC::GenParticle * >::const_iterator child ;
    for ( child = outParticles.begin( ) ; child != outParticles.end( ) ; 
          ++child ) {
      
      // Create a new particle for each daughter of theOldParticle
      HepMC::GenParticle * newPart =
        new HepMC::GenParticle ( (*child) -> momentum () ,
                                 (*child) -> pdg_id ()   ,
                                 (*child) -> status ()   ) ;
      newVertex -> add_particle_out( newPart ) ;
      
      const HepMC::GenParticle * theChild = (*child) ;
      // Recursive call : fill the event with the daughters
      sc = fillHepMCEvent( newPart , theChild ) ;
      
      if ( ! sc.isSuccess() ) return sc ;
    }
  }
  return sc ;
}

//=============================================================================
// Choose one particle in acceptance 
//=============================================================================
HepMC::GenParticle * Signal::chooseAndRevert( const ParticleVector & 
                                              theParticleList , 
                                              bool & isInverted ,
                                              bool & hasFlipped ) {
  HepMC::GenParticle * theSignal ;
  isInverted = false ;
  hasFlipped = false ;

  unsigned int nPart = theParticleList.size() ;
  if ( nPart > 1 ) {
    unsigned int iPart = 
      (unsigned int) floor( nPart * m_flatGenerator() ) ;
    theSignal = theParticleList[ iPart ] ;

    // Now erase daughters of the other particles in particle list
    for ( unsigned int i = 0 ; i < nPart ; ++i ) {
      if ( i != iPart ) 
        HepMCUtils::RemoveDaughters( theParticleList[ i ] ) ;
    }
  } else if ( 1 == nPart ) theSignal = theParticleList.front() ;
  else return 0 ;

  if ( theSignal -> momentum().pz() < 0 ) {
    revertEvent( theSignal -> parent_event() ) ;
    isInverted = true ;
  }

  // now force the particle to decay
  if ( m_cpMixture ) m_decayTool -> enableFlip() ;
  m_decayTool -> generateSignalDecay( theSignal , hasFlipped ) ;
  
  return theSignal ;
}

//=============================================================================
// Establish correct multiplicity of signal
//=============================================================================
bool Signal::ensureMultiplicity( const unsigned int nSignal ) {
  if ( ! m_cpMixture ) return true ;
  if ( nSignal > 1 ) return true ;
  return ( m_flatGenerator() >= ( ( 1. - m_signalBr ) / 
                                  ( 2. - m_signalBr ) ) ) ;
}

//=============================================================================
// update counters for efficiency calculations
//=============================================================================
void Signal::updateCounters( const ParticleVector & particleList , 
                             unsigned int & particleCounter , 
                             unsigned int & antiparticleCounter ,
                             bool onlyForwardParticles , 
                             bool isInverted ) const {
  int nP( 0 ) , nAntiP( 0 ) ;
  ParticleVector::const_iterator from = particleList.begin() ;
  ParticleVector::const_iterator to = particleList.end() ;
  
  if ( onlyForwardParticles ) {
    // if the particle has been inverted z -> -z, do not count it
    if ( ! isInverted ) {
      nP = std::count_if( from , to , isForwardParticle() ) ;
      nAntiP = std::count_if( from , to , isForwardAntiParticle() ) ;
    }
  } else {
    nP = std::count_if( from , to , isParticle() ) ;
    nAntiP = particleList.size() - nP ;
  }

  particleCounter += nP ;
  antiparticleCounter += nAntiP ;
}

