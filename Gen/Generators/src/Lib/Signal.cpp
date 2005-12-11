// $Id: Signal.cpp,v 1.7 2005-12-11 23:22:30 robbep Exp $
// Include files 

// local
#include "Generators/Signal.h"

// from Gaudi
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IRndmGenSvc.h" 

// from Event
#include "Event/HepMCEvent.h"

// from Generators
#include "Generators/IDecayTool.h"
#include "Generators/GenCounters.h"

// Function to sort HepMC::GenParticles according to their barcode
bool compareHepMCParticles( const HepMC::GenParticle * part1 ,
                            const HepMC::GenParticle * part2 ) {
  return ( part1->barcode() < part2->barcode() ) ;
}

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
    m_signalMass      ( 0.) ,
    m_signalPID       ( 0 ) ,
    m_cpMixture       ( true ) { 
  declareProperty( "SignalPIDList" , m_pidVector ) ;
  declareProperty( "Clean" , m_cleanEvents ) ;
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
    m_signalMass = prop -> mass() ;
    m_signalPID  = abs( prop -> pdgID() ) ;
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

  printEfficiency( info() , "particle generator level cut" , 
                   m_nParticlesAfterCut ,  m_nParticlesBeforeCut ) ;
  printEfficiency( info() , "anti-particle generator level cut" , 
                   m_nAntiParticlesAfterCut , m_nAntiParticlesBeforeCut ) ;
  info() << endmsg ;
}

//=============================================================================
// Isolate signal to produce "clean" events
//=============================================================================
StatusCode Signal::isolateSignal( const HepMC::GenParticle * theSignal ) 
  const {
  StatusCode sc = StatusCode::SUCCESS ;
  // Create a new event to contain isolated signal decay tree
  HepMCEvent * mcevt = new HepMCEvent( name() , 1 , 1 ) ;
  HepMC::GenEvent * hepMCevt = mcevt -> pGenEvt() ;
  
  if ( 0 == theSignal -> production_vertex() ) 
    return Error( "Signal particle has no production vertex." ) ;
  
  // create a new vertex and a new HepMC Particle for the root particle
  // (a copy of which will be associated to the new HepMC event) 

  HepMC::GenVertex * newVertex =
    new HepMC::GenVertex( theSignal -> production_vertex() -> position() ) ;
  
  hepMCevt -> add_vertex( newVertex ) ;
  
  HepMC::GenParticle * theNewParticle =
    new HepMC::GenParticle( theSignal -> momentum() , theSignal -> pdg_id() ,
                            theSignal -> status() ) ;
  
  newVertex -> add_particle_out( theNewParticle ) ;
  hepMCevt -> set_signal_process_vertex( theSignal -> production_vertex() ) ;
  
  // Associate the new particle to the HepMC event
  // and copy all tree to the new HepMC event
  sc = fillHepMCEvent( theNewParticle , theSignal ) ;
  
  if ( ! sc.isSuccess( ) ) 
    return Error( "Could not fill HepMC event for signal tree" , sc ) ;
                            
  // Check if container already exists
  if ( exist< HepMCEvents >( "/Event/Gen/SignalDecayTree" ) ) 
    return Error( "SignalDecayTree container already exists !" ) ;
  
  HepMCEvents * hepVect = new HepMCEvents ;
  hepVect -> insert( mcevt ) ;
  
  // Register new location and store HepMC event
  sc = put( hepVect , "/Event/Gen/SignalDecayTree" ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not register SignalDecayTree" ) ;
  
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
    theNewParticle -> parent_event() -> add_vertex( newVertex ) ;
    newVertex -> add_particle_in( theNewParticle ) ;

    // loop over child particle of this vertex after sorting them
    std::list< const HepMC::GenParticle * > outParticles ;
    std::copy( oVertex -> particles_out_const_begin() , 
               oVertex -> particles_out_const_end() , outParticles.begin() ) ;
    outParticles.sort( compareHepMCParticles ) ;

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
// Choose one particle in acceptance and revert if necessary
//=============================================================================
HepMC::GenParticle * Signal::chooseAndRevert( const ParticleVector & 
                                              theParticleList ) {
  HepMC::GenParticle * theSignal ;
  
  unsigned int nPart = theParticleList.size() ;
  if ( nPart > 1 ) {
    unsigned int iPart = 
      (unsigned int) floor( nPart * m_flatGenerator() ) ;
    theSignal = theParticleList[ iPart ] ;
  } else if ( 1 == nPart ) theSignal = theParticleList.front() ;
  else return 0 ;
  
  if ( theSignal -> momentum().pz() < 0 ) {
    revertEvent( theSignal -> parent_event() ) ;
    m_nInvertedEvents++ ;
  }
  
  return theSignal ;
}

//=============================================================================
// Establish correct multiplicity of signal
//=============================================================================
bool Signal::ensureMultiplicity( const unsigned int nSignal ) {
  if ( ! m_cpMixture ) return true ;
  if ( nSignal > 1 ) return true ;
  return (m_flatGenerator() >= ( ( 1. - m_signalBr ) / 
                                 ( 2. - m_signalBr ) ) ) ;
}

//=============================================================================
// update counters for efficiency calculations
//=============================================================================
void Signal::updateCounters( const ParticleVector & particleList , 
                             unsigned int & particleCounter , 
                             unsigned int & antiparticleCounter ,
                             bool onlyForwardParticles ) const {
  int nP( 0 ) , nAntiP( 0 ) ;
  ParticleVector::const_iterator from = particleList.begin() ;
  ParticleVector::const_iterator to = particleList.end() ;
  
  if ( onlyForwardParticles ) {
    nP = std::count_if( from , to , isForwardParticle() ) ;
    nAntiP = std::count_if( from , to , isForwardAntiParticle() ) ;
  } else {
    nP = std::count_if( from , to , isParticle() ) ;
    nAntiP = particleList.size() - nP ;
  }
  particleCounter += nP ;
  antiparticleCounter += nAntiP ;
}

