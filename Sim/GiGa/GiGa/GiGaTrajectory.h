// $Id: GiGaTrajectory.h,v 1.13 2002-12-07 21:05:30 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.12  2002/12/07 14:27:50  ibelyaev
//  see $GIGAROOT/cmt/requirements file
//
// ============================================================================
#ifndef    GIGA_GIGATRAJECTORY_H
#define    GIGA_GIGATRAJECTORY_H 1 
// ============================================================================
/// STL
#include <vector>
/// base "interface" class
#include "G4VTrajectory.hh" 
/// GiGa 
#include "GiGa/GiGaTrajectoryPoint.h"
///
class G4ParticleDefinition;
class G4Track;
class G4Step;
class G4SteppingManager;
///

/** @class GiGaTrajectory GiGaTrajectory.h GiGa/GiGaTrajectory.h
 *
 *  Customisation of "standard" G4Trajectory class to 
 *  incorporate TimeOfFlight of the each point. 
 *  Overwise conversion to MCParticle/MCVertex structure is impossible.  
 *  
 *  @author  Vanya Belyaev
 *  @date    22/02/2001
*/

class GiGaTrajectory: public G4VTrajectory                    , 
                      public std::vector<GiGaTrajectoryPoint*>
{
public:
  
  /// default (empty) constructor
  GiGaTrajectory (                              );

  /** constructor from the track
   *  @param track pointer to the track 
   */
  GiGaTrajectory ( const G4Track*         track );

  /** copy constructor 
   *  @param right object to be copied  
   */  
  GiGaTrajectory ( const GiGaTrajectory & right );

  /// destructor 
  virtual ~GiGaTrajectory();

  ///  overloaded new operator 
  inline void* operator new    ( size_t ) ;
  
  ///  overloaded delete operator 
  inline void  operator delete ( void*  ) ;
  
  /** clone (virtual constructor) method 
   *  @return clone of current trajectory
   */
  virtual GiGaTrajectory* clone() const ;
  
  /// comparison (needed by G4)
  inline int operator == (const GiGaTrajectory& right) const
  {return ( &right == this );}
  
  /// accessors - a'la Gaudi and a'la Geant4 
  inline const int&                  trackID     () const 
  { return m_trackID      ; } 
  inline const int&                  GetTrackID  () const 
  { return trackID ()     ; }
  inline const int&                  parentID    () const 
  { return m_parentID     ; }  
  inline const int&                  GetParentID () const 
  { return parentID()     ; }
  inline const HepLorentzVector&     fourMomentum() const 
  { return m_4vect        ; } 
  inline const HepLorentzVector&     Get4Momentum() const 
  { return fourMomentum() ; } 
  /// 
  inline const G4ParticleDefinition* partDef     () const 
  { return m_partDef ; } 
  ///
  virtual void ShowTrajectory  ()                 const ;
  virtual void DrawTrajectory  ( G4int i_mode=0 ) const ;
  virtual void AppendStep      ( const G4Step*  )       ;
  virtual void MergeTrajectory ( G4VTrajectory* )       ;
  ///
  virtual int                 GetPointEntries(          ) const 
  { return size() ; }
  virtual G4VTrajectoryPoint* GetPoint       ( int index ) const   
  { return point( index ) ; }
  
  G4VTrajectoryPoint*         point          ( int index ) const   
  { return (size_t) index < size() ? *(begin()+index) : 0 ; }
  
  ///
private:
  /// assignement operator id private 
  GiGaTrajectory& operator=( const GiGaTrajectory& );
private: 
  ///
  int                          m_trackID  ;
  int                          m_parentID ;
  const G4ParticleDefinition*  m_partDef  ; 
  HepLorentzVector             m_4vect    ;   
  /// 
};
// ============================================================================

// ============================================================================
/** @fn  gigaTrajectory
 *  Fast cast of G4VTrajectory interface to concrete GiGa implementation
 *  @param  g4   pointer to G4VTrajectory interface 
 *  @return cast (dynamic or static) to GiGaTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GiGaTrajectory* gigaTrajectory( G4VTrajectory* g4 )
{
  GiGaUtil::FastCast<G4VTrajectory,GiGaTrajectory> cast ;
  return cast( g4  );
};
// ============================================================================


// ============================================================================
#endif  ///< GIGA_GIGATRAJECTORY_H
// ============================================================================







