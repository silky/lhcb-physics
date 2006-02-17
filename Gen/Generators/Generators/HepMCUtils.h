// $Id: HepMCUtils.h,v 1.5 2006-02-17 13:17:04 robbep Exp $
#ifndef GENERATORS_HEPMCUTILS_H 
#define GENERATORS_HEPMCUTILS_H 1

// Include files
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

/** @namespace HepMCUtils HepMCUtils.h Generators/HepMCUtils.h
 *  
 *  Utility functions to use HepMC Events
 *
 *  @author Patrick Robbe
 *  @date   2006-02-14
 */

namespace HepMCUtils {
  /// Returns true if trees of vertices V1 and V2 belong to the same tree
  bool commonTrees( HepMC::GenVertex * V1 , 
                    const HepMC::GenVertex * V2 ) ;

  /// Compare 2 HepMC GenParticle according to their barcode
  bool compareHepMCParticles( const HepMC::GenParticle * part1 , 
                              const HepMC::GenParticle * part2 ) ;

  /** Check if a particle is before or after oscillation.
   *  In HepMC description, the mixing is seen as a decay B0 -> B0bar. In this
   *  case, the B0 is said to be the BAtProduction contrary to the B0bar.
   *  @param[in] thePart  Particle to check.
   *  @return true if the particle is the particle before osillation.
   */
  bool IsBAtProduction( const HepMC::GenParticle * thePart ) ;

  /// Comparison function as structure
  struct particleOrder {
    bool operator()( const HepMC::GenParticle * part1 ,
                     const HepMC::GenParticle * part2 ) const {
      return ( part1 -> barcode() < part2 -> barcode() ) ;
    }
  };

  /// Type of HepMC particles container ordered with barcodes
  typedef std::set< HepMC::GenParticle * , particleOrder > ParticleSet ;
};

//=============================================================================
// Inline functions
//=============================================================================

//=============================================================================
// Function to test if vertices are in the same decay family
//=============================================================================
inline bool HepMCUtils::commonTrees( HepMC::GenVertex * V1 , 
                                     const HepMC::GenVertex * V2 ) {
  if ( 0 == V2 ) return false ;
  if ( 0 == V1 ) return false ;
  if ( V1 == V2 ) return true ;
  HepMC::GenVertex::vertex_iterator iter ;
  for ( iter = V1 -> vertices_begin( HepMC::descendants ) ; 
        iter != V1 -> vertices_end( HepMC::descendants ) ; ++iter ) {
    if ( V2 == (*iter) ) return true ;
  }
  for ( iter = V1 -> vertices_begin( HepMC::ancestors ) ;
        iter != V1 -> vertices_end( HepMC::ancestors ) ; ++iter ) {
    if ( V2 == (*iter) ) return true ;
  }
  return false ;
}

//=============================================================================
// Function to sort HepMC::GenParticles according to their barcode
//=============================================================================
inline bool 
HepMCUtils::compareHepMCParticles( const HepMC::GenParticle * part1 ,
                                   const HepMC::GenParticle * part2 ) {
  return ( part1->barcode() < part2->barcode() ) ;
}

//=============================================================================
// Returns true if B is first B (removing oscillation B) and false
// if the B is the B after oscillation
//=============================================================================
inline bool 
HepMCUtils::IsBAtProduction( const HepMC::GenParticle * thePart ) {
  if ( ( abs( thePart -> pdg_id() ) != 511 ) && 
       ( abs( thePart -> pdg_id() ) != 531 ) ) return true ;
  if ( 0 == thePart -> production_vertex() ) return true ;
  HepMC::GenVertex * theVertex = thePart -> production_vertex() ;
  if ( 1 != theVertex -> particles_in_size() ) return true ;
  HepMC::GenParticle * theMother = 
    (* theVertex -> particles_in_const_begin() ) ;
  if ( theMother -> pdg_id() == - thePart -> pdg_id() ) return false ;
  return true ;
}

 
#endif // GENERATORS_HEPMCUTILS_H
