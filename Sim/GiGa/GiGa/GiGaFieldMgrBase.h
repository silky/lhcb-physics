// $Id: GiGaFieldMgrBase.h,v 1.4 2005-10-25 17:21:40 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/07/05 16:06:19  gcorti
// introduce property DeltaChord
//
// Revision 1.2  2003/10/30 16:33:33  witoldp
// modified GiGaFieldManager
//
// Revision 1.1  2003/04/06 18:49:45  ibelyaev
//  see $GIGAROOT/doc/release.notes
// 
// ============================================================================
#ifndef GIGA_GIGAFIELDMGRBASE_H 
#define GIGA_GIGAFIELDMGRBASE_H 1
// ============================================================================
// Include files
#include "GiGa/IGiGaFieldMgr.h"
#include "GiGa/GiGaBase.h"
#include "GiGa/GiGaFactory.h"

/** @class GiGaFieldMgrBase GiGaFieldMgrBase.h GiGa/GiGaFieldMgrBase.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class GiGaFieldMgrBase :
  public virtual IGiGaFieldMgr , 
  public GiGaBase
{
public:
  
  /** initialize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize     () ; 
  
  /** finalize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   finalize       () ;
  
  /** flag for indication global/local characted of 
   *  the tool field manager  
   */
  virtual bool                    global   () const ;

  /** accessor to field  manager 
   *  @see G4FieldManager
   *  @return pointer to the field manager  
   */
  virtual G4FieldManager*         fieldMgr () const ;
  
  /** accessor to the stepper 
   *  @see G4MagIntegratorStepper
   *  @see IGiGaFieldMgr 
   *  @return pointer to the stepper  
   */
  virtual G4MagIntegratorStepper* stepper  () const ;
  
protected: 
  
  /** create the stepper 
   *  @return status code 
   */
  StatusCode createStepper  () const ;

  /** create and configure Field Manager 
   *  @return status code
   */
  StatusCode createFieldMgr () const ;
  
protected:
  
  /** Standard constructor
   *  @see GiGaBase
   *  @see  AlgTool 
   *  @param type   type of the object (?) 
   *  @param name   instance name 
   *  @param parent pointer to the parent 
   */
  GiGaFieldMgrBase 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ;
  
  /// destructor 
  virtual ~GiGaFieldMgrBase( ); 
  
private:
  
  // the default constructor is disabled 
  GiGaFieldMgrBase ();
  // the copy  constructor is disabled 
  GiGaFieldMgrBase            ( const GiGaFieldMgrBase& ) ;
  // the assignement operator is disabled 
  GiGaFieldMgrBase& operator= ( const GiGaFieldMgrBase& ) ;
  
private:

  bool m_global;                         ///< global/local flag 
  mutable G4FieldManager* m_manager;     ///< field manager itself 
  double m_minStep;                      ///< minimal step
  // delta chord - minimal distance between chord and curved trajectory
  double m_deltaChord;
  // delta intersection - max acceptable error on volume intersection
  double m_deltaintersection;
  // delta one step - max acceptable error on integration step (pos and mom)
  double m_deltaonestep;
  // min epsilon step - if deltaonestep < this, this is max acceptable error
  double m_minimumEpsilonStep;
  // max epsilon step - if deltaonestep > this, this is max acceptable error 
  double m_maximumEpsilonStep;
  // stepper type 
  std::string                     m_stepperType ;
  // the stepper itself 
  mutable G4MagIntegratorStepper* m_stepper     ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAFIELDMGRBASE_H
// ============================================================================
