#ifndef CRYSTALBALLRHOOMEGA_LINESHAPE_HH
#define CRYSTALBALLRHOOMEGA_LINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/Mojito/Lineshapes/ILineshape.h"
#include "Mint/Mojito/Lineshapes/BW_BW.h"
#include "Mint/Mojito/Lineshapes/BW_BW_DifferentMother.h"
#include "Mint/Mint/NamedParameter/NamedParameter.h"

#include <complex>

class CrystalBallRhoOmega : public BW_BW, virtual public ILineshape{
  // Implemented as a normal Breit Wigner K*(1430)->K+ pi-
  // with an extra bit of "Background", parameterised
  // as in the Lass paper.
  // Ignore the bit about the K0 eta' in the CRYSTALBALLRHOOMEGA paper
  // (wonder whether that's correct)?
 protected:

  BW_BW_DifferentMother _omegaBW;

  MINT::NamedParameter<double> _eps, _beta, _phi, _delta, _aRatioSq;
  std::complex<double> rhoBWVal();
  std::complex<double> omegaBWVal();
  std::complex<double> rhoBWValAtResonance();
  std::complex<double> omegaBWValAtResonance();

  mutable double _mRho, _mOmega, _GRho, _GOmega;
  
  double p() const;
  double q() const;
  double eps() const;
  std::complex<double> expIBeta() const;
  std::complex<double> expIPhi() const;
  double delta() const;
  double mRho() const;
  double mOmega() const;
  double GammaRhoFixed() const;
  double GammaOmegaFixed() const;
  double normRho() const;
  double normOmega();
  std::complex<double> offDiagonalTerm();
  double omegaToRhoAmpRatio() const;
 public:
  
  CrystalBallRhoOmega( const AssociatedDecayTree& tree
		       , IDalitzEventAccess* events);

  virtual std::complex<double> getVal();
  virtual std::complex<double> getValAtResonance();
  virtual DalitzCoordinate getDalitzCoordinate(double nSigma=3)const;
  virtual void print(std::ostream& out = std::cout) const;
  virtual void print(std::ostream& out = std::cout) ;

  virtual MINT::counted_ptr<IGenFct> generatingFunction() const;

  virtual std::string name() const{
    return "CrystallBarrel("+_theDecay.oneLiner() +")";
  }
  virtual ~CrystalBallRhoOmega(){}
};

#endif
//
