// $Id: IProductionTool.h,v 1.2 2005-11-04 10:51:48 robbep Exp $
#ifndef GENERATORS_IPRODUCTIONTOOL_H 
#define GENERATORS_IPRODUCTIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"


/** @class IProductionTool IProductionTool.h Generators/IProductionTool.h
 *  
 *  Abstract interface to production generators
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-16
 */

static const InterfaceID IID_IProductionTool( "IProductionTool" , 1 , 0 ) ;

class HardInfo ;
namespace HepMC { class GenEvent ; }
class ParticleProperty ;

class IProductionTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IProductionTool ; }
  
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    HardInfo * theInfo ) = 0 ;

  virtual void setStable( const ParticleProperty * thePP ) = 0 ;

  virtual void turnOffFragmentation( ) = 0 ;
  virtual void turnOnFragmentation( )  = 0 ;

  virtual StatusCode hadronize( HepMC::GenEvent * theEvent , 
                                HardInfo * theInfo ) = 0 ;

  virtual void savePartonEvent( HepMC::GenEvent * theEvent ) = 0 ;
  virtual void retrievePartonEvent( HepMC::GenEvent * theEvent ) = 0 ;
};
#endif // GENERATORS_IPRODUCTIONTOOL_H
