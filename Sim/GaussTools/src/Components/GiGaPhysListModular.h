// ============================================================================
#ifndef    GIGA_GiGaPhysListModular_H
#define    GIGA_GiGaPhysListModular_H 1 
// ============================================================================
// include files 

// G4
#include "G4VModularPhysicsList.hh"

// GiGa 
#include "GiGa/GiGaPhysListBase.h"


// forward declarations 
template <class TYPE> class GiGaFactory;



class GiGaPhysListModular : public GiGaPhysListBase,
                            public virtual G4VModularPhysicsList
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysListModular>;

protected:
  
  /** standard constructor 
   *  @see GiGaPhysListBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaPhysListModular
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaPhysListModular();

  ///
public:
  StatusCode initialize();
  
  void SetCuts();
  ///
private:
  std::vector<std::string> m_physconstr;

  ///
  GiGaPhysListModular();
  GiGaPhysListModular           ( const GiGaPhysListModular& );
  GiGaPhysListModular& operator=( const GiGaPhysListModular& );
  ///
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysListModular_H
// ============================================================================











