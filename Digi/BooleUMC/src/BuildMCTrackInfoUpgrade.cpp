// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/VeloCluster.h"
#include "Event/VLCluster.h"
#include "Event/VeloPixCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h" /// Needed for path of table
#include "Event/MCOTTime.h"
#include "Event/FTCluster.h"

#include "Linker/LinkedTo.h"
#include "Event/MCProperty.h"

// Det
#include "VeloDet/DeVelo.h"
#include "VLDet/DeVL.h"
#include "VeloPixDet/DeVeloPix.h"
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"
#include "FTDet/DeFTDetector.h"

// local
#include "BuildMCTrackInfoUpgrade.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BuildMCTrackInfoUpgrade
//
// 2004-01-08 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BuildMCTrackInfoUpgrade )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BuildMCTrackInfoUpgrade::BuildMCTrackInfoUpgrade( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_velo(0)
  , m_vlDet(0)
  , m_veloPix(0)
  , m_ttDet(0)
  , m_itDet(0)
  , m_otDet(0)
{
  declareProperty( "WithVelo",  m_withVelo  = false );
  declareProperty( "WithVP",    m_withVP    = false );
  declareProperty( "WithVL",    m_withVL    = false );
  declareProperty( "WithIT",    m_withIT    = false );
  declareProperty( "WithOT",    m_withOT    = false );
  declareProperty( "WithFT",    m_withFT    = true  );
}
//=============================================================================
// Destructor
//=============================================================================
BuildMCTrackInfoUpgrade::~BuildMCTrackInfoUpgrade() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode BuildMCTrackInfoUpgrade::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  if ( !(m_withVelo||m_withVP||m_withVL) ) {
    error() << "At least one of Velo, VL or VP is needed !" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( (m_withVelo&&m_withVL) || (m_withVelo&&m_withVP) || (m_withVL&&m_withVP) ) {
    error() << "Only one of Velo, VP and VL allowed !" << endmsg;
    return StatusCode::FAILURE;
  }  

  if ( m_withVP ) m_veloPix = getDet<DeVeloPix>( DeVeloPixLocation::Default );
  if ( m_withVL   ) m_vlDet = getDet<DeVL>( DeVLLocation::Default );
  if ( m_withVelo ) m_velo  = getDet<DeVelo>( DeVeloLocation::Default );
  m_ttDet = getDet<DeSTDetector>(DeSTDetLocation::TT );  
  if ( m_withIT ) m_itDet = getDet<DeSTDetector>(DeSTDetLocation::IT );
  if ( m_withOT ) m_otDet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  if ( m_withFT ) m_ftDet = getDet<DeFTDetector>(DeFTDetectorLocation::Default );

  if(msgLevel(MSG::DEBUG)) {
    debug() << "Number of TT layers " << m_ttDet->layers().size() << endmsg;
    if ( m_withIT ) debug() << "Number of IT layers " << m_itDet->layers().size() << endmsg;
    if ( m_withOT ) debug() << "Number of OT layers " << m_otDet->layers().size() << endmsg;
    if ( m_withFT ) debug() << "Number of FT layers " << m_ftDet->layers().size() << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BuildMCTrackInfoUpgrade::execute() {
  const bool isDebug   = msgLevel(MSG::DEBUG);
  const bool isVerbose = msgLevel(MSG::VERBOSE);

  if(isDebug) debug() << "==> Execute" << endmsg;  
  
  LHCb::MCParticles* mcParts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);

  LHCb::MCProperty* trackInfo = new LHCb::MCProperty();
  put( trackInfo, LHCb::MCPropertyLocation::TrackInfo );  

  //== The array size is bigger than MCParticle.size() as the MCParticles are
  //== compressed, uninteresting particles were removed at the end of Brunel.
  int highestKey = mcParts->size();
  for ( LHCb::MCParticles::const_iterator itP = mcParts->begin();
        mcParts->end() != itP; itP++ ) {
    if ( highestKey < (*itP)->key() ) highestKey = (*itP)->key();
  }
  unsigned int sizePart = highestKey+1;

  if(isDebug) debug() << "Highest MCParticle number " << highestKey << endmsg;

  std::vector<int> lastVelo ( sizePart, -1 );
  std::vector<int> veloR    ( sizePart, 0 );
  std::vector<int> veloPhi  ( sizePart, 0 );
  std::vector<int> veloPix  ( sizePart, 0 );
  std::vector<int> station  ( sizePart, 0 );
  
  LHCb::MCParticle* part;
  unsigned int MCNum;
  
  if ( m_withVelo ) {    //== particle-> VeloDigit links
    LHCb::VeloClusters* veloClus = get<LHCb::VeloClusters>( LHCb::VeloClusterLocation::Default);
    LinkedTo<LHCb::MCParticle, LHCb::VeloCluster> veloLink( eventSvc(), msgSvc(), 
                                                            LHCb::VeloClusterLocation::Default );
    if( veloLink.notFound() ) return StatusCode::FAILURE;
  
    for ( LHCb::VeloClusters::const_iterator vIt = veloClus->begin() ;
        veloClus->end() != vIt ; vIt++ ) {
      int sensor = (*vIt)->channelID().sensor();
      const DeVeloSensor* sens=m_velo->sensor(sensor);
      part = veloLink.first( *vIt );
      while ( NULL != part ) {
        if ( mcParts == part->parent() ) {  //== PArticle from the current spill. key is ALWAYS <= highestKey !
          MCNum = part->key();
          if ( sensor != lastVelo[MCNum] ) {  // Count only once per sensor a given MCParticle
            lastVelo[MCNum] = sensor;
            if ( sens->isR() ) {
              veloR[MCNum]++;
              if(isVerbose)
                verbose() << "MC " << MCNum << " Velo R sensor " << sensor << " nbR " << veloR[MCNum] << endmsg;
            } else if ( sens->isPhi() ) {
              veloPhi[MCNum]++;
              if(isVerbose)
                verbose() << "MC " << MCNum << " Velo Phi sensor " << sensor << " nbPhi " << veloPhi[MCNum] << endmsg;
            }
          }
        }
        part = veloLink.next() ;
      }
    }
  } else if ( m_withVL ) {    //== particle-> VLDigit links
    LHCb::VLClusters* vlClus = get<LHCb::VLClusters>( LHCb::VLClusterLocation::Default);
    LinkedTo<LHCb::MCParticle, LHCb::VLCluster> vlLink( eventSvc(), msgSvc(), 
                                                        LHCb::VLClusterLocation::Default );
    if( vlLink.notFound() ) return StatusCode::FAILURE;
  
    for ( LHCb::VLClusters::const_iterator vIt = vlClus->begin() ;
        vlClus->end() != vIt ; vIt++ ) {
      int sensor = (*vIt)->channelID().sensor();
      const DeVLSensor* sens = m_vlDet->sensor(sensor);
      part = vlLink.first( *vIt );
      while ( NULL != part ) {
        if ( mcParts == part->parent() ) {  //== PArticle from the current spill. key is ALWAYS <= highestKey !
          MCNum = part->key();
          if ( sensor != lastVelo[MCNum] ) {  // Count only once per sensor a given MCParticle
            lastVelo[MCNum] = sensor;
            if ( sens->isR() ) {
              veloR[MCNum]++;
              if(isVerbose)
                verbose() << "MC " << MCNum << " VL R sensor " << sensor << " nbR " << veloR[MCNum] << endmsg;
            } else if ( sens->isPhi() ) {
              veloPhi[MCNum]++;
              if(isVerbose)
                verbose() << "MC " << MCNum << " VL Phi sensor " << sensor << " nbPhi " << veloPhi[MCNum] << endmsg;
            }
          }
        }
        part = vlLink.next() ;
      }
    }
  } else if ( m_withVP ) {
    LHCb::VeloPixClusters* veloPixClus = get<LHCb::VeloPixClusters>( LHCb::VeloPixClusterLocation::VeloPixClusterLocation);
    LinkedTo<LHCb::MCParticle, LHCb::VeloPixCluster> veloPixLink( eventSvc(), msgSvc(),
                                                                  LHCb::VeloPixClusterLocation::VeloPixClusterLocation );
    if( veloPixLink.notFound() ) return StatusCode::FAILURE;
  
    std::sort( veloPixClus->begin(), veloPixClus->end(), increasingSensor() );  //== Sorting not done in decoding!
    for ( LHCb::VeloPixClusters::const_iterator vIt = veloPixClus->begin() ;
          veloPixClus->end() != vIt ; vIt++ ) {
      int sensor = (*vIt)->channelID().sensor();
      part = veloPixLink.first( *vIt );
      while ( NULL != part ) {
        if ( mcParts == part->parent() ) {
          MCNum = part->key();
          if ( sensor != lastVelo[MCNum] ) {  // Count only once per sensor a given MCParticle
            lastVelo[MCNum] = sensor;
            veloPix[MCNum]++;
            if(isVerbose)  
              verbose() << "MC " << MCNum << " VeloPix sensor " << sensor << " nbVeloPix " << veloPix[MCNum] << endmsg;
            }
          }
        }
      part = veloPixLink.next() ;
    }
  }
  
  //== TT cluster -> particle associaton
  LHCb::STClusters* TTDig = get<LHCb::STClusters>( LHCb::STClusterLocation::TTClusters);
  LinkedTo<LHCb::MCParticle, LHCb::STCluster> ttLink( eventSvc(), msgSvc(), 
                                     LHCb::STClusterLocation::TTClusters );
  if( ttLink.notFound() ) return StatusCode::FAILURE;
  
  for ( LHCb::STClusters::const_iterator tIt = TTDig->begin() ;
        TTDig->end() != tIt ; tIt++ ) {
    int sta   = (*tIt)->channelID().station() - 1;  // 0-1 from 1-2
    int lay   = ((*tIt)->channelID().layer() - 1)%4;
    bool isX  = (0==lay) || (3==lay);
    part = ttLink.first( *tIt );
    while ( NULL != part ) {
      if ( mcParts == part->parent() ) {
        MCNum = part->key();
        updateBit( station[MCNum], sta, isX );
        if(isVerbose) verbose() << "MC " << MCNum << " TT Sta " << sta << " lay " << lay << endmsg;
      }
      part = ttLink.next() ;
    }
  }
  
  if ( m_withIT ) {    //== IT cluster -> particle associaton
    LHCb::STClusters* ITDig = get<LHCb::STClusters>( LHCb::STClusterLocation::ITClusters);
    LinkedTo<LHCb::MCParticle, LHCb::STCluster> itLink( eventSvc(), msgSvc(), 
                                                        LHCb::STClusterLocation::ITClusters );
    if( itLink.notFound() ) return StatusCode::FAILURE;
    
    for ( LHCb::STClusters::const_iterator iIt = ITDig->begin() ;
          ITDig->end() != iIt ; iIt++ ) {
      int sta   = (*iIt)->channelID().station() + 1; // 2-4 from 1-3
      int lay   = ((*iIt)->channelID().layer() - 1)%4;
      bool isX  = (0==lay) || (3==lay);
      part = itLink.first( *iIt );
      while ( NULL != part ) {
        if ( mcParts == part->parent() ) {
          MCNum = part->key();
          updateBit( station[MCNum], sta, isX );
          if(isVerbose) verbose() << "MC " << MCNum << " IT Sta " << sta << " lay " << lay << endmsg;
        }
        part = itLink.next() ;
      }
    }
  }
  
  if ( m_withOT ) {  //== OT cluster -> particle association
    const LHCb::MCOTTimes* OTTim = get<LHCb::MCOTTimes>(LHCb::MCOTTimeLocation::Default);
    LinkedTo<LHCb::MCParticle> otLink( eventSvc(), msgSvc(), LHCb::OTTimeLocation::Default );
    if( otLink.notFound() ) return StatusCode::FAILURE;
    
    
    for ( LHCb::MCOTTimes::const_iterator timIt = OTTim->begin() ;
          OTTim->end() != timIt ; timIt++ ) {
      // OT stations should be 2-4 in this numbering, 0,1 are TT stations
      int sta   = (*timIt)->channel().station() + 1;
      int lay   = (*timIt)->channel().layer() ;     
      bool isX  = (0==lay) || (3==lay);
      part = otLink.first( (*timIt)->channel() );
      while ( NULL != part ) {
        if ( mcParts == part->parent() ) {
          MCNum = part->key();
          updateBit( station[MCNum], sta, isX );
          if(isVerbose) verbose() << "MC " << MCNum << " OT Sta " << sta << " lay " << lay << endmsg;
        }
        part = otLink.next() ;
      }
    }
  }

  if ( m_withFT ) {  //=== Fibre Tracker
    const LHCb::FTClusters* ftClus = get<LHCb::FTClusters>(LHCb::FTClusterLocation::Default);
    LinkedTo<LHCb::MCParticle> ftLink( eventSvc(), msgSvc(), LHCb::FTClusterLocation::Default );
    if( ftLink.notFound() ) return StatusCode::FAILURE;
    
    
    for ( LHCb::FTClusters::const_iterator cIt = ftClus->begin(); ftClus->end() != cIt ; cIt++ ) {
      // FT has just layers, from 0 to 11. Create station = layer/4 + 2 to be in the range 2-4
      int sta   = (*cIt)->channelID().layer()/4 + 2;
      int lay   = (*cIt)->channelID().layer() %4;     
      bool isX  = (0==lay) || (3==lay);
      part = ftLink.first( (*cIt)->channelID() );
      while ( NULL != part ) {
        if ( mcParts == part->parent() ) {
          MCNum = part->key();
          updateBit( station[MCNum], sta, isX );
          if(isVerbose) verbose() << "MC " << MCNum << " FT Sta " << sta << " lay " << lay << endmsg;
        }
        part = ftLink.next() ;
      }
    }
  }  

  //=====================================================================
  // Now compute the acceptance, with MCHits
  //=====================================================================
  computeAcceptance( station );

  //== Build now the trackInfo tracks

  for ( LHCb::MCParticles::const_iterator itP = mcParts->begin();
        mcParts->end() != itP; itP++ ) {
    MCNum = (*itP)->key();    
    int mask = station[MCNum];    
    if ( m_withVP ) {
      if (  2 < veloPix[MCNum] ) mask |= MCTrackInfo::maskHasVelo;
      if ( 15 < veloPix[MCNum] ) veloPix[MCNum]   = 15;
      mask |= (veloPix[MCNum]  <<MCTrackInfo::multVeloR );
    } else {
      if (  2 < veloR[MCNum]   ) mask |= MCTrackInfo::maskVeloR;
      if (  2 < veloPhi[MCNum] ) mask |= MCTrackInfo::maskVeloPhi;
      if ( 15 < veloR[MCNum]   ) veloR[MCNum]   = 15;
      if ( 15 < veloPhi[MCNum] ) veloPhi[MCNum] = 15;
      mask |= (veloR[MCNum]  <<MCTrackInfo::multVeloR );
      mask |= (veloPhi[MCNum]<<MCTrackInfo::multVeloPhi );
    }

    if ( 0 != mask ) {
      trackInfo->setProperty( *itP, mask );
      if(isDebug) {
        debug() << format( "Track %4d mask %8x nR %2d nPhi %2d ", 
                           MCNum, mask, veloR[MCNum], veloPhi[MCNum] );
        if ( MCTrackInfo::maskHasVelo == (mask & MCTrackInfo::maskHasVelo ) ) debug() << " hasVelo ";
        if ( MCTrackInfo::maskHasTT   == (mask & MCTrackInfo::maskHasTT   ) ) debug() << " hasTT ";
        if ( MCTrackInfo::maskHasT    == (mask & MCTrackInfo::maskHasT    ) ) debug() << " hasT ";
        debug() << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Process the MC(Velo)Hits to get the 'acceptance'
//=========================================================================
void BuildMCTrackInfoUpgrade::computeAcceptance ( std::vector<int>& station ) {

  const bool isDebug = msgLevel(MSG::DEBUG);
  if ( m_withVP ) {
    std::vector<int> nVeloPix( station.size(), 0 );
    LHCb::MCHits* veloHits = get<LHCb::MCHits>( LHCb::MCHitLocation::VeloPix);
    for ( LHCb::MCHits::const_iterator vHit = veloHits->begin();
          veloHits->end() != vHit; vHit++ ) {
      unsigned int MCNum = (*vHit)->mcParticle()->key();
      if ( station.size() <= MCNum ) continue;
      nVeloPix[MCNum]++;
    }
    for ( unsigned int MCNum = 0; station.size() > MCNum; MCNum++ ){
      if ( 2 < nVeloPix[MCNum] ) station[MCNum] |= MCTrackInfo::maskAccVelo;
    }
  } else if ( m_withVelo ) {
    std::vector<int> nVeloR( station.size(), 0 );
    std::vector<int> nVeloP( station.size(), 0 );
    
    LHCb::MCHits* veloHits = get<LHCb::MCHits>( LHCb::MCHitLocation::Velo);
    for ( LHCb::MCHits::const_iterator vHit = veloHits->begin();
          veloHits->end() != vHit; vHit++ ) {
      unsigned int MCNum = (*vHit)->mcParticle()->key();
      if ( station.size() <= MCNum ) continue;
      int staNr = (*vHit)->sensDetID();
      const DeVeloSensor* sens=m_velo->sensor(staNr);   
      if ( sens->isR() ) {
        nVeloR[MCNum]++;
      } else {
        nVeloP[MCNum]++;
      }
    }
  } else {
    std::vector<int> nVeloR( station.size(), 0 );
    std::vector<int> nVeloP( station.size(), 0 );
    
    LHCb::MCHits* vlHits = get<LHCb::MCHits>( LHCb::MCHitLocation::VL );
    for ( LHCb::MCHits::const_iterator vHit = vlHits->begin();
          vlHits->end() != vHit; vHit++ ) {
      unsigned int MCNum = (*vHit)->mcParticle()->key();
      if ( station.size() <= MCNum ) continue;
      int staNr = (*vHit)->sensDetID();
      const DeVLSensor* sens = m_vlDet->sensor(staNr);   
      if ( sens->isR() ) {
        nVeloR[MCNum]++;
      } else {
        nVeloP[MCNum]++;
      }
    }
    for ( unsigned int MCNum = 0; station.size() > MCNum; MCNum++ ){
      if ( 2 < nVeloR[MCNum] ) station[MCNum] |= MCTrackInfo::maskAccVeloR;
      if ( 2 < nVeloP[MCNum] ) station[MCNum] |= MCTrackInfo::maskAccVeloPhi;
    }
  }
  
  //== TT
  
  LHCb::MCHits* ttHits = get<LHCb::MCHits>( LHCb::MCHitLocation::TT );
  for ( LHCb::MCHits::const_iterator tHit = ttHits->begin();
        ttHits->end() != tHit; tHit++ ) {
    unsigned int MCNum = (*tHit)->mcParticle()->key();
    if ( station.size() <= MCNum ) continue;
    
    Gaudi::XYZPoint midPoint = (*tHit)->midPoint();
    DeSTLayer* ttLay = m_ttDet->findLayer( midPoint );
    if ( 0 == ttLay ) {
      if(isDebug) debug() << format( "TT Hit not in any LAYER ! x %8.2f y%8.2f z%9.2f",
                                     midPoint.x(), midPoint.y(), midPoint.z() ) << endmsg;
      continue;
    }
    int  sta = ttLay->elementID().station() -1;
    bool isX = ttLay->angle() == 0;
    updateAccBit( station[MCNum], sta, isX );
  }

  if ( m_withIT ) {       //== IT
    LHCb::MCHits* itHits = get<LHCb::MCHits>( LHCb::MCHitLocation::IT );
    for ( LHCb::MCHits::const_iterator iHit = itHits->begin();
          itHits->end() != iHit; iHit++ ) {
      unsigned int MCNum = (*iHit)->mcParticle()->key();
      if ( station.size() <= MCNum ) continue;
      Gaudi::XYZPoint midPoint = (*iHit)->midPoint();
      DeSTLayer* itLay = m_itDet->findLayer( midPoint );
      if ( 0 == itLay ) {
        if(isDebug) debug() << format( "IT Hit not in any LAYER ! x %8.2f y%8.2f z%9.2f",
                                       midPoint.x(), midPoint.y(), midPoint.z() ) << endmsg;
        continue;
      }
      int  sta = itLay->elementID().station() +1;  // want 2,3,4
      bool isX = itLay->angle() == 0;
      updateAccBit( station[MCNum], sta, isX );    
    }
  }
  
  if ( m_withOT ) {  //== OT
    LHCb::MCHits* otHits = get<LHCb::MCHits>( LHCb::MCHitLocation::OT );
    for ( LHCb::MCHits::const_iterator oHit = otHits->begin();
          otHits->end() != oHit; oHit++ ) {
      unsigned int MCNum = (*oHit)->mcParticle()->key();
      if ( station.size() <= MCNum ) continue;
      Gaudi::XYZPoint midPoint = (*oHit)->midPoint();
      DeOTLayer* otLay = m_otDet->findLayer( midPoint );
      if ( 0 == otLay ) {
        if(isDebug) debug() << format( "OT Hit not in any LAYER ! x %8.2f y%8.2f z%9.2f",
                                       midPoint.x(), midPoint.y(), midPoint.z() ) << endmsg;
        continue;
      }
      int sta = otLay->elementID().station() +1;
      bool isX = otLay->angle() == 0;
      updateAccBit( station[MCNum], sta, isX );
    }
  }
}
//=============================================================================
