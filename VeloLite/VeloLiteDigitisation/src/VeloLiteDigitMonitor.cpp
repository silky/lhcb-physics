// Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/SmartRefVector.h"
// Kernel/LHCbKernel
#include "Kernel/VeloLiteChannelID.h"
// Event/MCEvent
#include "Event/MCHit.h"
// Local
#include "VeloLiteDigitMonitor.h"

using namespace Gaudi::Units;

/** @file VeloLiteDigitMonitor.cpp
 *
 *  Implementation of class : VeloLiteDigitMonitor
 *
 */

DECLARE_ALGORITHM_FACTORY(VeloLiteDigitMonitor);

//=============================================================================
/// Standard constructor, initializes variables
//=============================================================================
VeloLiteDigitMonitor::VeloLiteDigitMonitor(const std::string& name, 
                                           ISvcLocator* pSvcLocator) : 
    GaudiTupleAlg(name, pSvcLocator),
    m_det(0),
    m_nDigits(0.),
    m_nDigits2(0.),
    m_nSignal(0),
    m_nNoise(0),
    m_nOther(0),
    m_nEvents(0)
{
  declareProperty("PrintInfo", m_printInfo = false);
  declareProperty("Detailed",  m_detailed = true);
}

//=============================================================================
/// Destructor
//=============================================================================
VeloLiteDigitMonitor::~VeloLiteDigitMonitor() {

} 

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VeloLiteDigitMonitor::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  debug() << " ==> initialize()" << endmsg;
  m_det = getDet<DeVeloLite>(DeVeloLiteLocation::Default);
  setHistoTopDir("VeloLite/");
  return StatusCode::SUCCESS;

};

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VeloLiteDigitMonitor::execute() {

  debug() << " ==> execute()" << endmsg;
  ++m_nEvents;
  if (!exist<LHCb::MCVeloLiteDigits>(LHCb::MCVeloLiteDigitLocation::Default)) {
    error() << "There are no MCVeloLiteDigits in the TES!" << endmsg;
  } else {
    m_digits = get<LHCb::MCVeloLiteDigits>(LHCb::MCVeloLiteDigitLocation::Default);
    monitor();
  }
  return StatusCode::SUCCESS;

};

//=============================================================================
///  Finalize
//=============================================================================
StatusCode VeloLiteDigitMonitor::finalize() {

  debug() << " ==> finalize()" << endmsg;
  double err = 0.;
  if (0 != m_nEvents) {
    m_nDigits  /= m_nEvents;
    m_nDigits2 /= m_nEvents;
    err = sqrt((m_nDigits2 - (m_nDigits * m_nDigits)) / m_nEvents);
  }
  info() << "------------------------------------------------------" << endmsg;
  info() << "             - VeloLiteDigitMonitor -                 " << endmsg;
  info() << "------------------------------------------------------" << endmsg;
  info() << " Number of digits / event: " << m_nDigits 
         << " +/- " << err << endmsg; 
  const double all = m_nSignal + m_nNoise + m_nOther;
  if (all > 0) {
    info().precision(4);
    info() << "   digits from signal:                      " 
           << (m_nSignal / all) * 100 << "%" << endmsg;
    info() << "   digits from noise:                       " 
           << (m_nNoise / all) * 100 << "%" << endmsg;
    info() << "   digits from other, coupling + spillover: " 
           << (m_nOther / all) * 100 << "%" << endmsg;
  } else {
    info() << " ==> No MCVeloLiteDigits found! " << endmsg;
  }
  info() << "------------------------------------------------------" << endmsg;
  return GaudiAlgorithm::finalize();

}

void VeloLiteDigitMonitor::monitor() {

  debug() << " ==> monitor()" << endmsg;
  const double size = m_digits->size();
  m_nDigits += size;
  m_nDigits2 += size * size;
  plot(size, "nDigits", "Number of MCVeloLiteDigits / event", 0., 10000., 50);
  // Main loop over container
  LHCb::MCVeloLiteDigits::iterator it;
  for (it = m_digits->begin(); it != m_digits->end(); ++it) {
    LHCb::MCVeloLiteDigit* digit = (*it);
    bool signal = false, noise = false, other = false;
    if (fabs(digit->noise()) > 0. && (digit->signal() == 0.0)) {
      noise = true;
      ++m_nNoise;
    } else if (fabs(digit->signal()) > 0. && (digit->mcHits().size() != 0)) {
      signal = true;
      ++m_nSignal;
    } else if (fabs(digit->signal()) > 0. && (digit->mcHits().size() == 0)) {
      other = true;
      ++m_nOther;
    }
    // Print some info if asked
    if (m_printInfo) {
      info() << " ==> MCVeloLiteDigit: "
             << " ChannelID: " << digit->channelID()
             << ", sensor number: " << digit->sensor()
             << ", strip number: " << digit->strip()
             << "\n \t\t" << " Added noise: " << digit->noise()
             << ", added CM noise: " << digit->cmnoise()
             << ", added pedestal: " << digit->pedestal()
             << "\n \t\t" << " Added signal: " << digit->signal()
             << ", charge: " << digit->charge()
             << endmsg;
    }
    plot(digit->charge(), "charge", 
         "Charge in Si strip (electrons)",
         -10000., 50000., 100);
    plot(digit->signal(), "signal", 
         "Signal deposited in Si strip (electrons)",
         0., 50000., 100);
    plot(digit->noise(), "noise", 
         "Noise added in Si strip (electrons)",
         -10000., 10000, 100);
    plot(digit->cmnoise(), "cmnoise",
         "Common-Mode noise added in Si strip (electrons)",
         -20000., 20000., 100);
    plot(digit->pedestal(), "pedestal",
         "Pedestal added in Si strip (electrons)",
         0., 50000., 100);
    plot(digit->adc(), "adc",
         "ADC counts", 0., 256., 256);
    plot2D(digit->sensor(), digit->strip(), "sensorAndStrip",
           "Sensor and strip number",
           0., 100., 0., 3000., 100, 50);
     
    if (signal) {
      plot(digit->charge(), "chargeSignal", 
           "Signal dominated - Charge in Si (electrons)",
           -10000., 50000., 100);
      plot(digit->signal(), "signalSignal", 
           "Signal dominated - Signal deposited in Si (electrons)",
           0., 50000., 100);
      plot(digit->noise(), "noiseSignal",
           "Signal dominated - Noise added in Si strip (electrons)",
           -10000., -10000., 100);
      plot2D(digit->sensor(), digit->strip(), "sensorAndStripSignal",
             "Signal dominated - sensor and strip number",
             0., 100., 0., 3000., 100, 50);
    }
    if (noise) {
      plot(digit->charge(), "chargeNoise", 
           "Noise dominated - Charge in Si strip (electrons)",
           -10000., 50000., 100);
      plot(digit->signal(), "signalNoise",
           "Noise dominated - Signal deposited in Si strip (electrons)",
           0., 50000., 100);
      plot(digit->noise(), "noiseNoise",
           "Noise dominated - Noise added in Si strip (electrons)",
           -10000., 10000., 100);
      plot2D(digit->sensor(), digit->strip(), "sensorAndStripNoise",
             "Noise dominated - sensor and strip number",
             0., 100., 0., 3000., 100, 50);
    }
    if (other) {
      plot(digit->charge(), "chargeOther",
           "Other dominated - Charge in Si strip (electrons)",
           -10000., 50000., 100);
      plot(digit->signal(), "signalOther",
           "Other dominated - Signal deposited in Si strip (electrons)",
           0., 50000., 100);
      plot(digit->noise(), "noiseOther",
           "Other dominated - Noise added in Si strip (electrons)",
           -10000., 10000., 100);
      plot2D(digit->sensor(), digit->strip(), "sensorAndStripOther",
             "Other dominated - sensor and strip number",
             0., 100., 0., 3000., 100, 50);
    }
    // Get the MCHits from which MCVeloLiteDigit was made, store energy
    SmartRefVector<LHCb::MCHit> mcHits = digit->mcHits();
    std::vector<double> mcHitsCharge = digit->mcHitsCharge();

    if (m_printInfo) { 
      info() << "Test MCVeloLiteDigit: " << mcHits.size() << " MC hits" << endmsg;
    }
    plot(mcHits.size(), "hitsPerDigit",
         "Number of MC hits / MCVeloLiteDigit",
         -0.5, 5.5, 6);
    double totalSignal = 0.;
    std::vector<double>::iterator itch = mcHitsCharge.begin();
    SmartRefVector<LHCb::MCHit>::iterator ith;
    for (ith = mcHits.begin(); ith != mcHits.end(); ++ith) {
      if (m_printInfo) {
        info() << "Test MCVeloLiteDigit: MC hit, "
               << "charge in current strip (electrons)" << (*itch)
               << ", energy (eV)" << (*ith)->energy() / eV << endmsg;
      }
      totalSignal += (*itch);
      plot((*itch), "mcHitSignal",
           "MC hit signal deposited in MCVeloLiteDigit",
           0., 50000., 100);
      ++itch;
    }
    plot(totalSignal, "mcTotal",
         "MC total signal deposited in MCVeloLiteDigit",
         0., 50000., 100);
    if (signal) {
      plot(totalSignal, "mcTotalSignal",
           "Signal dominated - MC total signal deposited in MCVeloLiteDigit",
           0., 50000., 100);
    }
    if (noise) {
      plot(totalSignal, "mcTotalNoise",
           "Noise dominated - MC total signal deposited in MCVeloLiteDigit",
           0., 50000., 100);
    }
    if (other) {
      plot(totalSignal, "mcTotalOther",
           "Other dominated - MC total signal deposited in MCVeloLiteDigit",
           0., 50000., 100);
    }
    // Some other details to check
    if (m_detailed) {
      LHCb::VeloLiteChannelID channel = digit->channelID();
      const DeVeloLiteSensor* sens = m_det->sensor(digit->channelID().sensor());
      double sensorZ = sens->z() / cm;
      if (m_printInfo) {
        info() << "Channel: " << channel << ", sensor (from channel): "
               << channel.sensor() << ", sensor (from MCVeloLiteDigit): "
               << digit->sensor() << ", Z position: "
               << sensorZ << endmsg;
      }
      if (sens->isR()) {
        const DeVeloLiteRType* rSens = dynamic_cast<const DeVeloLiteRType*>(sens);
        const double radius = rSens->rOfStrip(channel.strip());
        const unsigned int zone = rSens->zoneOfStrip(channel.strip());
        if (m_printInfo) {
          info() << "Sensor: " << channel.sensor()
                 << ", strip: " << channel.strip()
                 << ", zone: " << zone
                 << ", sensorZ: " << sensorZ
                 << ", radius: " << radius
                 << ", radius/cm: " << radius / cm << endmsg;
        }
        if (0 == zone) {
          plot2D(sensorZ, radius / cm, "rz0",
                 "MCVeloLiteDigit R position vs. Z (cm), Zone 0",
                 -20., 80., 0., 5., 1000, 50);
        } else if (1 == zone) {
          plot2D(sensorZ, radius / cm, "rz1",
                 "MCVeloLiteDigit R position vs. Z (cm), Zone 1",
                 -20., 80., 0., 5., 1000, 50);
        } else if (2 == zone) {
          plot2D(sensorZ, radius / cm, "rz2",
                 "MCVeloLiteDigit R position vs. Z (cm), Zone 2",
                 -20., 80., 0., 5., 1000, 50);
        } else if (3 == zone) {
          plot2D(sensorZ, radius / cm, "rz3",
                 "MCVeloLiteDigit R position vs. Z (cm), Zone 3",
                 -20., 80., 0., 5., 1000, 50);
        } else if (4 == zone) {
          plot2D(sensorZ, radius / cm, "rz4",
                 "MCVeloLiteDigit R position vs. Z (cm), Zone 4",
                 -20., 80., 0., 5., 1000, 50);
        }
      } else if (sens->isPhi()) {
        const unsigned int strip = channel.strip();
        const unsigned int zone = sens->zoneOfStrip(strip);
        const DeVeloLitePhiType* phiSens = dynamic_cast<const DeVeloLitePhiType*>(sens);
        const double radius = phiSens->rMin(zone);
        const double phi = phiSens->globalPhi(strip, 0., radius);
        if (m_printInfo) {
          info() << "Sensor: " << channel.sensor()
                 << ", type: " << (sens->type())
                 << ", strip: " << channel.strip()
                 << ", sensorZ: " << sensorZ
                 << ", phi: " << phi / degree
                 << endmsg;
        }
        if (0 == zone) {
          plot2D(sensorZ, phi / degree, "phiz0",
                 "MCVeloLiteDigit Phi position vs. Z (cm), Zone 0",
                 -20., 80., -180., 180., 1000, 60);
        } else if (1 == zone) {
          plot2D(sensorZ, phi / degree, "phiz1",
                 "MCVeloLiteDigit Phi position vs. Z (cm), Zone 1",
                 -20., 80., -180., 180., 1000, 60);
        } else if (2 == zone) {
          plot2D(sensorZ, phi / degree, "phiz2",
                 "MCVeloLiteDigit Phi position vs. Z (cm), Zone 2",
                 -20., 80., -180., 180., 1000, 60);
        }
      }
    }
  }
  return;

}

