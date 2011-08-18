#ifndef RETURN_PHASESPACE_HH
#define RETURN_PHASESPACE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include "Mint/Mojito/DalitzEvents/DalitzEventAccess.h"
#include "Mint/Mint/IReturnIntefaces/IReturnReal.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEventList.h"

class ReturnPhaseSpace : public DalitzEventAccess
, virtual public MINT::IGetRealEvent<IDalitzEvent>{
 public:
  ReturnPhaseSpace(IDalitzEventAccess* evts);
  ReturnPhaseSpace(IDalitzEventList* evts);
  ReturnPhaseSpace(const ReturnPhaseSpace& other);

  double RealVal();
};

#endif
//
