// $Id: GiGaVolumeUtils.cpp,v 1.1 2002-07-09 20:33:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/01/22 18:24:44  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
// 
// ============================================================================
// Include files
// Geant4 
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PhysicalVolumeStore.hh"
// GiGaCnv 
#include "GiGaCnv/GiGaAssembly.h"
#include "GiGaCnv/GiGaAssemblyStore.h"
#include "GiGaCnv/GiGaVolumeUtils.h"
// local

// ============================================================================
/** @file GiGaVolumeUtils.h 
 *  
 *  Implementation file for class : GiGaVolumeUtils
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date 20/01/2002 
 */
// ============================================================================

// ============================================================================
/** @function findLVolume 
 *  retrieve Geant4 logical volume from the store by name 
 *  @param name volume  name 
 *  @return pointer to logical volume 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date 20/01/2002 
 */
// ============================================================================
G4LogicalVolume* GiGaVolumeUtils::findLVolume ( const std::string& Name )
{
  /// get pointer to the logical volume store 
  G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  /// valid store? 
  if( 0 == store       )  { return 0 ; }                       ///< RETURN !!!
  /// loop over all store entries 
  for( G4LogicalVolumeStore::iterator ivol = store->begin() ;
       store->end() != ivol ; ++ivol ) 
    { if( Name == (*ivol)->GetName() ) { return *ivol ; } }    ///< RETURN !!!
  /// not found ?  
  return (G4LogicalVolume*) 0 ;
};

// ============================================================================
/** @function findPVolume
 *  retrieve Geant4 physical volume from the store by name
 *  @param name volume  name
 *  @return pointer to logical volume
 */
// ============================================================================
G4VPhysicalVolume* GiGaVolumeUtils::findPVolume ( const std::string& Name )
{
  /// get the pointer to the store 
  G4PhysicalVolumeStore* store = G4PhysicalVolumeStore::GetInstance();
  /// valid store ? 
  if( 0 == store    ) { return 0 ; }                           ///< RETURN !!!
  /// loop over the store 
  for( G4PhysicalVolumeStore::iterator ivol = store->begin() ; 
       store->end() != ivol; ++ivol )
    { if( Name == (*ivol)->GetName() ) { return *ivol ; } }     ///< RETURN !!!
  /// not found ?
  return (G4VPhysicalVolume*) 0 ;
};

// ============================================================================
/** @function findLAssembly
 *  retrieve assembly volume from the store by name 
 *  @param name volume  name 
 *  @return pointer to logical assembly 
 */
// ============================================================================
GiGaAssembly*    GiGaVolumeUtils::findLAssembly ( const std::string& Name )
{
  /// get the store instance 
  GiGaAssemblyStore* store = GiGaAssemblyStore::store();
  return 0 != store ? store->assembly( Name ) : (GiGaAssembly*) 0 ;
};

// ============================================================================
// End 
// ============================================================================
