#ifndef BUILDMCTRACKINFO_H 
#define BUILDMCTRACKINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/MCTrackInfo.h"

class DeVelo;
class DeVP;
class DeSTDetector;
class DeOTDetector;
class DeFTDetector;

/** @class BuildMCTrackInfo BuildMCTrackInfo.h
 *  Build the Reconstructable MCProperty table.
 *
 *  @author Olivier Callot
 *  @date   2012-04-02 : Updated version for upgrade: IT, OT and FT optional, Pixel/normal Velo
 */
class BuildMCTrackInfo : public GaudiAlgorithm {
public: 
  /// Standard constructor
  BuildMCTrackInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BuildMCTrackInfo( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  void updateBit( int& result, int sta, bool isX ) {
    if ( 0 == sta ) {
      result |= MCTrackInfo::maskTT1;
    } else if ( 1 == sta ) {
      result |= MCTrackInfo::maskTT2;
    } else if ( 2 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskT1X;
      else       result |= MCTrackInfo::maskT1S;
    } else if ( 3 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskT2X;
      else       result |= MCTrackInfo::maskT2S;
    } else if ( 4 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskT3X;
      else       result |= MCTrackInfo::maskT3S;
    }
  }
  

  void updateAccBit( int& result, int sta, bool isX ) {
    if ( 0 == sta ) {
      result |= MCTrackInfo::maskAccTT1;
    } else if ( 1 == sta ) {
      result |= MCTrackInfo::maskAccTT2;
    } else if ( 2 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskAccT1X;
      else       result |= MCTrackInfo::maskAccT1S;
    } else if ( 3 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskAccT2X;
      else       result |= MCTrackInfo::maskAccT2S;
    } else if ( 4 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskAccT3X;
      else       result |= MCTrackInfo::maskAccT3S;
    }
  }
  
  void computeAcceptance ( std::vector<int>& station ) ;

private:
  bool m_withVelo;
  bool m_withVP;
  bool m_withUT;
  bool m_withIT;
  bool m_withOT;
  bool m_withFT;

  DeVelo*       m_velo;
  DeVP*         m_vpDet;

  DeSTDetector* m_ttDet;
  DeSTDetector* m_itDet;
  DeOTDetector* m_otDet;
  DeFTDetector* m_ftDet;
  std::string   m_ttClustersName;
  std::string   m_ttHitsName;
};
#endif // BUILDMCTRACKINFO_H
