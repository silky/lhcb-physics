// $Id$
// Include files
//#include <iterator>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Linker
#include "Linker/LinkerWithKey.h"

// from FTEvent
#include "Event/MCFTDigit.h"
#include "Event/FTCluster.h"

// local
#include "FTClusterCreator.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : FTClusterCreator
//
// 2012-04-06 : Eric Cogneras
// TO DO :
// - split cluster once cluster width is above
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FTClusterCreator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  FTClusterCreator::FTClusterCreator( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputLocation" ,    m_inputLocation    = LHCb::MCFTDigitLocation::Default, "Path to input MCDigits");
  declareProperty("OutputLocation" ,   m_outputLocation   = LHCb::FTClusterLocation::Default, "Path to output Clusters");
  declareProperty("ADCThreshold" ,     m_adcThreshold     = 1 , "Minimal ADC Count to be added in cluster");
  declareProperty("ClusterMinWidth" ,  m_clusterMinWidth  = 2
 , "Minimal allowed width for clusters");
  declareProperty("ClusterMaxWidth" ,  m_clusterMaxWidth  = 8 , "Maximal allowed width for clusters");
  declareProperty("ClusterMinCharge" , m_clusterMinCharge = 2 , "Minimal charge to keep cluster");
  declareProperty("ClusterMinADCPeak" , m_clusterMinADCPeak = 2 , "Minimal ADC for cluster peaks");
}
//=============================================================================
// Destructor
//=============================================================================
FTClusterCreator::~FTClusterCreator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FTClusterCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ){
    debug() << "==> Initialize" << endmsg;
    debug() << ": InputLocation is " <<m_inputLocation << endmsg;
    debug() << ": OutputLocation is " <<m_outputLocation << endmsg;
    debug() << ":m_adcThreshold is " <<m_adcThreshold << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTClusterCreator::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // retrieve FTDigits
  const MCFTDigits* mcDigitsCont = get<MCFTDigits>(m_inputLocation);
  debug() <<"mcDigitsCont->size() : " << mcDigitsCont->size()<< endmsg;

  // define clusters container
  FTClusters *clusterCont = new FTClusters();
  clusterCont->reserve(mcDigitsCont->size());

  // Register FTClusters in the transient data store
  put(clusterCont, m_outputLocation);

  // Create a link between the FTCluster and the MCParticle which leave a track
  LinkerWithKey<LHCb::MCParticle,LHCb::FTCluster> myLink( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default);

  // DEBUG : print Digit content : should be sorted
  if(msgLevel(MSG::DEBUG)){
    for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
      MCFTDigit* mcDigit = *iterDigit;
      std::map< const LHCb::MCParticle*, double> mcMap = mcDigit->mcParticleMap();
      debug() <<"Channel ="<<mcDigit->channelID()<< " : " <<"\t ADC ="<<mcDigit->adcCount() << endmsg;
      for(std::map<const LHCb::MCParticle*,double>::const_iterator mapiter=mcMap.begin(); mapiter!=mcMap.end(); ++mapiter){
        debug() <<"MCParticle : index="<<mapiter->first->index() <<" "<<mapiter->first->particleID()
                << " Energy=" <<mapiter->second << endmsg;
      }
    }
  }


  // Since Digit Container is sorted wrt channelID, clusters are defined searching for bumps of ADC Count
  MCFTDigits::const_iterator seedDigitIter = mcDigitsCont->begin();


  while(seedDigitIter != mcDigitsCont->end()){ // loop over digits
    MCFTDigit* seedDigit = *seedDigitIter;

    if(seedDigit->adcCount() > m_adcThreshold){ // ADC count in  digit is over threshold : start cluster

      // vector of ADC count of each cell from the cluster
      std::vector<int> clusterADCDistribution;

      // total energy from MC
      double totalEnergyFromMC = 0;

      // map of contributing MCParticles with their relative energy deposit
      std::map< const LHCb::MCParticle*, double> mcContributionMap;
      if ( msgLevel( MSG::DEBUG) ) {
        debug() <<"++ Starts clustering with Channel "<<seedDigit->channelID()
                <<" (ADC = "<<seedDigit->adcCount() <<" )"<< endmsg;
      }
    
      MCFTDigits::const_iterator lastDigitIter = seedDigitIter;
      do{
        // Add digit to cluster
        MCFTDigit* lastDigit = *lastDigitIter;
        if ( msgLevel( MSG::DEBUG) ) {
          debug() <<"+Add to Cluster : "<<lastDigit->channelID()<<" (ADC = "<<lastDigit->adcCount() <<" )"<< endmsg;
        }
    
        clusterADCDistribution.push_back(lastDigit->adcCount());

        // Keep track of MCParticles involved in the cluster definition
        std::map<const LHCb::MCParticle*, double> mcMap = lastDigit->mcParticleMap();
        std::map<const LHCb::MCParticle*, double>::const_iterator mcMapIter=mcMap.begin();
        for(; mcMapIter!=mcMap.end(); ++mcMapIter){
          totalEnergyFromMC +=mcMapIter->second;
          mcContributionMap[mcMapIter->first] += mcMapIter->second;
        }
        ++lastDigitIter;

      }while((lastDigitIter != mcDigitsCont->end()) 
             && keepAdding(lastDigitIter)
             && (clusterADCDistribution.size()<m_clusterMaxWidth));
      

      // Compute total ADC counts and cluster barycenter position in ChannelID unit
      unsigned int totalCharge = 0;
      double meanPosition = 0;
      
      for(std::vector<int>::const_iterator i = clusterADCDistribution.begin();i != clusterADCDistribution.end(); ++i){
        totalCharge += *i;
        meanPosition += (i-clusterADCDistribution.begin()) * (*i);
        if(msgLevel(MSG::DEBUG)){
          debug() <<"- Distance="<< (i-clusterADCDistribution.begin())
                  <<" adc="<<*i << " totalCharge=" << totalCharge
                  << endmsg;
        }
      }
      meanPosition =(*seedDigitIter)->channelID() + meanPosition/totalCharge;


      // Checks :
      // - total ADC charge of the cluster is over threshold and
      // - number of cells above the minimal allowed value      
      if( ( totalCharge > m_clusterMinCharge) 
          && (clusterADCDistribution.size() > m_clusterMinWidth) ) {
        
        // Define Cluster(channelID, fraction, width, charge)  and save it
        int meanChanPosition = std::floor(meanPosition);
        double fractionChanPosition = (meanPosition-std::floor(meanPosition));

        // Define Cluster(channelID, fraction, width, charge)  and save it
        FTCluster *newCluster = new FTCluster(meanChanPosition,fractionChanPosition,(lastDigitIter-seedDigitIter),totalCharge);

        clusterCont->insert(newCluster);


        // Define second member of mcContributionMap as the fraction of energy corresponding to each involved MCParticle
        for(std::map<const LHCb::MCParticle*,double>::iterator i = mcContributionMap.begin(); i != mcContributionMap.end(); ++i){
          myLink.link(newCluster, (i->first), (i->second)/totalEnergyFromMC ) ;
          if ( msgLevel( MSG::DEBUG) ) {
            debug() << "Linked ClusterChannel=" << newCluster->channelID()
                    << " to MCIndex="<<i->first->index()
                    << " with EnergyFraction=" << (i->second)/totalEnergyFromMC
                    << endmsg;
          }
        }

        // DEBUG
        if ( msgLevel( MSG::DEBUG) ) {
          debug() <<"+ Finish clustering from : "<<(*seedDigitIter)->channelID()
                  <<" (ADC = "<<(*seedDigitIter)->adcCount() <<" ) to "<<(*(lastDigitIter-1))->channelID()
                  <<" (ADC = "<<(*(lastDigitIter-1))->adcCount() <<")"<< endmsg;

          debug() <<"= Cluster is : "<<newCluster->channelID()
                  <<" Charge = "<<newCluster->charge()
                  << " Width = " << newCluster->size()
                  << " Fraction = " <<newCluster->fraction()
                  << endmsg;
        }

      }
      seedDigitIter = (lastDigitIter-1);
    }

    ++seedDigitIter;

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTClusterCreator::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
bool FTClusterCreator::keepAdding(LHCb::MCFTDigits::const_iterator clusCandIter)
{
  /** checks that channelID is :
      - 1. in the same SiPM
      - 2. follows the previous one
      - 3. previous channel not 63
      - 4. previous channel not 127
      - 5. above threshold

  */
  return (((*clusCandIter)->channelID().sipmId() == ((*(clusCandIter-1))->channelID().sipmId()))
          &&((*clusCandIter)->channelID() == ((*(clusCandIter - 1))->channelID() + 1))
          && ((*(clusCandIter - 1 ))->channelID().sipmCell() != 63 )
          && ((*(clusCandIter - 1 ))->channelID().sipmCell() != 127 )
          &&((*clusCandIter)->adcCount() > m_adcThreshold));

}




