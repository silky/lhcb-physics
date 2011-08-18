#ifndef INTEG_CALCULATOR_HH
#define INTEG_CALCULATOR_HH

#include "Mint/Mojito/DalitzIntegrator/IIntegrationCalculator.h"
#include "Mint/Mojito/FitAmplitude/FitAmpPairList.h"
#include "Mint/Mint/Utils/counted_ptr.h"
#include "Mint/Mojito/BreitWignerMC/DalitzHistoSet.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEvent.h"
#include "Mint/Mojito/FitAmplitude/FitFractionList.h"
#include <iostream>

namespace MINT{
  class Minimiser;
}

class IntegCalculator : public virtual IIntegrationCalculator{
 protected:
  FitAmpPairList _withEff, _noEff;

  FitAmpPairList& withEff() {return _withEff;}
  FitAmpPairList& noEff()   {return _noEff;}

  static std::string dirNameWithEff();
  static std::string dirNameNoEff();
  bool makeDirectories(const std::string& asSubdirOf=".")const;
 public:
  IntegCalculator();
  IntegCalculator(const IntegCalculator& other);
  IntegCalculator(const FitAmpPairList& wEff);
  virtual MINT::counted_ptr<IIntegrationCalculator> 
    clone_IIntegrationCalculator() const;

  const FitAmpPairList& withEff()const {return _withEff;}
  const FitAmpPairList& noEff()const {return _noEff;}

  void setEfficiency(MINT::counted_ptr<IGetDalitzEvent> eff);
  void unsetEfficiency();
  double efficiency(IDalitzEvent* evtPtr);
  
  virtual void addAmps(FitAmplitude* a1, FitAmplitude* a2);
  virtual void addEvent(IDalitzEvent* evtPtr, double weight=1);
  virtual void addEvent(MINT::counted_ptr<IDalitzEvent> evtPtr
			, double weight=1);

  virtual bool add(const IntegCalculator& other);
  virtual bool add(const IntegCalculator* other);
  virtual bool add(const MINT::const_counted_ptr<IntegCalculator>& other);

  virtual bool append(const IntegCalculator& other);
  virtual bool append(const IntegCalculator* other);
  virtual bool append(const MINT::const_counted_ptr<IntegCalculator>& other);

  virtual int numEvents()   const;
  virtual double integral() const;
  virtual double variance() const;

  virtual bool makeAndStoreFractions(MINT::Minimiser* mini=0);
  virtual double getFractionChi2() const;

  virtual DalitzHistoSet histoSet() const;
  virtual void saveEachAmpsHistograms(const std::string& prefix) const;
  virtual void doFinalStats(MINT::Minimiser* mini=0);

  virtual bool save(const std::string& dirname) const;
  virtual bool retrieve(const std::string& commaSeparatedList);
  virtual bool retrieveSingle(const std::string& dirname);

  virtual FitFractionList getFractions() const;

  virtual void print(std::ostream& os=std::cout) const;

  virtual ~IntegCalculator(){}

};

#endif
//
