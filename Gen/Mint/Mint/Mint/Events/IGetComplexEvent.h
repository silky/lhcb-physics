#ifndef IGETCOMPLEXEVENT_HH
#define IGETCOMPLEXEVENT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/Mint/IReturnIntefaces/IReturnComplex.h"
#include "Mint/Mint/Events/IEventAccess.h"
//#include <complex>

namespace MINT{

template<typename EVENT>
class IGetComplexEvent 
: virtual public IEventAccess<EVENT>
, virtual public IReturnComplex
{
 public:
  //  virtual double getVal()=0;
  //  virtual std::complex<double> ComplexVal()=0;
};

}//namespace MINT
#endif
//
