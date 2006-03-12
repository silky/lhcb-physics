// $Id: RichDigiDataObjVerifier.cpp,v 1.12 2006-03-12 16:34:17 jonrob Exp $

// local
#include "RichDigiDataObjVerifier.h"

// LHCb namespace
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RichDigiDataObjVerifier
//
// 2002-11-20 : Andy Presland   (Andrew.Presland@cern.ch)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDigiDataObjVerifier>          s_factory ;
const        IAlgFactory& RichDigiDataObjVerifierFactory = s_factory ;

// Standard constructor, initializes variables
RichDigiDataObjVerifier::RichDigiDataObjVerifier( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : RichAlgBase ( name, pSvcLocator )
{

  // Declare job options
  declareProperty( "CheckMCRichDigits",           m_bdMcDigits = true );
  declareProperty( "CheckMCRichHits",             m_bdMCHits   = true );
  declareProperty( "CheckMCRichOpticalPhotons",   m_bdMCPhots  = true );

}

// Destructor
RichDigiDataObjVerifier::~RichDigiDataObjVerifier() {};

// Initialisation
StatusCode RichDigiDataObjVerifier::initialize()
{

  // Initialize base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // add custom initialisations here

  return sc;
}

void RichDigiDataObjVerifier::checkHitsAt( const std::string & location ) const
{
  // MCRichHits
  if ( !exist<MCRichHits>(location) )
  {
    debug() << "Cannot locate MCRichHits at " << location << endreq;
  }
  else
  {
    const MCRichHits * mcHits = get<MCRichHits>(location);
    debug() << "Successfully located " << mcHits->size()
            << " MCRichHits at " << location << endreq;
    unsigned int nHit(0);
    for ( MCRichHits::const_iterator iHit = mcHits->begin();
          iHit != mcHits->end(); ++iHit, ++nHit )
    {
      debug() << "MCRichHit " << nHit << " " << *iHit          
              << " container = " << location << endreq;
      const MCParticle * mcPart = (*iHit)->mcParticle();
      if ( mcPart )
      {
        const std::string mcploc = objectLocation(mcPart->parent());
        debug() << "  Parent MCParticle " << mcPart->key() << " " << mcPart 
                << " container = " << mcploc << endreq;
      }
      else
      {
        Warning( "NULL pointer to parent MCParticle" );
      }
      if ( msgLevel(MSG::DEBUG) )
      {
        std::cout << "   Data members " << **iHit << std::endl;
      }
    }
  }

}

void RichDigiDataObjVerifier::checkPhotsAt( const std::string & location ) const
{
  // MCRichHits
  if ( !exist<MCRichOpticalPhotons>(location) )
  {
    debug() << "Cannot locate MCRichOpticalPhotons at " << location << endreq;
  }
  else
  {
    const MCRichOpticalPhotons * mcPhots = get<MCRichOpticalPhotons>(location);
    debug() << "Successfully located " << mcPhots->size()
            << " MCRichOpticalPhotons at " << location << endreq;
    for ( MCRichOpticalPhotons::const_iterator iP = mcPhots->begin();
          iP != mcPhots->end(); ++iP )
    {
      debug() << "MCRichOpticalPhoton " << (*iP)->key() << " " << *iP 
              << " container = " << location
              << endreq;
      if ( msgLevel(MSG::DEBUG) )
      {
        std::cout << "   Data members " << **iP << std::endl;
      }
      // get associated MCRichHit
      const MCRichHit * mchit = (*iP)->mcRichHit();
      if ( mchit )
      {
        const std::string mchloc = objectLocation(mchit->parent());
        debug() << "  Associated MCRichHit " << mchit 
                << " container = " << mchloc << endreq;
      }
      else
      {
        Warning( "NULL pointer to associated MCRichHit" );
      }

    }
  }

}

// Main execution
StatusCode RichDigiDataObjVerifier::execute()
{
  debug() << "Execute" << endreq;

  // MCRichHits
  if ( m_bdMCHits )
  {
    checkHitsAt (               MCRichHitLocation::Default );
    checkHitsAt ( "Prev/"     + MCRichHitLocation::Default );
    checkHitsAt ( "PrevPrev/" + MCRichHitLocation::Default );
    checkHitsAt ( "Next/"     + MCRichHitLocation::Default );
    checkHitsAt ( "NexNext/"  + MCRichHitLocation::Default );
  }

  // MCRichOpticalPhotons
  if ( m_bdMCPhots )
  {
    checkPhotsAt (               MCRichOpticalPhotonLocation::Default );
    checkPhotsAt ( "Prev/"     + MCRichOpticalPhotonLocation::Default );
    checkPhotsAt ( "PrevPrev/" + MCRichOpticalPhotonLocation::Default );
    checkPhotsAt ( "Next/"     + MCRichOpticalPhotonLocation::Default );
    checkPhotsAt ( "NexNext/"  + MCRichOpticalPhotonLocation::Default );
  }

  // MCRichDigits
  MCRichDigits * richMcDigits( 0 );
  if ( m_bdMcDigits ) 
  {
    if ( !exist<MCRichDigits>(MCRichDigitLocation::Default) ) 
    {
      Warning("Cannot locate MCRichDigits at "+MCRichDigitLocation::Default);
    } 
    else 
    {
      richMcDigits = get<MCRichDigits>(MCRichDigitLocation::Default);
      debug() << "Successfully located " << richMcDigits->size()
              << " MCRichDigits at " << MCRichDigitLocation::Default << endreq;
      for ( MCRichDigits::const_iterator imcDigit = richMcDigits->begin();
            imcDigit != richMcDigits->end(); ++imcDigit ) 
      {
        debug() << "MCRichDigit " << (*imcDigit)->key() << endreq;
        debug() << "  MCRichHits(" << (*imcDigit)->hits().size() << ") keys= ";
        //for ( SmartRefVector<MCRichHit>::const_iterator iHit = (*imcDigit)->hits().begin();
        //      iHit != (*imcDigit)->hits().end(); ++iHit ) {
        //  debug() << (long)(*iHit)->key() << " ";
        //}
        debug() << endreq;
      }
    }
  } // end MCRichDigit print

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichDigiDataObjVerifier::finalize()
{
  debug() << "Finalize" << endreq;

  // finalize base class
  return RichAlgBase::finalize();
}
