// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include "Mint/Mint/IReturnIntefaces/ComplexProduct.h"
#include <iostream>
using namespace std;
using namespace MINT;

ComplexProduct::ComplexProduct()
  : _facVec(), _fixedDoubleVec(), _fixedComplexVec()
{
}
ComplexProduct::ComplexProduct(double initVal)
  : _facVec(), _fixedDoubleVec(), _fixedComplexVec()
{
  addTerm(initVal);
}

ComplexProduct::ComplexProduct(const std::complex<double>& z)
  : _facVec(), _fixedDoubleVec(), _fixedComplexVec()
{
  addTerm(z);
}

ComplexProduct::ComplexProduct(const ComplexProduct& other)
  : _facVec(), _fixedDoubleVec(), _fixedComplexVec()
{
  for(unsigned int i=0; i< other._facVec.size(); i++){
    addTerm(other._facVec[i]);
  }
  for(unsigned int i=0; i< other._fixedDoubleVec.size(); i++){
    addTerm(other._fixedDoubleVec[i]);
  }
  for(unsigned int i=0; i< other._fixedComplexVec.size(); i++){
    addTerm(other._fixedComplexVec[i]);
  }
}

void ComplexProduct::addTerm(double val){
  _fixedDoubleVec.push_back(val);
}

void ComplexProduct::addTerm(const std::complex<double>& z){
  //  counted_ptr<IReturnComplex> bz(new BasicComplex(z));
  // addTerm(bz);
  _fixedComplexVec.push_back(z);
}

void ComplexProduct::addTerm(const counted_ptr<IReturnComplex>& irc){
  bool dbThis=false;
  _facVec.push_back(irc);
  if(dbThis){
    cout << "ComplexProduct::addTerm as pointer"
	 << " just got: " << irc->ComplexVal()
	 << " taking me to prod = " << ComplexVal()
	 << endl;
  }
}

ComplexProduct& ComplexProduct::operator*=(double val){
  addTerm(val);
  return *this;
}
ComplexProduct& ComplexProduct::operator*=(const std::complex<double>& z){
  addTerm(z);
  return *this;
}
ComplexProduct& ComplexProduct::operator*=(const counted_ptr<IReturnComplex>& 
					   irc){
  addTerm(irc);
  return *this;
}

/* now inlined:
std::complex<double> ComplexProduct::ComplexVal(){
  std::complex<double> prod(1,0);

  for(unsigned i = 0; i < _facVec.size(); i++){
    prod *= (_facVec[i])->ComplexVal();
  }

  return prod;
}
*/
//
