// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/24 11:13:57  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
#ifndef GIGACNV_TRAJECTORY2PARTICLE_H 
#define GIGACNV_TRAJECTORY2PARTICLE_H 1
// ============================================================================
/// Include files
/// STD & STL
#include <functional>
/// forward decalrations 
class IParticlePropertySvc;
class MCParticle;
class GiGaTrajectory;

/** @class Trajectory2Particle 
 * 
 *  Class to perform the conversion of
 *  G4VTrajectory* to MCParticle* object 
 * 
 *  @author Ivan Belyaev
 *  @date   22/07/2001
 */

class Trajectory2Particle:
  public std::unary_function<MCParticle*,const GiGaTrajectory*> 
{
public:
  
  /** standard constructor
   *  @param  Svc   pointer to Particle Property Service 
   */
  Trajectory2Particle( IParticlePropertySvc* Svc   );
  /// destructor 
  ~Trajectory2Particle();
  
  /** perform the conversion of G4VTrajectory object to 
   *  MCParticle object.
   *
   *  NB! Method throws GiGaException in the case of problems! 
   *
   *  @param trajectory pointer to G4VTrajectory object 
   *  @return pointer to converted MCParticle object
   */
  MCParticle* operator()( const GiGaTrajectory* trajectory ) const;
  
  /** copy constructor 
   *  @param right const reference to object 
   */
  Trajectory2Particle( const Trajectory2Particle& right );
  
protected:
  
  /** accessor to Particle Property Service 
   *  @return pointer to Particle Property  Service 
   */
  inline IParticlePropertySvc* ppSvc() const { return m_ppSvc ; }
  
private: 
  
  /** default constructor is private! 
   */
  Trajectory2Particle();

private:
  
  IParticlePropertySvc*       m_ppSvc ; ///< particle property service
  
};

// ============================================================================
#endif ///< GIGACNV_TRAJECTORY2PARTICLE_H
// ============================================================================
