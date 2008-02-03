// $Id: MuonHitChecker.h,v 1.5 2008-02-03 14:58:31 asatta Exp $
#ifndef MuonHitChecker_H 
#define MuonHitChecker_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// for Muons
#include "Event/MCHit.h"   

/** @class MuonHitChecker MuonHitChecker.h
 *  
 *
 *  @author A Sarti
 *  @date   2005-05-20
 */
class MuonHitChecker : public GaudiTupleAlg {
public:
  /// Standard constructor
  MuonHitChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonHitChecker( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::vector<int> m_numberOfGaps;

private:

  int nhit[5][4],cnt[5][4];
  int nhit_ri[5],cnt_ri[5];

  bool m_detailedMonitor;
  int m_hit_outside_gaps;
  
};
#endif // MuonHitChecker_H
