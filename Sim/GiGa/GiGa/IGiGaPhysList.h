// $Id: IGiGaPhysList.h,v 1.8 2003-04-06 18:49:46 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef        GIGA_IGiGaPhysList_H
#define        GIGA_IGiGaPhysList_H 1 
// ============================================================================
// GiGa 
#include "GiGa/IGiGaPhysicsList.h"
#include "GiGa/IIDIGiGaPhysList.h"
// Geant4 
#include "G4VUserPhysicsList.hh"

/** @class IGiGaPhysList IGiGaPhysList.h GiGa/IGiGaPhysList.h
 *
 *  definition of (pseudo)abstract (pseudo)interface 
 *    to Geant 4 Physics List class
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

class IGiGaPhysList: 
  virtual public   IGiGaPhysicsList ,
  virtual public G4VUserPhysicsList 
{
public:
  
  /** uniqie interface identification
   *  @see IInterface 
   *  @see InterfaceID 
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_IGiGaPhysList; }
  
protected:
  
  // virtual destructor 
  virtual ~IGiGaPhysList(){};
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif   ///<   GIGA_IGiGaPhysList_H
// ============================================================================
