// $Id: GiGaMCParticleCnv.h,v 1.7 2002-05-02 11:57:03 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGACNV_GIGAMCPARTICLECNV_H
#define GIGACNV_GIGAMCPARTICLECNV_H  1 
// ============================================================================
/// STL 
#include <set>
/// GaudiKernel
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
/// LHCbEvent 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
/// GiGa
#include "GiGa/GiGaTrajectory.h" 
/// GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
/// G4 
#include "G4ParticleDefinition.hh"

/// forward declarations 
template <class TYPE> 
class CnvFactory;
///
class GiGaTrajectory;
///

/** @class GiGaMCParticleCnv GiGaMCParticleCnv.h
 *
 *  Converter of Geant4 trajectories into Gaudi MCParticles
 *
 *  @author  Vanya Belyaev
 *  @date    22/02/2001
 */

class GiGaMCParticleCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaMCParticleCnv>; 
  /// 
protected:

  /** Standard Constructor
   *  @param loc pointer to service locator 
   */
  GiGaMCParticleCnv( ISvcLocator* loc );
  /// virtual destructor 
  virtual ~GiGaMCParticleCnv();
  ///
  
public:
  
  /** initialize the converter 
   *  @return status code
   */
  virtual StatusCode initialize() ;
  
  /** finalize  the converter 
   *  @return status code
   */
  virtual StatusCode finalize  () ;
 
  /** create the Object 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode createObj     ( IOpaqueAddress*     Address   , 
                                     DataObject*&        Object    ) ;
  
  /** update the Object 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode updateObj     ( IOpaqueAddress*     Address   , 
                                     DataObject*         Object    ) ; 

  /** fill the object references 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode fillObjRefs   ( IOpaqueAddress*     Address   , 
                                     DataObject*         Object    ) ;

  /** update the object references 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode updateObjRefs ( IOpaqueAddress*     Address   , 
                                     DataObject*         Object    ) ;
  
  /** retrieve the class identifier  for created object 
   *  @return class idenfifier for created object 
   */
  static const CLID&          classID();
  
  /** retrieve the storage type for created object 
   *  @return storage type  for created object 
   */
  static const unsigned char storageType() ; 
  ///
protected: 
  
private:
  ///
  GiGaMCParticleCnv () ; /// no default constructor 
  GiGaMCParticleCnv( const GiGaMCParticleCnv& ) ; /// no copy 
  GiGaMCParticleCnv& operator=( const GiGaMCParticleCnv& ) ; /// no assignement
  ///
private: 
  
  ///
};

// ============================================================================
#endif   ///< GIGACNV_GIGAMCPARTICLECNV_H
// ============================================================================














