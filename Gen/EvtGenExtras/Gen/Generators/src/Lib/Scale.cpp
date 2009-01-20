// $Id: Scale.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2008/07/24 22:05:38  robbep
// Move to HEPMC2
//
// Revision 1.1  2006/10/06 14:11:16  ibelyaev
//  add (Read,Write)HepMCAsciiFile components
//
// ============================================================================
// Include files 
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenEvent.h"
// ============================================================================
// Local 
// ============================================================================
#include "Generators/Scale.h"
// ============================================================================
/** @file 
 *  implementation of simple functon to rescale 
 *  HepMCEvent in between Pythia and LHCb units 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-05
 */
// ============================================================================
void GeneratorUtils::scale
( HepMC::GenEvent* event , 
  const double     mom   , 
  const double     time  ) 
{
  if ( 0 == event ) { return ; }
  // rescale particles   
  for ( HepMC::GenEvent::particle_iterator ip = event->particles_begin() ;
        event->particles_end() != ip ; ++ip ) 
  {
    HepMC::GenParticle* p = *ip ;
    if ( 0 != p ) { 
      p->set_momentum( HepMC::FourVector( p->momentum().px() * mom ,
                                          p->momentum().py() * mom , 
                                          p->momentum().pz() * mom , 
                                          p->momentum().e() * mom ) ) ; 
    }  
  }
  // rescale vertices 
  for ( HepMC::GenEvent::vertex_iterator iv = event->vertices_begin() ; 
        event->vertices_end() != iv ; ++iv ) 
  {
    HepMC::GenVertex* v = *iv ;
    if ( 0 == v ) { continue ; }
    HepMC::FourVector newPos = v->position() ;
    newPos.setT ( newPos.t() * time ) ;
    v->set_position ( newPos ) ;
  }   
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

