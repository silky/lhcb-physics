
//------------------------------------------------------------------------------------
/** @file RichDigitQC.h
 *
 *  Header file for RICH Digitisation Quality Control algorithm : RichDigitQC
 *
 *  CVS Log :-
 *  $Id: RichDigitQC.h,v 1.17 2005-12-16 15:12:34 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//------------------------------------------------------------------------------------

#ifndef RICHDIGIQC_RICHDIGITQC_H
#define RICHDIGIQC_RICHDIGITQC_H 1

// STD
#include <sstream>

// Boost
#include "boost/lexical_cast.hpp"

// base class
#include "RichKernel/RichHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event model
#include "Event/MCRichDigit.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichPoissonEffFunctor.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"

// RICH Interfaces
#include "RichKernel/IRichDetNumberingTool.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// Constants
#include "Kernel/PhysicalConstants.h"

// LHCb namespace
using namespace LHCb;

/** @class RichDigitQC RichDigitQC.h RichDigiQC/RichDigitQC.h
 *
 *  Monitor for Rich digitisation and DAQ simulation
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigitQC : public RichHistoAlgBase 
{

public:

  /// Standard constructor
  RichDigitQC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigitQC( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  std::string mchitLocation( const MCRichDigit * digit ) const;

private: // data

  /// Pointer to RICH detector numbering tool
  const IRichDetNumberingTool * m_detNumTool;

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_smartIDs;

  /// Pointer to MC truth tool
  const IRichMCTruthTool * m_mcTool;

  // job options
  std::string m_digitTDS;  ///< Location of MCRichDigits in TES
  bool m_extraHists;       ///< Flag to turn on the production of additional histograms

  /// Number of events processed
  unsigned int m_evtC; 

  /// L1 occupancy counter
  typedef Rich::HashMap< const RichDAQ::Level1ID, unsigned int > L1Counter;

  /// Counter for hits in each HPD
  typedef Rich::HashMap< const RichSmartID, unsigned int > HPDCounter;
  HPDCounter m_nHPD[Rich::NRiches]; ///< Tally for HPD occupancy, in each RICH

  typedef Rich::HashMap< std::string, unsigned int > SpillCount;
  typedef std::vector< SpillCount > SpillDetCount;

  /// Number of digitised hits per RICH detector and event location
  SpillDetCount m_spillDigits;

  /// Number of digitised hits per RICH detector and event location
  SpillDetCount m_totalSpills;

  /// Number of background hits in each RICH
  std::vector< unsigned int > m_bkgHits;

  /// List of event locations to look for MCRichHits in
  typedef Rich::HashMap< std::string, bool > EventLocations;
  EventLocations m_evtLocs;

};

inline std::string RichDigitQC::mchitLocation( const MCRichDigit * digit ) const
{
  // if signal, always first hit
  return objectLocation( digit->hits().front()->parent() );
}

#endif // RICHDIGIQC_RICHDIGITQC_H
