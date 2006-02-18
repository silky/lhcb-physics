
//===============================================================================
/** @file RichSignal.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichSignal
 *
 *  CVS Log :-
 *  $Id: RichSignal.cpp,v 1.10 2006-02-18 16:04:06 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#include "RichSignal.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSignal>  s_factory;
const         IAlgFactory& RichSignalFactory = s_factory ;

// Standard constructor, initializes variables
RichSignal::RichSignal( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : RichAlgBase        ( name, pSvcLocator ),
    m_mcDeposits       ( 0 ),
    m_smartIDTool      ( 0 ),
    m_truth            ( 0 ),
    m_smartIDnotFound  ( Rich::NRiches, 0 ),
    m_smartIDInvalid   ( Rich::NRiches, 0 )
{

  declareProperty( "HitLocation",
                   m_RichHitLocation =  MCRichHitLocation::Default );
  declareProperty( "PrevLocation",
                   m_RichPrevLocation = "Prev/" + MCRichHitLocation::Default );
  declareProperty( "PrevPrevLocation",
                   m_RichPrevPrevLocation = "PrevPrev/" + MCRichHitLocation::Default );
  declareProperty( "NextLocation",
                   m_RichNextLocation = "Next/" + MCRichHitLocation::Default );
  declareProperty( "NextNextLocation",
                   m_RichNextNextLocation = "NextNext/" + MCRichHitLocation::Default );
  declareProperty( "LHCBackgroundLocation",
                   m_lhcBkgLocation = "LHCBackground/" + MCRichHitLocation::Default );
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = MCRichDepositLocation::Default );

  declareProperty( "UseSpillover",     m_doSpillover = true );
  declareProperty( "UseLHCBackground", m_doLHCBkg = true );

}

RichSignal::~RichSignal() {};

StatusCode RichSignal::initialize()
{
  // Initialize base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // randomn number generator
  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  // tools
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );
  acquireTool( "RichMCTruthTool", m_truth, 0, true       );

  return sc;
}

StatusCode RichSignal::execute()
{
  debug() << "Execute" << endreq;

  // Form new container of MCRichDeposits
  m_mcDeposits = new MCRichDeposits();
  put( m_mcDeposits, m_RichDepositLocation );

  // initialise debug counters
  m_smartIDnotFound[Rich::Rich1] = 0;
  m_smartIDnotFound[Rich::Rich2] = 0;
  m_smartIDInvalid[Rich::Rich1]  = 0;
  m_smartIDInvalid[Rich::Rich2]  = 0;

  // Process main event
  // must be done first (so that first associated hit is signal)
  ProcessEvent( m_RichHitLocation, 0, 0 );

  // if requested, process spillover events
  if ( m_doSpillover )
  {
    ProcessEvent( m_RichPrevPrevLocation, -50, -2 );
    ProcessEvent( m_RichPrevLocation,     -25, -1 );
    ProcessEvent( m_RichNextLocation,      25,  1 );
    ProcessEvent( m_RichNextNextLocation,  50,  2 );
  }

  // if requested, process LHC background
  if ( m_doLHCBkg ) { ProcessEvent( m_lhcBkgLocation, 0, 0 ); }

  // Debug Printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Found in Rich(1/2) " << m_smartIDnotFound[Rich::Rich1] << "/"
            << m_smartIDnotFound[Rich::Rich2] << " hits outside active pixels" << endreq
            << "Found in Rich(1/2) " << m_smartIDInvalid[Rich::Rich1] << "/"
            << m_smartIDInvalid[Rich::Rich2] << " hits with invalid RichSmartIDs" << endreq
            << "Created overall " << m_mcDeposits->size()
            << " MCRichDeposits at " << m_RichDepositLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::ProcessEvent( const std::string & hitLoc,
                                     const double tofOffset,
                                     const int eventType ) const
{

  // Load hits
  if ( exist<MCRichHits>(hitLoc) )
  {
    ++counter( "Found MCRichHits at " + hitLoc );
  }
  else { return StatusCode::SUCCESS; }
  MCRichHits * hits = get<MCRichHits>( hitLoc );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully located " << hits->size()
            << " MCRichHits at " << hitLoc << " : Pointer=" << hits << endreq;
  }

  unsigned int nDeps(0);
  for ( MCRichHits::const_iterator iHit = hits->begin();
        iHit != hits->end(); ++iHit )
  {

    // debug - turn off background hits
    //if ( (*iHit)->backgroundHit() ) continue;
    //if ( (*iHit)->hpdQuartzCK() || (*iHit)->gasQuartzCK() ) continue;

    // Is hit in active pixel
    RichSmartID tempID;
    if ( (m_smartIDTool->smartID((*iHit)->entry(),tempID)).isSuccess() )
    {
      // is smart ID "valid"
      if ( tempID.pixelDataAreValid() )
      {

        // For the time being strip any sub-pixel information
        const RichSmartID id = tempID.pixelID();

        // Create a new deposit
        MCRichDeposit* dep = new MCRichDeposit();
        m_mcDeposits->insert( dep );
        ++nDeps;

        // Set RichSmartID
        dep->setSmartID( id );

        // set parent hit
        dep->setParentHit( *iHit );

        // Hit energy
        dep->setEnergy( (*iHit)->energy() );

        // TOF
        double tof = tofOffset + (*iHit)->timeOfFlight();
        // Global shift for Rich2.
        if ( Rich::Rich2 == (*iHit)->rich() ) tof -= 40;
        dep->setTime( tof );

        // get history from hit
        MCRichDigitHistoryCode hist = (*iHit)->mcRichDigitHistoryCode();

        // add event type to history
        if      (  0 == eventType ) { hist.setSignalEvent(true);   }
        else if ( -1 == eventType ) { hist.setPrevEvent(true);     }
        else if ( -2 == eventType ) { hist.setPrevPrevEvent(true); }
        else if (  1 == eventType ) { hist.setNextEvent(true);     }
        else if (  2 == eventType ) { hist.setNextNextEvent(true); }

        // Update history in dep
        dep->setHistory( hist );

      } // valid smart ID
      else { ++m_smartIDInvalid[(*iHit)->rich()]; }

    } // active hit if
    else { ++m_smartIDnotFound[(*iHit)->rich()]; }

  } // hit loop

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << nDeps << " MCRichDeposits for " << hitLoc << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichSignal::finalize()
{
  // finalize random number generator
  m_rndm.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}

