// $Id: IGiGaGeoSrc.h,v 1.4 2002-05-07 12:21:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_IGIGAGEOSRC_H 
#define GIGA_IGIGAGEOSRC_H 1
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/IInterface.h"
/// GiGa 
#include "GiGa/IIDIGiGaGeoSrc.h"
/// forward declaration from Geant4 
class G4VPhysicalVolume;


/** @interface IGiGaGeo IGiGaGeo.h "GiGa/IGiGaGeoSrc.h"
 *  
 *  definition of "minimal" geometry source interface for GiGa
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   21/07/2001
 */

class IGiGaGeoSrc: virtual public IInterface
{
public:
  
  /// Retrieve unique interface ID
  static const InterfaceID& interfaceID() { return IID_IGiGaGeoSrc; }
  
  /** retrieve the pointer to top-level "world" volume,
   *  needed for Geant4 - root for the whole Geant4 geometry tree 
   *  @return pointer to constructed(converted) geometry tree 
   */  
  virtual G4VPhysicalVolume*  world     () = 0 ;
  
  /** retrieve the pointer to top-level "world" volume,
   *  needed for Geant4 - root for the whole Geant4 geometry tree 
   *  @att obsolete method!
   *  @return pointer to constructed(converted) geometry tree 
   */  
  virtual G4VPhysicalVolume*  G4WorldPV () = 0 ;
  
protected:
  
  /// virtual destructor 
  virtual ~IGiGaGeoSrc(){};
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // GIGA_IGIGAGEOSRC_H
// ============================================================================
