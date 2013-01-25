/** @class MCFTDigitCreator MCFTDigitCreator.h
 *  
 *  From the list of MCFTDeposit (deposited energy in each FTChannel), 
 *  this algorithm converts the deposited energy in ADC charge according
 *  to the mean number of photoelectron per MIP 
 *  + the mean ADC count per photoelectron.
 *  Created digits are put in transient data store.
 *
 *  TO DO :
 *  - add noise
 *
 *  THINK ABOUT :
 *  - dead channels
 *
 *  @author COGNERAS Eric
 *  @date   2012-04-04
 */

#ifndef MCFTDIGITCREATOR_H 
#define MCFTDIGITCREATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/RndmGenerators.h"

// LHCbKernel
#include "Kernel/FTChannelID.h"


class MCFTDigitCreator : public GaudiHistoAlg {
public: 
  /// Standard constructor
  MCFTDigitCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCFTDigitCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  int deposit2ADC(const LHCb::MCFTDeposit* ftdeposit); ///< This function converts deposited energy in ADC Count
  
protected:

private:

  std::string m_inputLocation;///< FT energy deposit Location
  std::string m_outputLocation;///< FT digit Location

  double m_photoElectronsPerMeV; ///< mean number of photoelectrons produced per deposited MeV
  double m_sipmGain; ///< SiPM gain
  double m_adcNoise; ///< Sigma of the noise in the ADC.

  Rndm::Numbers m_gauss;
  Rndm::Numbers m_flat;
};
#endif // MCFTDIGITCREATOR_H
