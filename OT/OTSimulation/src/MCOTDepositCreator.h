// $Id: MCOTDepositCreator.h,v 1.2 2004-11-10 13:05:14 jnardull Exp $
#ifndef OTSIMULATION_MCOTDEPOSITCREATOR_H
#define OTSIMULATION_MCOTDEPOSITCREATOR_H 1

// Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiAlg/GaudiAlgorithm.h"


// Event
#include "Event/MCOTDeposit.h"

// OTDet
#include "OTDet/DeOTDetector.h" 

// forward declarations
class IOTEffCalculator;
class IOTSmearer;
class IOTrtRelation;
class IOTRandomDepositCreator;

/** @class MCOTDepositCreator MCOTDepositCreator.h \
 *         "OTSimulation/MCOTDepositCreator.h"
 *
 *  Top level algorithm that makes MCOTDeposits from MCHits.
 *   @li find hits and calculate distance to closest wire
 *   @li apply single cell efficiency (function of distance).
 *   @li smear the distance with a Gaussian
 *   @li apply r-t relation
 *   @li add propagation time along wire
 *   @li add cross talk
 *   @li add random noise
 *
 *  @author M.Needham
 *  @author adapted to the Event Model by: J. van Tilburg 
 *          jtilburg@nikhef.nl (03-04-2002)
 *  @date   21/10/2000
 */

typedef std::vector<MCOTDeposit*> MCOTDepositVector;

class MCOTDepositCreator : public GaudiAlgorithm {

public:
 
  /// constructor
  MCOTDepositCreator(const std::string& name, 
             ISvcLocator *svcloc );

  /// destructor
  virtual ~MCOTDepositCreator();

  /// intialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();

private:

  /// make initial list of digitizations
  StatusCode makeDigitizations();

  /// apply single cell effiency
  StatusCode singleCellEff();

  /// apply resolution smearing
  StatusCode applySmear();

  /// apply r-t relation
  StatusCode applyRTrelation();

  /// apply propagation along wire
  StatusCode applyPropagationTime();

  /// apply cross talk
  StatusCode addCrossTalk();

  /// apply random noise
  StatusCode addNoise();

  /// make unique name
  std::string toolName(const std::string& aName, 
                       const int id) const;

  /// pointer to XML geometry:
  DeOTDetector* m_tracker;

  /// pointers to Tools
  std::vector<IOTEffCalculator*> m_singleCellEffVector;
  std::vector<IOTSmearer*> m_smearerVector;
  std::vector<IOTrtRelation*> m_rtRelationVector;

  std::string m_noiseToolName;
  IOTRandomDepositCreator* m_noiseTool;
 
  /// temporary deposits vector
  MCOTDepositVector* m_tempDeposits;

  unsigned int m_numStations;              ///< number of stations
  unsigned int m_firstOTStation;           ///< first OT station
  std::vector<std::string> m_spillVector;  ///< spills to digitize  
  std::vector<double>  m_spillTimes;       ///< spillTimes
    std::vector<std::string> m_spillNames; ///< spillNames

  bool m_addCrossTalk;                     ///< flag to add Xtalk
  double m_crossTalkLevel;                 ///< level of crosstalk
  bool m_addNoise;                         ///< flag to add random noise
  double m_noiseLevel;                     ///< level of random noise
  Rndm::Numbers m_flatDist;                ///< flat dist for crosstalk

  std::vector<double> m_startReadOutGate;  ///< start of readout gate
  double m_sizeOfReadOutGate;              ///< end of readout gate   

};

#endif // OTSIMULATION_MCOTDEPOSITCREATOR_H
