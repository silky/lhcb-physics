// $Id: MomentumRange.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_MOMENTUMRANGE_H
#define PARTICLEGUNS_MOMENTUMRANGE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from ParticleGuns
#include "LbPGuns/IParticleGunTool.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class MomentumRange MomentumRange.h "MomentumRange.h"
 *
 *  Particle gun with given momentum range
 *  
 *  @author Patrick Robbe
 *  @date   2008-05-18
 */
class MomentumRange : public GaudiTool , virtual public IParticleGunTool {
 public:
  
  /// Constructor
  MomentumRange( const std::string & type , const std::string& name, 
                 const IInterface * parent ) ;
  
  /// Destructor
  virtual ~MomentumRange();
  
  /// Initialize particle gun parameters
  virtual StatusCode initialize();

  /// Generation of particles
  virtual void generateParticle( Gaudi::LorentzVector & momentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId ) ;

  /// Print counters
  virtual void printCounters( ) { ; } ;
                                 
 private:  
  double m_minMom;   ///< Minimum momentum (Set by options)  
  double m_minTheta; ///< Minimum theta angle (Set by options)
  double m_minPhi;   ///< Minimum phi angle (Set by options)

  double m_maxMom;   ///< Maximum momentum (Set by options)
  double m_maxTheta; ///< Maximum theta angle (Set by options)
  double m_maxPhi;   ///< Maximum phi angle (Set by options)

  /// Pdg Codes of particles to generate (Set by options)
  std::vector<int>         m_pdgCodes;

  /// Masses of particles to generate
  std::vector<double>      m_masses;

  /// Names of particles to generate
  std::vector<std::string> m_names;

  /// Flat random number generator
  Rndm::Numbers m_flatGenerator ;
};

#endif // PARTICLEGUNS_MOMENTUMRANGE_H
