// $Id: GaussTrackActionByProcess.h,v 1.1 2004-02-20 19:35:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONBYENERGY_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONBYENERGY_H 1
// ============================================================================
// local
// ============================================================================
#include "GaussTools/GaussTrackActionZ.h"
// ============================================================================

/** @class GaussTrackActionByProcess GaussTrackActionByProcess.h 
 *  
 *  Track action, valid for certain region in Z based on 
 *  particle/children types 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
class GaussTrackActionByProcess : public GaussTrackActionZ 
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GaussTrackActionByProcess> ;
public:

  /// perform initialization
  virtual StatusCode initialize () ;
  
  /** perform action 
   *  @see G4VUserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PreUserTrackingAction  ( const G4Track* ) ;
  
  /** perform action 
   *  @see G4VUserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PostUserTrackingAction ( const G4Track* ) ;
  
protected:
  
  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussTrackActionByProcess
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionByProcess();
  
private:
  
  // default constructor   is disabled 
  GaussTrackActionByProcess() ; 
  // copy    constructor   is disabled 
  GaussTrackActionByProcess           ( const GaussTrackActionByProcess& ) ; 
  // assignement operator  is disabled 
  GaussTrackActionByProcess& operator=( const GaussTrackActionByProcess& ) ;
  
protected:
  
  typedef std::vector<std::string> Processes ;
  typedef std::vector<int>         ProcTypes ;
  
  bool   storeByOwnProcess () const 
  {
    if ( m_ownProcs.empty () && m_ownPTypes.empty () ) { return false ; }
    if ( 0 == trackMgr    () ) { return false ; }      // RETURN 
    const G4Track* track = trackMgr()->GetTrack() ;  
    if ( 0 == track          ) { return false ; }      // RETURN 
    const G4VProcess* process  = track ->GetCreatorProcess() ;
    if ( 0 == process        ) { return false ; }      // RETURN 
    if (   std::binary_search ( m_ownPTypes.begin       () , 
                                m_ownPTypes.end         () , 
                                process->GetProcessType () ) ) { return true ; }
    return std::binary_search ( m_ownProcs.begin        () ,
                                m_ownProcs.end          () ,
                                process->GetProcessName () ) ;
  };
  
  bool   storeByChildProcess () const 
  {
    if  ( m_childProcs.empty () && m_childPTypes.empty () ) { return false ; }
    if  ( 0 == trackMgr    () ) { return false ; }      // RETURN 
    const G4TrackVector* children = trackMgr()->GimmeSecondaries() ;  
    if  ( 0 == children       ) { return false ; }      // RETURN 
    for ( unsigned int i = 0 ; i < children->size() ; ++i ) 
    {
      const G4Track* track = (*children)[i];
      if ( 0 == track   ) { continue ; }
      const G4VProcess* process = track->GetCreatorProcess() ;
      if ( 0 == process ) { continue ; }
      if ( std::binary_search ( m_childPTypes.begin     () , 
                                m_childPTypes.end       () , 
                                process->GetProcessType () ) ) { return true ; }
      if ( std::binary_search ( m_childProcs.begin      () ,
                                m_childProcs.end        () ,
                                process->GetProcessName () ) ) { return true ; }
      
    }
    return false ;
  };
  
private:
  
  Processes m_ownProcs    ;   // sorted !
  Processes m_childProcs  ;   // sorted !
  ProcTypes m_ownPTypes   ;   // sorted !
  ProcTypes m_childPTypes ;   // sorted !
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GAUSSTRACKACTIONBYENERGY_H
// ============================================================================
