// $Id: GetTrackerHitsAlg.cpp,v 1.13 2007-10-02 16:22:00 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaHitsByName.h"

// from GiGaCnv
#include "GiGaCnv/IGiGaKineCnvSvc.h" 
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
#include "GiGaCnv/GiGaKineRefTable.h"

// from Geant4
#include "G4HCofThisEvent.hh"

// from LHCb
#include "Event/MCExtendedHit.h"
#include "DetDesc/DetectorElement.h"

// local
#include "GetTrackerHitsAlg.h"
#include "TrackerHit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GetTrackerHitsAlg
//
// 2005-10-02 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GetTrackerHitsAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetTrackerHitsAlg::GetTrackerHitsAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_gigaSvc      ( 0 )
  , m_gigaKineCnvSvc ( 0 )
{
  declareProperty( "GiGaService",    m_gigaSvcName  = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName  = IGiGaCnvSvcLocation::Kine );
  declareProperty( "ExtendedInfo",   m_extendedInfo = false );
  declareProperty( "MCHitsLocation", m_hitsLocation = "" );
  declareProperty( "CollectionName", m_colName = "" );
  declareProperty( "Detector",       m_detName = "" );

}

//=============================================================================
// Destructor
//=============================================================================
GetTrackerHitsAlg::~GetTrackerHitsAlg() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetTrackerHitsAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if( "" == m_hitsLocation ) {
    fatal() << "Property MCHitsLocation need to be set! " << endmsg;
    return StatusCode::FAILURE;
  }
  if( "" == m_colName ) {
    fatal() << "Property CollectionName need to be set! " << endmsg;
    return StatusCode::FAILURE;
  }
  if( "" == m_detName ) {
    fatal() << "Property Detector need to be set! " << endmsg;
    return StatusCode::FAILURE;
  }

  debug() << " The hits " << m_hitsLocation  << endmsg;
  debug() << " will be taken from G4 collection " << m_colName  << endmsg;
  debug() << " for detector " << m_detName << endmsg;

  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName ); // GiGa has to already exist!

  // get kineCnv service that hold the MCParticle/Geant4 table list
  m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );

  // get the detector element
  m_detector = getDet<DetectorElement>(m_detName);
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetTrackerHitsAlg::execute() {

  debug() << "==> Execute" << endmsg;

  if( 0 == gigaSvc() ) {
    return Error( " execute(): IGiGaSvc* points to NULL" );
  }

  // Create the MCHits and put them in the TES
  // Cannot use 
  // MCHits* hits = getOrCreate<MCHits,MCHits>( m_hitsLocation );
  // because triggers convertion
  LHCb::MCHits* hits = new LHCb::MCHits();
  put( hits, m_hitsLocation );
  
  // Get the G4 necessary hit collection from GiGa
  GiGaHitsByName col( m_colName );
  *gigaSvc() >> col;   // also StatusCode sc = retrieveHitCollection( col );
                       // in TRY/CATCH&PRINT
  
  if( 0 == col.hits() ) { 
    return Warning( "The hit collection='" + m_colName + "' is not found!",
                    StatusCode::SUCCESS ); 
  }
  
  const TrackerHitsCollection* hitCollection = trackerHits( col.hits() );
  if( 0 == hitCollection ) { 
    return Error( "Wrong Collection type" );
  }
  
  // The MCParticles should have already been filled
  if( !( exist<LHCb::MCParticles>( LHCb::MCParticleLocation::Default ) ) ) {
    return Error( "MCParticles do not exist at'" 
                  + LHCb::MCParticleLocation::Default +"'" );
  }
  
  // reserve elements on output container
  int numOfHits = hitCollection->entries();
  if( numOfHits > 0 ) {
    hits->reserve( numOfHits );
  }

  // tranform G4Hit into MCHit and insert it in container
  for( int iG4Hit = 0; iG4Hit < numOfHits; ++iG4Hit ) { 
    
    // create hit or extended hit depending on choice
    if ( m_extendedInfo ) {
      LHCb::MCExtendedHit* newHit = new LHCb::MCExtendedHit();
      fillHit( (*hitCollection)[iG4Hit], newHit );
      Gaudi::XYZVector mom( (*hitCollection)[iG4Hit]->GetMomentum() );
      newHit->setMomentum( mom );
      hits->add( newHit );
    }
    else {
      LHCb::MCHit* newHit = new LHCb::MCHit();
      fillHit( (*hitCollection)[iG4Hit], newHit );
      hits->add( newHit );
    }
  }
  
  // Check that all hits have been transformed
  if( (size_t) hits->size() != (size_t) hitCollection->entries() ) {
    return Error("MCHits and G4TrackerHitsCollection have different sizes!");
  }  

  return StatusCode::SUCCESS;
  
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetTrackerHitsAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
//  Fill MCHit 
//=============================================================================

void GetTrackerHitsAlg::fillHit( TrackerHit* g4Hit, LHCb::MCHit* mcHit ) {
  
  // fill data members
  Gaudi::XYZPoint entry( g4Hit->GetEntryPos() );
  Gaudi::XYZPoint exit( g4Hit->GetExitPos() );
  mcHit->setEntry( entry );
  mcHit->setDisplacement( exit-entry );
  mcHit->setEnergy( g4Hit->GetEdep() );
  mcHit->setTime( g4Hit->GetTimeOfFlight() );
  mcHit->setP( g4Hit->GetMomentum().mag() );
 
  // get sensitive detector identifier using mid point
  int detID = m_detector->sensitiveVolumeID( mcHit->midPoint() );
  mcHit->setSensDetID(detID);

  // fill reference to MCParticle using the Geant4->MCParticle table
  GiGaKineRefTable& table = kineSvc()->table();
  int trackID = g4Hit->GetTrackID();
  if( table[trackID].particle() ) {
    mcHit->setMCParticle( table[trackID].particle() );
  } else {
    warning() << "No pointer to MCParticle for MCHit associated to G4 trackID: "
              << trackID << endmsg;
  }
  
}

//=============================================================================
