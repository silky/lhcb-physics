// $Id: GetMCRichOpticalPhotonsAlg.cpp,v 1.1 2005-12-22 16:42:43 jonrob Exp $

// local
#include "GetMCRichOpticalPhotonsAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : GetMCRichOpticalPhotonsAlg
//
// 2005-12-06 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<GetMCRichOpticalPhotonsAlg>          s_factory ;
const        IAlgFactory& GetMCRichOpticalPhotonsAlgFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetMCRichOpticalPhotonsAlg::GetMCRichOpticalPhotonsAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator)
  : GetMCRichInfoBase ( name , pSvcLocator )
  , m_nEvts                   ( 0 )
  , m_hitTally                ( Rich::NRiches, 0 )
{
  declareProperty( "MCRichHitsLocation", m_richHitsLocation = MCRichHitLocation::Default );
  declareProperty( "MCRichOpticalPhotonsLocation",
                   m_richPhotonsLocation  = MCRichOpticalPhotonLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
GetMCRichOpticalPhotonsAlg::~GetMCRichOpticalPhotonsAlg() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetMCRichOpticalPhotonsAlg::initialize()
{
  const StatusCode sc = GetMCRichInfoBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise", sc );

  // add custom initialisations here if needed

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetMCRichOpticalPhotonsAlg::execute()
{
  debug() << "Execute" << endmsg;

  // Create the photons and put them in the TES
  MCRichOpticalPhotons * photons = new MCRichOpticalPhotons();
  put( photons, m_richPhotonsLocation );

  // Get the G4 necessary hit collections from GiGa
  G4HCofThisEvent* hitscollections = 0;
  *gigaSvc() >> hitscollections;

  if ( 0 != hitscollections )
  {
    // Get MCRichHits
    const MCRichHits * mcHits = get<MCRichHits>( MCRichHitLocation::Default );

    // note this key is need for consistency with MCRichHit converter
    unsigned int globalKey = 0;
    ++m_nEvts; // Count events
    for ( int iii=0; iii < RichG4HitCollectionName()->RichHCSize(); ++iii ) 
    {
      const std::string & colName = RichG4HitCollectionName()->RichHCName(iii);

      G4SDManager * fSDM = G4SDManager::GetSDMpointer();
      if ( !fSDM ) return Error( "NULL G4SDManager pointer !!" );
      int collectionID = fSDM->GetCollectionID(colName);
      if ( -1 == collectionID ) return StatusCode::SUCCESS;

      RichG4HitsCollection * myCollection =
        dynamic_cast<RichG4HitsCollection*>(hitscollections->GetHC(collectionID));
      if ( 0 == myCollection ) return StatusCode::SUCCESS;

      const int numberofhits = myCollection->entries();
      // reserve space
      photons->reserve( numberofhits );

      //convert hits
      for ( int ihit = 0; ihit < numberofhits; ++ihit ) 
      {

        // Pointer to G4 hit
        RichG4Hit * g4hit = (*myCollection)[ihit];

        // New optical photon object
        MCRichOpticalPhoton * mcPhoton = new MCRichOpticalPhoton();

        // insert in container
        photons->insert( mcPhoton, globalKey );

        // Copy required info from RichG4Hit to RichMCOpticalPhoton

        // Incidence point on HPD
        mcPhoton->setPdIncidencePoint( Gaudi::XYZPoint(g4hit->GetGlobalPEOriginPos()) );

        // Photon energy at production
        mcPhoton->setEnergyAtProduction( static_cast<float>(g4hit->PhotEnergyAtProd()) );

        // Emission point on track
        mcPhoton->setEmissionPoint( Gaudi::XYZPoint(g4hit->GetPhotEmisPt()) );

        // The momentum of the parent track at production
        mcPhoton->setParentMomentum( Gaudi::XYZVector(g4hit->ChTrackMomVect()) );

        // Cherenkov theta and phi at production
        mcPhoton->setCherenkovTheta ( g4hit->ThetaCkvAtProd() );
        mcPhoton->setCherenkovPhi   ( g4hit->PhiCkvAtProd()   );

        // Spherical mirror reflection point
        mcPhoton->setSphericalMirrorReflectPoint( Gaudi::XYZPoint(g4hit->Mirror1PhotonReflPosition()) );

        // Flat mirror reflection point
        mcPhoton->setFlatMirrorReflectPoint( Gaudi::XYZPoint(g4hit->Mirror2PhotonReflPosition()) );

        // Rich detector information
        if ( g4hit->GetCurRichDetNum() < 0 ) {
          //  mcPhoton->setRichInfoValid( false );
        } else {
          //  mcPhoton->setRichInfoValid( true );
          mcPhoton->setRich(static_cast<Rich::DetectorType>(g4hit->GetCurRichDetNum()));
        }

        // Radiator information
        if ( g4hit->GetRadiatorNumber() < 0 ) {
          //  mcPhoton->setRadiatorInfoValid( false );
        } else {
          //  mcPhoton->setRadiatorInfoValid( true );
          mcPhoton->setRadiator(static_cast<Rich::RadiatorType>(g4hit->GetRadiatorNumber()));
        }

        // charged track hitting HPD flag
        mcPhoton->setChargedTrack( g4hit->GetChTrackID() < 0 );

        // Rayleigh scattered flag
        mcPhoton->setScatteredPhoton( g4hit->OptPhotRayleighFlag() > 0 );

        // Overall background flag
        mcPhoton->setBackgroundHit( mcPhoton->chargedTrack() ||
                                    mcPhoton->scatteredPhoton());
        // || !mcPhoton->richInfoValid() );

        // SmartRef to associated MCRichHit
        mcPhoton->setMcRichHit( (*mcHits)[globalKey] );

        // Count photons
        if      ( Rich::Rich1 == mcPhoton->rich() ) ++m_hitTally[Rich::Rich1];
        else if ( Rich::Rich2 == mcPhoton->rich() ) ++m_hitTally[Rich::Rich2];

        // finally, increment the key
        ++globalKey;

      } // loop over g4 hits

    }

    // Should have one opticalphoton for each and every MCRichHit
    if ( (unsigned int)photons->size() != mcHits->size() )
    {
      error() << "Created " << photons->size()
              << " MCRichOpticalPhotons and"
              << mcHits->size() << " MCRichHits !!" << endreq;
    }

  }
  else
  {
    info() << "No RichG4Hits to be converted since no Collections available"
           << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetMCRichOpticalPhotonsAlg::finalize()
{
  const RichStatDivFunctor occ;

  info() << "Av. # MCRichOpticalPhotons : Rich1 = "
         << occ(m_hitTally[Rich::Rich1],m_nEvts)
         << " Rich2 = " << occ(m_hitTally[Rich::Rich2],m_nEvts)
         << endreq;

  return GetMCRichInfoBase::finalize();  // must be called after all other actions
}

//=============================================================================
