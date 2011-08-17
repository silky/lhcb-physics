#ifndef FIT_COMPLEX_HH
#define FIT_COMPLEX_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include <string>
#include <iostream>
#include <complex>
#include <string>
#include "counted_ptr.h"

#include "NamedParameter.h"
#include "MinuitParameterSet.h"

#include "NamedParameterBase.h"
#include "FitParameter.h"
#include "IReturnComplex.h"

#include "Phase.h"

namespace MINT{
  class FitComplex : virtual public IReturnComplex{
    //  FitComplex(const FitComplex& ){};
    // no copying for now.
    // dangerous because for each
    // FitParameter, a pointer
    // is held in MinuitParametSet.
    // Anyway, what would the copied parameter
    // mean?
  protected:
  public:

    enum TYPE{POLAR=0, CARTESIAN=1, UNDEFINED=2};

    virtual TYPE type()const=0;

    virtual FitParameter& p1()=0;
    virtual FitParameter& p2()=0;
    
    virtual const FitParameter& p1() const=0;
    virtual const FitParameter& p2() const=0;
    
    virtual void set(std::complex<double> z)=0;

    virtual std::complex<double> getVal() const=0;
    virtual std::complex<double> getValInit() const=0;
    virtual bool gotInitialised()const=0;
    virtual void print(std::ostream& os = std::cout) const;
    
    inline double getReal() const{return getVal().real();}
    inline double getImag() const{return getVal().imag();}
    
    inline double getAmp() const{return std::abs(getVal());}
    inline double getPhase() const {return std::arg(getVal());}
    
    bool isConstant() const;
    virtual bool isZero() const;
    
    virtual operator std::complex<double>() const{
      return getVal();
    }
    virtual std::complex<double> ComplexVal(){
      return getVal();
    }
    

    FitComplex();
    virtual ~FitComplex(){}
    void setParameterSet(MinuitParameterSet* pset);
    
    
    std::complex<double> operator-()const{ return - this->getVal();}
  };
  
  counted_ptr<FitComplex> FitComplexMaker(const std::string& name
					  , const char* fname=0
					  , MinuitParameterSet* pset=0
					  , FitParameter::FIX_OR_WHAT 
					  fow=FitParameter::FIX
					  , NamedParameterBase::VERBOSITY 
					  vb=NamedParameterBase::VERBOSE
					  );
  
}//namespace MINT

std::complex<double> operator*(const std::complex<double>& cplx
			       , const MINT::FitComplex& fc);
std::complex<double> operator*(const MINT::FitComplex& fc
			       ,const std::complex<double>& cplx);

std::complex<double> operator+(const std::complex<double>& cplx
			       , const MINT::FitComplex& fc);
std::complex<double> operator+(const MINT::FitComplex& fc
			       ,const std::complex<double>& cplx);

std::complex<double> operator-(const std::complex<double>& cplx
			       , const MINT::FitComplex& fc);
std::complex<double> operator-(const MINT::FitComplex& fc
			       ,const std::complex<double>& cplx);

std::complex<double> operator/(const std::complex<double>& cplx
	  , const MINT::FitComplex& fc);
std::complex<double> operator/(const MINT::FitComplex& fc
			       ,const std::complex<double>& cplx);

std::ostream& operator<<(std::ostream& os, const MINT::FitComplex& fc);

#endif
//
