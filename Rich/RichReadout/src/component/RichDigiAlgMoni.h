// $Id: RichDigiAlgMoni.h,v 1.4 2005-12-16 15:13:33 jonrob Exp $
#ifndef RICHMONITOR_RICHDIGIALGMONI_H
#define RICHMONITOR_RICHDIGIALGMONI_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Event model
#include "Event/MCRichDigit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichHit.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichRadiatorType.h"
#include "RichKernel/RichParticleIDType.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// GSL
#include "gsl/gsl_math.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// temporary histogramming numbers
#include "RichDetParams.h"

// namespaces
using namespace LHCb; ///< General LHCb namespace

/** @class RichDigiAlgMoni RichDigiAlgMoni.h RichDigiQC/RichDigiAlgMoni.h
 *
 *  Monitor for Rich Digitisation algorithm performance
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigiAlgMoni : public RichAlgBase 
{

public:

  /// Standard constructor
  RichDigiAlgMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigiAlgMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  // Map to count cherenkov photons for each radiator
  typedef std::pair<const MCParticle*,Rich::RadiatorType> PhotPair;
  typedef RichMap< PhotPair, int >                        PhotMap;

  // PD occupancies
  typedef RichMap<RichSmartID,int> PDMulti;

  /// Book histograms
  StatusCode bookHistograms();

  /// Particle mass
  double mass( const MCParticle * mcPart );

  /// Returns beta for a given MCParticle
  double mcBeta( const MCParticle * mcPart );

  /// Returns the momentum for a given MCParticle
  double momentum( const MCParticle * mcPart );

  /// Count the number of photo electrons
  void countNPE( PhotMap & photMap,
                 const MCRichHit * hit );

  /// IS this a true cherenkov signal hit ?
  bool trueCKHit(  const MCRichHit * hit );

private: // data

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_smartIDTool;

  /// Pointer to MC truth tool
  const IRichMCTruthTool * m_mcTool;

  // job options
  std::string m_histPth;        ///< Output histogram path
  std::string m_mcdigitTES;     ///< Location of MCRichDigits in TES
  std::string m_mcdepTES;       ///< Location of MCRichDeposits in TES
  std::string m_mchitTES;       ///< Location of MCRichHits in TES

  // Particle masses
  RichMap<Rich::ParticleIDType,double> m_particleMass;

  // Histograms
  IHistogram1D* m_digitMult[Rich::NRiches]; ///< MCRichDigit event multiplicity
  IHistogram1D* m_hitMult[Rich::NRiches]; ///< MCRichHit event multiplicity

  IHistogram1D* m_tofDep[Rich::NRiches];   ///< TOF information for MCRichDeposit
  IHistogram1D* m_depEnDep[Rich::NRiches]; ///< Deposit energy for MCRichDeposit

  IHistogram1D* m_tofHit[Rich::NRiches];   ///< TOF information for signal MCRichHit
  IHistogram1D* m_depEnHit[Rich::NRiches]; ///< Deposit energy for signal MCRichHit

  IHistogram1D* m_tofHitB[Rich::NRiches];   ///< TOF information for background MCRichHit
  IHistogram1D* m_depEnHitB[Rich::NRiches]; ///< Deposit energy for background MCRichHit

  IHistogram1D* m_pdDigsXGlobal[Rich::NRiches];     ///< Observed PD digits x global
  IHistogram1D* m_pdDigsYGlobal[Rich::NRiches];     ///< Observed PD digits y global
  IHistogram1D* m_pdDigsZGlobal[Rich::NRiches];     ///< Observed PD digits z global
  IHistogram2D* m_pdDigsXYGlobal[Rich::NRiches];    ///< Observed PD digits xVy global
  IHistogram2D* m_pdDigsXZGlobal[Rich::NRiches];    ///< Observed PD digits xVz global
  IHistogram2D* m_pdDigsYZGlobal[Rich::NRiches];    ///< Observed PD digits yVz global
  IHistogram2D* m_pdCloseUpXZ[Rich::NRiches];
  IHistogram2D* m_pdCloseUpYZ[Rich::NRiches]; 

  IHistogram1D* m_mcHitsXGlobal[Rich::NRiches];     ///< Observed hits x global
  IHistogram1D* m_mcHitsYGlobal[Rich::NRiches];     ///< Observed hits y global
  IHistogram1D* m_mcHitsZGlobal[Rich::NRiches];     ///< Observed hits z global
  IHistogram2D* m_mcHitsXYGlobal[Rich::NRiches];    ///< Observed hits xVy global
  IHistogram2D* m_mcHitsXZGlobal[Rich::NRiches];    ///< Observed hits xVz global
  IHistogram2D* m_mcHitsYZGlobal[Rich::NRiches];    ///< Observed hits yVz global
  IHistogram2D* m_mcCloseUpXZ[Rich::NRiches];
  IHistogram2D* m_mcCloseUpYZ[Rich::NRiches];

  IHistogram1D* m_digiErrX[Rich::NRiches]; ///< Distance in x between hit and digit positions
  IHistogram1D* m_digiErrY[Rich::NRiches]; ///< Distance in y between hit and digit positions
  IHistogram1D* m_digiErrZ[Rich::NRiches]; ///< Distance in z between hit and digit positions
  IHistogram1D* m_digiErrR[Rich::NRiches]; ///< Absolute distance between hit and digit positions

  IHistogram1D* m_hitsPerDigi[Rich::NRiches];  ///< The number of MCRichHits per Digit
  IHistogram1D* m_pdOcc[Rich::NRiches];  ///< The number of digits in each PD

  IHistogram1D* m_mchitNpes[Rich::NRadiatorTypes];   ///< Number of MCRichHit p.e.s. per radiator 
  IHistogram1D* m_mcdigitNpes[Rich::NRadiatorTypes]; ///< Number of MCRichDigit p.e.s. per radiator 
  IHistogram1D* m_mcdepNpes[Rich::NRadiatorTypes];   ///< Number of MCRichDeposit p.e.s. per radiator 

  IHistogram1D* m_npesRetained[Rich::NRadiatorTypes]; ///< The fraction of signal p.e.s retained by the digitisation

  IHistogram1D* m_chargedTkDeps[Rich::NRiches];
  IHistogram1D* m_chargedTkDigs[Rich::NRiches];

};

inline double RichDigiAlgMoni::mass( const MCParticle * mcPart )
{
  return m_particleMass[ m_mcTool->mcParticleType(mcPart) ];
}

inline double RichDigiAlgMoni::momentum( const MCParticle * mcPart )
{
  return ( mcPart ? mcPart->momentum().P() : 0 );
}

inline double RichDigiAlgMoni::mcBeta( const MCParticle * mcPart )
{
  if ( !mcPart ) return 0;
  const double pTot = momentum( mcPart );
  const double Esquare = pTot*pTot + gsl_pow_2(mass(mcPart));
  return ( Esquare > 0 ? pTot/sqrt(Esquare) : 0 );
}

inline bool RichDigiAlgMoni::trueCKHit(  const MCRichHit * hit ) 
{

  // Which radiator
  const Rich::RadiatorType rad = (Rich::RadiatorType)( hit->radiator() );

  // Is this a true hit
  return ( !hit->scatteredPhoton() &&
           !hit->chargedTrack()
           && ( rad == Rich::Aerogel ||
                rad == Rich::Rich1Gas ||
                rad == Rich::Rich2Gas ) );
}

#endif // RICHMONITOR_RICHDIGIALGMONI_H
