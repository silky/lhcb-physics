// $Id: ExternalGenerator.cpp,v 1.12 2006-02-01 21:27:41 robbep Exp $
// Include files 

// local
#include "Generators/ExternalGenerator.h"

// SEAL
#include "SealBase/StringOps.h"

// Gaudi
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/IDecayTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/LhaPdf.h"
#include "Generators/StringParse.h"

// Calls to FORTRAN routines
#ifdef WIN32
extern "C" {
  void __stdcall LHAPDFGAUSS_INIT( int * ) ;
  void __stdcall LHAPDFGAUSS_END ( ) ;
}
#else
extern "C" {
  void lhapdfgauss_init_( int * ) ;
  void lhapdfgauss_end_ ( ) ;
}
#endif

//-----------------------------------------------------------------------------
// Implementation file for class : ExternalGenerator
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ExternalGenerator::ExternalGenerator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent ) , 
    m_decayTool( 0 ) , 
    m_cutTool  ( 0 ) { 
    m_defaultLhaPdfSettings.clear() ;
    declareInterface< ISampleGenerationTool >( this ) ;
    declareProperty( "ProductionTool" , 
                     m_productionToolName = "PythiaProduction" ) ; 
    declareProperty( "DecayTool" , m_decayToolName = "EvtGenDecay" ) ;
    declareProperty( "CutTool" , m_cutToolName = "LHCbAcceptance" ) ;
    declareProperty( "LhaPdfCommands" , m_userLhaPdfSettings ) ;
    m_defaultLhaPdfSettings.push_back( "lhacontrol lhaparm 17 LHAPDF" ) ;
    m_defaultLhaPdfSettings.push_back( "lhacontrol lhaparm 16 NOSTAT" ) ;
  }

//=============================================================================
// Destructor
//=============================================================================
ExternalGenerator::~ExternalGenerator( ) { ; }

//=============================================================================
// Initialization method
//=============================================================================
StatusCode ExternalGenerator::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Handle LHAPDF output
  int ival ;
  if ( msgLevel( MSG::DEBUG ) ) {
    ival = 1 ;    
    LhaPdf::lhacontrol().setlhaparm( 19 , "DEBUG" ) ;
  }
  else {
    ival = 0 ;
    LhaPdf::lhacontrol().setlhaparm( 19 , "SILENT" ) ;
  }
  
#ifdef WIN32
    LHAPDFGAUSS_INIT( &ival ) ;
#else
    lhapdfgauss_init_ ( &ival ) ;
#endif

  // Set default LHAPDF parameters:
  sc = parseLhaPdfCommands( m_defaultLhaPdfSettings ) ;
  // Set user LHAPDF parameters:
  sc = parseLhaPdfCommands( m_userLhaPdfSettings ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Unable to read LHAPDF commands" , sc ) ;

  // retrieve the particle property service
  IParticlePropertySvc * ppSvc = 
    svc< IParticlePropertySvc >( "ParticlePropertySvc" ) ;  

  // obtain the Decay Tool 
  // (ATTENTION: it has to be initialized before the production tool)
  if ( "" != m_decayToolName ) 
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;

  // obtain the Production Tool
  m_productionTool = tool< IProductionTool >( m_productionToolName , this ) ;

  // update the particle properties of the production tool
  IParticlePropertySvc::const_iterator iter ;
  for ( iter = ppSvc -> begin() ; iter != ppSvc -> end() ; ++iter ) {
    if ( ! m_productionTool -> isSpecialParticle( *iter ) ) 
      m_productionTool -> updateParticleProperties( *iter ) ;
    // set stable in the Production generator all particles known to the
    // decay tool
    if ( 0 != m_decayTool )
      if ( m_decayTool -> isKnownToDecayTool( (*iter)->pdgID() ) ) 
        m_productionTool -> setStable( *iter ) ;    
  }

  release( ppSvc ) ;

  // obtain the cut tool
  if ( "" != m_cutToolName ) 
    m_cutTool = tool< IGenCutTool >( m_cutToolName , this ) ;

  // now debug printout of Production Tool 
  // has to be after all initializations to be sure correct values are printed
  m_productionTool -> printRunningConditions( ) ;

  // set up the name to assign to HepMC events
  seal::StringList strList = 
    seal::StringOps::split( m_productionTool -> name() , "." ) ;

  m_hepMCName = seal::StringOps::remove( strList.back() , "Production" ) ;
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Decay heavy excited particles
//=============================================================================
StatusCode ExternalGenerator::decayHeavyParticles( HepMC::GenEvent * theEvent, 
                                                   const double mass ,
                                                   const int pid ) const {
  StatusCode sc ;
  
  m_decayTool -> disableFlip() ;

  HepMC::GenEvent::particle_iterator it ;
  for ( it = theEvent -> particles_begin() ; 
        it != theEvent -> particles_end() ; ++ it ) {
    
    if ( ( (*it) -> momentum().m() * GeV > mass ) &&
         ( LHCb::HepMCEvent::StableInProdGen == (*it) -> status() ) && 
         ( pid != abs( (*it) -> pdg_id() ) ) ) {
      
      if ( m_decayTool -> isKnownToDecayTool( (*it) -> pdg_id() ) ) {
        sc = m_decayTool -> generateDecayWithLimit( *it , pid ) ;
        if ( ! sc.isSuccess() ) return sc ;
        // if excited particle is unknown to EvtGen give error to oblige
        // us to define it in EvtGen decay table
      } else return Error( "Unknown undecayed excited particle !" ) ;
      
    }
  }  

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Check the presence of a particle of given type in event
// Attention : pidList must be sorted before begin used in this function
//=============================================================================
bool ExternalGenerator::checkPresence( const PIDs & pidList ,
                                       const HepMC::GenEvent * theEvent ,
                                       ParticleVector & particleList ) const {
  particleList.clear( ) ;
  HepMC::GenEvent::particle_const_iterator it ;
  for ( it = theEvent -> particles_begin() ; 
        it != theEvent -> particles_end() ; ++it ) 
    if ( std::binary_search( pidList.begin() , pidList.end() ,
                             (*it) -> pdg_id() ) ) 
      if ( ( 3 != (*it) -> status() ) && ( IsBAtProduction( *it ) ) )
        particleList.push_back( *it ) ;

  return ( ! particleList.empty() ) ;
}

//=============================================================================
// invert the event
//=============================================================================
void ExternalGenerator::revertEvent( HepMC::GenEvent * theEvent ) const {
  HepMC::GenEvent::vertex_iterator itv ;
  double x, y, z, t ;
  for ( itv = theEvent -> vertices_begin() ;
        itv != theEvent -> vertices_end() ; ++itv ) {
    x = (*itv) -> position().x() ;
    y = (*itv) -> position().y() ;
    z = (*itv) -> position().z() ;
    t = (*itv) -> position().t() ;
    (*itv) -> set_position( HepLorentzVector( x, y, -z, t ) ) ;
  }

  HepMC::GenEvent::particle_iterator itp ;
  double px, py, pz, E ;
  for ( itp = theEvent -> particles_begin() ;
        itp != theEvent -> particles_end() ; ++itp ) {
    px = (*itp) -> momentum().px() ;
    py = (*itp) -> momentum().py() ;
    pz = (*itp) -> momentum().pz() ;
    E  = (*itp) -> momentum().e() ;
    (*itp) -> set_momentum( HepLorentzVector( px, py, -pz, E ) ) ;
  }      
}

//=============================================================================
// count the number of particles with pz > 0
//=============================================================================
unsigned int ExternalGenerator::nPositivePz( const ParticleVector 
                                             & particleList ) const {
  ParticleVector::const_iterator iter ;
  unsigned int nP = 0 ;
  for ( iter = particleList.begin() ; iter != particleList.end() ; ++iter ) 
    if ( (*iter)->momentum().pz() > 0 ) nP++ ;
  
  return nP ;
}


//=============================================================================
// Set up event
//=============================================================================
void ExternalGenerator::prepareInteraction( LHCb::HepMCEvents * theEvents ,
    LHCb::GenCollisions * theCollisions , HepMC::GenEvent * & theGenEvent ,  
    LHCb::GenCollision * & theGenCollision ) const {
  LHCb::HepMCEvent * theHepMCEvent = new LHCb::HepMCEvent( ) ;
  theHepMCEvent -> setGeneratorName( m_hepMCName ) ;
  HepMC::GenEvent * theGE = new HepMC::GenEvent( ) ;
  theHepMCEvent -> setPGenEvt( theGE ) ;

  theGenCollision = new LHCb::GenCollision() ;
  
  theGenEvent = theHepMCEvent -> pGenEvt() ;
  theGenCollision -> setEvent( theHepMCEvent ) ;

  theEvents -> insert( theHepMCEvent ) ;
  theCollisions -> insert( theGenCollision ) ;
}

//=============================================================================
// Returns true if B is first B (removing oscillation B)
//=============================================================================
bool ExternalGenerator::IsBAtProduction( const HepMC::GenParticle * thePart ) 
  const {
  if ( ( abs( thePart -> pdg_id() ) != 511 ) && 
       ( abs( thePart -> pdg_id() ) != 531 ) ) return true ;
  if ( 0 == thePart -> production_vertex() ) return true ;
  HepMC::GenVertex * theVertex = thePart -> production_vertex() ;
  if ( 1 != theVertex -> particles_in_size() ) return true ;
  HepMC::GenParticle * theMother = 
    (* theVertex -> particles_in_const_begin() ) ;
  if ( theMother -> pdg_id() == - thePart -> pdg_id() ) return false ;
  return true ;
}

//=============================================================================
// Parse LHAPDF commands stored in a vector
//=============================================================================
StatusCode ExternalGenerator::parseLhaPdfCommands( const CommandVector & 
                                                   theCommandVector ) const {
  //
  // Parse Commands and Set Values from Properties Service...
  //
  CommandVector::const_iterator iter ;
  for ( iter = theCommandVector.begin() ; theCommandVector.end() != iter ; 
        ++iter ) {
    debug() << " Command is: " << (*iter) << endmsg ;
    StringParse mystring( *iter ) ;
    std::string block = mystring.piece(1);
    std::string entry = mystring.piece(2);
    std::string str   = mystring.piece(4);
    int    int1  = mystring.intpiece(3);
    double fl1   = mystring.numpiece(3);
    
    // Note that Pythia needs doubles hence the convert here
    debug() << block << " block  " << entry << " item  " << int1 
            << "  value " << fl1 << " str " << str << endmsg ;
    if ( "lhacontrol" == block )  
      if      ( "lhaparm" == entry ) 
        LhaPdf::lhacontrol().setlhaparm( int1 , str ) ;
      else if ( "lhavalue" == entry ) 
        LhaPdf::lhacontrol().setlhavalue( int1 , fl1 ) ;
      else return Error( std::string( "LHAPDF ERROR, block LHACONTROL has " ) +
                         std::string( "LHAPARM and LHAVALUE: YOU HAVE " ) + 
                         std::string( "SPECIFIED " ) + std::string( entry ) ) ;
    else return Error( std::string( " ERROR in LHAPDF PARAMETERS   " ) +
                       std::string( block ) +
                       std::string( " is and invalid common block name !" ) ) ;
  }
  
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Finalize method
//=============================================================================
StatusCode ExternalGenerator::finalize( ) {
  if ( ! msgLevel( MSG::DEBUG ) ) 
#ifdef WIN32
    LHAPDFGAUSS_END( ) ;
#else
  lhapdfgauss_end_() ;
  std::string delcmd( "rm -f lhapdf_init.tmp" ) ;
  system( delcmd.c_str() ) ;
#endif
  return GaudiTool::finalize() ;
}
