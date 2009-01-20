// $Id: IVertexSmearingTool.h,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
#ifndef GENERATORS_IVERTEXSMEARINGTOOL_H 
#define GENERATORS_IVERTEXSMEARINGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declaration
namespace LHCb {
  class HepMCEvent ;
}

/** @class IVertexSmearingTool IVertexSmearingTool.h "Generators/IVertexSmearingTool.h"
 *  
 *  Abstract interface to vertex smearing tools. Concrete implementations 
 *  apply vertex smearing algorithms to each generated pile-up interactions.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IVertexSmearingTool( "IVertexSmearingTool" , 2 , 
                                                  0 ) ;

class IVertexSmearingTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IVertexSmearingTool ; }
  
  /// Smear the vertex of the interaction (independantly of the others)
  virtual StatusCode smearVertex( LHCb::HepMCEvent * theEvent ) = 0 ;
};
#endif // GENERATORS_ISMEARINGTOOL_H
