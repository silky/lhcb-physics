#ifndef RICHREADOUT_RICHSIGNAL_H
#define RICHREADOUT_RICHSIGNAL_H 1

// base class
#include "RichUtils/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/MCParticle.h"
#include "Kernel/ParticleID.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// from RichDetTools
#include "RichDetTools/IRichSmartIDTool.h"

#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"

class RichSignal : public RichAlgBase {

public:

  RichSignal ( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RichSignal();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private: // methods

  StatusCode ProcessEvent( const std::string & hitLoc, 
                           const double tofOffset ) const;

private: // data

  MCRichSummedDeposits* mcSummedDeposits;
  MCRichDeposits* mcDeposits;

  /// String containing MCRichDeposits location in TDS
  std::string m_RichHitLocation;
  std::string m_RichPrevLocation;
  std::string m_RichPrevPrevLocation;
  std::string m_RichNextLocation;
  std::string m_RichNextNextLocation;
  std::string m_RichSummedDepositLocation;
  std::string m_RichDepositLocation;
  std::string m_lhcBkgLocation;

  /// Flag to turn on the use of the spillover events
  bool m_doSpillover;

  /// Flag to turn on the use of the LHC backgrounde events
  bool m_doLHCBkg;

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDTool;

  mutable Rndm::Numbers m_rndm;

};

#endif // RICHREADOUT_RICHSIGNAL_H
