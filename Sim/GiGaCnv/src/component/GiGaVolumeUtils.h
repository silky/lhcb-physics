// $Id: GiGaVolumeUtils.h,v 1.1 2002-01-22 18:24:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGACNV_GIGAVOLUMEUTILS_H
#define GIGACNV_GIGAVOLUMEUTILS_H 1
// Include files
// STD & STL
#include <string>
// forward declarations
class G4LogicalVolume  ; ///< Geant4
class GVPhysicalVolume ; ///< Geant4
class GiGaAssembly     ; ///< GiGaCnv

/** @namespace GiGaVolumeUtils GiGaVolumeUtils.h
 *
 *  collection of useful methods for extraction of
 *  volumes/assemblies from their static stores
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   20/01/2002
 */

namespace GiGaVolumeUtils
{

  /** @function findLVolume
   *  retrieve Geant4 logical volume from the store by name
   *  @param name volume  name
   *  @return pointer to logical volume
   */
  G4LogicalVolume*   findLVolume   ( const std::string& Name );
  
  /** @function findPVolume
   *  retrieve Geant4 physical volume from the store by name
   *  @param name volume  name
   *  @return pointer to logical volume
   */
  G4VPhysicalVolume* findPVolume ( const std::string& Name );

  /** @function findLAssembly
   *  retrieve assembly volume from the store by name
   *  @param name volume  name
   *  @return pointer to logical assembly
   */
  GiGaAssembly*      findLAssembly ( const std::string& Name );

};

// ============================================================================
// End
// ============================================================================
#endif // GIGACNV_GIGAVOLUMEUTILS_H
// ============================================================================
