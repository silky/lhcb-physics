// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "CrystalBallRhoOmega.h"
#include "CLHEPPhysicalConstants.h"
#include "ParticlePropertiesList.h"

#include <iostream>

using namespace std;

CrystalBallRhoOmega::CrystalBallRhoOmega( const AssociatedDecayTree& tree
					  , IDalitzEventAccess* events)
  : BW_BW(tree, events)
  , _omegaBW(tree, events, 223)
  , _eps("CrystalBallRhoOmega::eps", 1.14)
  , _beta("ChristalBallRhoOmega::beta", -4.3*pi/180.0)
  , _phi("ChristalBallRhoOmega::phi", 107*pi/180.0)
  , _delta("ChristalBallRhoOmega::delta", 2.5*MeV)
  , _aRatioSq("ChristalBallRhoOmega::aRatioSq", 3.9)
  , _mRho(-9999), _mOmega(-9999), _GRho(-9999), _GOmega(-9999)
{
}

std::complex<double> CrystalBallRhoOmega::rhoBWVal(){
  return BW_BW::getVal();
}

std::complex<double> CrystalBallRhoOmega::omegaBWVal(){
  return _omegaBW.getVal();
}

std::complex<double> CrystalBallRhoOmega::rhoBWValAtResonance(){
  return BW_BW::getValAtResonance();
}

std::complex<double> CrystalBallRhoOmega::omegaBWValAtResonance(){
  return _omegaBW.getValAtResonance();
}

double CrystalBallRhoOmega::p() const{
  return 1.0;
}

double CrystalBallRhoOmega::q() const{
  return 1.0;
}

double CrystalBallRhoOmega::eps() const{
  return _eps;
}

std::complex<double> CrystalBallRhoOmega::expIBeta() const{
  return complex<double>(cos(_beta), sin(_beta));
}

std::complex<double> CrystalBallRhoOmega::expIPhi() const{
  return complex<double>(cos(_phi), sin(_phi));
}

double CrystalBallRhoOmega::delta() const{
  return _delta;
}

double CrystalBallRhoOmega::mRho() const{
  if(_mRho < 0) _mRho = ParticlePropertiesList::mass(113);
  return _mRho;
}

double CrystalBallRhoOmega::GammaRhoFixed() const{
  if(_GRho < 0) _GRho = ParticlePropertiesList::width(113);
  return _GRho;
}

double CrystalBallRhoOmega::mOmega() const{
  if(_mOmega < 0) _mOmega = ParticlePropertiesList::mass(223);
  return _mOmega;
}

double CrystalBallRhoOmega::GammaOmegaFixed() const{
  if(_GOmega < 0) _GOmega = ParticlePropertiesList::width(223);
  return _GOmega;
}

double CrystalBallRhoOmega::normRho() const{
  return 1;
}

double CrystalBallRhoOmega::normOmega(){
  double m = mOmega()/mRho();
  double G = GammaOmegaFixed()/GammaRhoFixed();

  double r = sqrt(prSq()/_omegaBW.prSq());

  double d = sqrt((1. + _omegaBW.pABSq())/(1. + pABSq()));
  return m*G*r*d;
}

double CrystalBallRhoOmega::omegaToRhoAmpRatio() const{
  return sqrt(_aRatioSq);
}

std::complex<double> CrystalBallRhoOmega::offDiagonalTerm(){
  std::complex<double> num = expIPhi() * delta() * (mRho() + mOmega());
  double dm2 = mOmega()*mOmega() - mRho()*mRho();
  double dmG = mOmega()*GammaOmegaFixed() - mRho()*GofM();
  
  std::complex<double> den(dm2, -dmG);

  return num/den;
}

std::complex<double> CrystalBallRhoOmega::getVal(){
  resetInternals();
  //  cout << "CrystalBallRhoOmega called " << endl;
  std::complex<double> rho = rhoBWVal() * normRho();
  std::complex<double> om  = omegaBWVal()
    * eps() * expIBeta()* normOmega();

  return p()*q()*(rho + omegaToRhoAmpRatio() * om * offDiagonalTerm());
}

std::complex<double> CrystalBallRhoOmega::getValAtResonance(){
  std::complex<double> rho = rhoBWValAtResonance() * normRho();
  std::complex<double> om  = omegaBWValAtResonance()
    * eps() * expIBeta()* normOmega();

  return p()*q()*(rho + omegaToRhoAmpRatio()* om * offDiagonalTerm());
}

DalitzCoordinate CrystalBallRhoOmega::getDalitzCoordinate(double nSigma)const{
  return BW_BW::getDalitzCoordinate(nSigma);
}

void CrystalBallRhoOmega::print(std::ostream& out) const{
  out << "CrystalBallRhoOmega\n\t with rho:\n";
  BW_BW::print(out);
  out << "\n\t and with omega:\n";
  _omegaBW.print(out);
}

void CrystalBallRhoOmega::print(std::ostream& out){
  out << "CrystalBallRhoOmega\n\t with rho:\n";
  BW_BW::print(out);
  out << "\n\t and with omega:\n";
  _omegaBW.print(out);
}
    
MINT::counted_ptr<IGenFct> CrystalBallRhoOmega::generatingFunction() const{
  return BW_BW::generatingFunction();
}

std::ostream& operator<<(std::ostream& out, const CrystalBallRhoOmega& amp){

  amp.print(out);
  return out;
}
