// $Id: IOTrtRelation.h,v 1.1.1.1 2004-09-03 13:35:47 jnardull Exp $
#ifndef OTSIMULATION_IOTRTRELATION_H 
#define OTSIMULATION_IOTRTRELATION_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class MCOTDeposit;

static const InterfaceID IID_IOTrtRelation( "IOTrtRelation", 1, 0 );

/** @class IOTrtRelation IOTrtRelation.h "OTSimulation/IOTrtRelation.h"
 *  
 *  Outer tracker r-t relation interface.
 *
 *  @author Marco Cattaneo
 *  @date   08/01/2002
 */
class IOTrtRelation : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IOTrtRelation; }

  /// tool 'operation'. Fills the deposit time entry.
  virtual StatusCode convertRtoT(MCOTDeposit* aDeposit) = 0;
  
  /// r-t relation with correction for the magnetic field
  virtual double driftTime(const double driftDist,const HepPoint3D& aPoint) = 0;
  
  /// inverse r-t relation with correction for the magnetic field
  virtual double driftDistance( const double driftTime, 
                                const HepPoint3D& aPoint ) = 0;

};
#endif // OTSIMULATION_IOTRTRELATION_H
