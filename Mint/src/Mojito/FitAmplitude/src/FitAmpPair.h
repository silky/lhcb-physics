#ifndef FIT_AMP_PAIR_HH
#define FIT_AMP_PAIR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include "counted_ptr.h"
#include <complex>
#include <string>
#include <map>
#include <iostream>

#include "DalitzHistoSet.h"
#include "FitAmplitude.h"

//class FitAmplitude;
class IDalitzEvent;

class FitAmpPair{

  FitAmplitude* _a1;
  FitAmplitude* _a2;

  std::complex<double> _sum;
  std::string _sumName;

  std::complex<double> _sumsq;
  std::string _sumSqName;

  long int _Nevents;
  std::string _NName;

  double _weightSum;
  std::string _weightSumName;

  DalitzHistoSet _hsRe;
  DalitzHistoSet _hsIm;

  std::string _name;
  std::string _dirName;

  std::complex<double> _lastEntry;

  std::complex<double> ampValue(IDalitzEvent* evtPtr);
  int oneOrTwo() const;

  const DalitzHistoSet& histosRe() const{return _hsRe;}
  const DalitzHistoSet& histosIm() const{return _hsIm;}
  DalitzHistoSet& histosRe(){return _hsRe;}
  DalitzHistoSet& histosIm(){return _hsIm;}
  /*
  double ReSquared() const;
  double ImSquared() const;
  double ImRe() const;
  */

  void addToHistograms(IDalitzEvent* evtPtr, const std::complex<double>& c);

  const std::string& makeName();
  const std::string& makeDirName();

  bool makeDirectory(const std::string& asSubdirOf = ".")const;
  std::string valueFileName(const std::string& asSubdirOf) const;
  std::string histoReFileName(const std::string& asSubdirOf) const;
  std::string histoImFileName(const std::string& asSubdirOf) const;
  bool saveValues(const std::string& asSubdirOf = ".") const;
  bool retrieveValues(const std::string& fromDirectory = ".");
  bool saveHistos(const std::string& asSubdirOf = ".") const;
  bool retrieveHistos(const std::string& asSubdirOf = ".");

  bool isCompatibleWith(const FitAmpPair& other) const;
 public:
  FitAmpPair(FitAmplitude* a1=0, FitAmplitude* a2=0);
  FitAmpPair(const FitAmpPair& other);

  bool add(const FitAmpPair& other);

  const std::string& name() const;
  const std::string& name();
  const std::string& dirName()const;
  const std::string& dirName();

  bool save(const std::string& asSubdirOf=".") const;
  bool retrieve(const std::string& asSubdirOf=".");

  bool isSingleAmp() const;

  double add(IDalitzEvent* evt, double weight=1);
  double add(MINT::counted_ptr<IDalitzEvent> evt, double weight=1);

  // integral of things that depend on position in Dalitz space:
  std::complex<double> valNoFitPars() const;
  // factors that that don't
  std::complex<double> fitParValue() const;
  // product of the above
  std::complex<double> complexVal() const;
  // real part of the above.
  double integral() const;
  double variance() const;
  double weightSum() const;
  long int N() const;
  std::complex<double> lastEntry() const;

  DalitzHistoSet histoSet() const;

  FitAmplitude* amp1(){return _a1;}
  FitAmplitude* amp2(){return _a2;}
  const FitAmplitude* amp1() const{return _a1;}
  const FitAmplitude* amp2() const{return _a2;}
  
  virtual void print(std::ostream& os=std::cout) const;
  
  virtual ~FitAmpPair(){}

  FitAmpPair& operator+=(const FitAmpPair& other);
  FitAmpPair operator+(const FitAmpPair& other) const;

  friend class FitAmpPairCovariance;
};

class lessByFitAmpPairIntegral{
 public:
  bool operator()(const FitAmpPair& a, const FitAmpPair& b) const;
};

class lessByFitAmpPairIntegral_ptr{
 public:
  bool operator()(const FitAmpPair* a, const FitAmpPair* b) const;
};


std::ostream& operator<<(std::ostream& os, const FitAmpPair& fap);

class lessByFitAmpPairIntegral_ptr_int_pair{
 public:
  bool operator()(const std::pair<FitAmpPair*, int>& a, const std::pair<FitAmpPair*, int>& b) const;
};


#endif

