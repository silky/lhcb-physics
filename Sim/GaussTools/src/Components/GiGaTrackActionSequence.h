// $Id: GiGaTrackActionSequence.h,v 1.1 2002-12-12 15:19:33 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/26 18:10:55  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.4  2002/05/07 12:21:37  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef COMPONENT_GIGATRACKACTIONSEQUENCE_H 
#define COMPONENT_GIGATRACKACTIONSEQUENCE_H 1
// ============================================================================
/// STD & STL 
#include <string>
#include <vector>
/// GiGa 
#include "GiGa/GiGaTrackActionBase.h"
/// forward decalrations 
template <class TYPE> class GiGaFactory;

/** @class GiGaTrackActionSequence GiGaTrackActionSequence.h
 *
 *  A concrete implementation of IGiGaTrackAction interface
 *  Simple action - just a chain/sequence of other Track Actions   
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/07/2001
 */

class GiGaTrackActionSequence: public GiGaTrackActionBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaTrackActionSequence>;
  ///
public:
  
  typedef  std::vector<std::string>        MEMBERS ;
  typedef  std::vector<IGiGaTrackAction*>  ACTIONS ; 
  
  /** initialize the track action  
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ; 
  
  /** finalize the action object 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code
   */ 
  virtual StatusCode finalize   () ;
  
  /** perform the pre-action
   *  @see G4UserTrackingAction
   *  @param track pointer to Geant4 track object 
   */ 
  virtual void PreUserTrackingAction  ( const G4Track* track );
  
  /** perform the post-action
   *  @see G4UserTrackingAction
   *  @param track pointer to Geant4 track object 
   */ 
  virtual void PostUserTrackingAction ( const G4Track* track );
  
protected:
  
  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaTrackActionSequence
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaTrackActionSequence();

private:

  /// no default constructor
  GiGaTrackActionSequence();
  /// no copy constructor 
  GiGaTrackActionSequence( const GiGaTrackActionSequence& );
  /// no assignment
  GiGaTrackActionSequence& operator=( const GiGaTrackActionSequence& );
  
private:
  
  MEMBERS   m_members ;
  ACTIONS   m_actions ;
  
  /// boolean flag to inform all members about the tracking manager 
  bool      m_setMgr ;
  
};

// ============================================================================
#endif ///< COMPONENT_GIGATRACKACTIONSEQUENCE_H
// ============================================================================
