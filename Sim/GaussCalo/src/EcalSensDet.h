// $Id: EcalSensDet.h,v 1.1 2003-07-07 08:21:10 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef       GAUSSCALO_EcalSensDet_H
#define       GAUSSCALO_EcalSensDet_H 1 
// ============================================================================
// GaudiKernel
/// Ntupel Svc 
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
/// GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "GiGa/GiGaHashMap.h"
/// local
#include "CaloSim.h"
#include "CaloHit.h"
#include "EHCalSensDet.h"
/// forward declarations 
template <class SD> class GiGaSensDetFactory;

class DeCalorimeter ;

/** @class EcalSensDet EcalSensDet.h
 *
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Patrick Robbe robbe@lal.in2p3.fr 
 *
 *  @date    23/01/2001
 */

class EcalSensDet: public EHCalSensDet 
{
  /// friend factory 
  friend class GiGaFactory<EcalSensDet>;
  
protected:
  
  /** fill the hit with the concrete information about the energy and the time.
   *  The function is to be called from ProcessHits method.
   *
   *  @param hit        hit  to be filled with the correct information, 
   *                    the cellID information is accessible from the hit
   *  @param prePoint   PreStepPoint  for the given step 
   *  @param globalTime the global time of prestep point 
   *  @param deposit    raw energy deposition for the step 
   *  @param track      the actual G4Track pbject (decoded from G4Step)
   *  @param pdef       the actual particle type  (decoded from G4Step/G4Track)
   *  @param material   the actual material       (decoded from G4Step) 
   *  @param step       the step itself, the most important 
   *                    information from the step 
   *                    is already decoded into prePoint,globalTime,track,
   *                    particle definition,material 
   *                    etc for efficiency reasons.
   *
   */  
  virtual StatusCode    fillHitInfo 
  ( CaloSubHit*                 hit         ,
    const HepPoint3D&           prePoint    ,
    const double                globalTime  , 
    const double                deposit     ,
    const G4Track*              track       , 
    const G4ParticleDefinition* pdef        ,
    const G4Material*           material    ,
    const G4Step*               step        ) const ;
  
protected:
  
  /** standard constructor 
   *  @see CaloSensDet 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  EcalSensDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~EcalSensDet() {};
  
private:
  
  // no default constructor 
  EcalSensDet() ; 
  // no copy constructor
  EcalSensDet           ( const EcalSensDet& ); 
  // no assignement  
  EcalSensDet& operator=( const EcalSensDet& ) ;
  
protected:
  
  /** Correction due to the local non uniformity due to the light 
   *  collection efficiency in cell cell
   */
  double localNonUniformity ( const G4Step*     step    , 
                              const CaloCellID& cell    ) const ;  
  
  /** The energy deposited in the cell by the step contributes 
   *  - frac1*energy in the time bin timeBin (25 ns bins)
   *  - frac2*energy in the time bin timeBin+1
   *  the origin is the time of collision
   */
  
  void   timeFraction       ( const G4Step*     step    , 
                              const CaloCellID& cell    ,
                              double&           frac1   , 
                              double&           frac2   , 
                              int&              timeBin ) const ;
  
private :
  
  double                                      m_a_local_inner_ecal  ;
  double                                      m_a_local_middle_ecal ;
  double                                      m_a_local_outer_ecal  ;
  
  double                                      m_ecalTimeHisto[2][25] ;  

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///< GAUSSCALO_EcalSensDet_H
// ============================================================================
