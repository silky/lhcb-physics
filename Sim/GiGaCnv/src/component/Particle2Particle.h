/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef GIGACNV_PARTICLE2PARTICLE_H 
#define GIGACNV_PARTICLE2PARTICLE_H 1
/// ===========================================================================
/// STD & STL 
#include <functional>
/// GiGaCnv 
#include "Particle2Definition.h"
/// forward declarations 
class IParticlePropertySvc;
class MCParticle;
class G4PrimaryParticle;

/** @class Particle2Particle Particle2Particle.h 
 *  
 *  a helper auxiallary class to performconversion between
 *  MCParticle and G4PrimaryParticle objects 
 *
 *  @author Ivan Belyaev
 *  @date   22/07/2001
 */

class Particle2Particle: 
  public std::unary_function<const MCParticle*,G4PrimaryParticle*> 
{
public:
  
  /** standard constructor
   *  @param Svc pointer to particle property service 
   */
  Particle2Particle( IParticlePropertySvc* Svc );
  /// Destructor
  ~Particle2Particle( ); 
  
  /** convert MCParticle object into G4Primary particle object
   *  The method is recursive!
   *  @param particle pointer to MCParticle object
   *  @return pointer to new G4Primary particle
   */
  G4PrimaryParticle* operator() ( const MCParticle* particle ) const ;
  
protected:
  
  /** get the particle definition for given particle 
   *  @param particle pointer to MCParticle object
   *  @return Geant4 particle definition 
   */
  inline G4ParticleDefinition* 
  definition( const MCParticle* particle ) const 
  { return m_p2d( particle ); }
  
private:
  
  /// particle to particle definition converter 
  Particle2Definition  m_p2d;
  
};

/// ===========================================================================
#endif /// < GIGACNV_PARTICLE2PARTICLE_H
/// ===========================================================================
