// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/FitAmpPairList.h"

#include "Mint/IDalitzEvent.h"
#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpPairFitCovariance.h"

#include "Mint/Minimiser.h"

#include "Mint/Utils.h"

//#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <algorithm>
#include <vector>

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace MINT;

FitAmpPairList::FitAmpPairList()
  : std::vector<FitAmpPair>()
  , _Nevents(0)
  , _sum(0)
  , _sumsq(0)
  , _psSum(0)
  , _psSumSq(0)
  , _cov(this)
  , _efficiency(0)
{
}
FitAmpPairList::FitAmpPairList(const FitAmpPairList& other)
  : IIntegrationCalculator()
  , std::vector<FitAmpPair>(other)
  , _Nevents(other._Nevents)
  , _sum(other._sum)
  , _sumsq(other._sumsq)
  , _psSum(other._psSum)
  , _psSumSq(other._psSumSq)
  , _cov(other._cov, this)
  , _efficiency(other._efficiency)
{
}
counted_ptr<IIntegrationCalculator> 
FitAmpPairList::clone_IIntegrationCalculator() const{
  counted_ptr<IIntegrationCalculator> ptr(new FitAmpPairList(*this));
  return ptr;
}

void FitAmpPairList::addAmps(FitAmplitude* a1, FitAmplitude* a2){
  FitAmpPair amps(a1, a2);
  this->push_back(amps);
}
void FitAmpPairList::addEvent(IDalitzEvent* evtPtr, double weight){
  bool dbThis=false;
  static unsigned int nZeroPs=0;
  _Nevents++;
  double x=0;
  if(0 == evtPtr) return;
  double ps = evtPtr->phaseSpace();

  if(ps <= 0.0){
	  if (!(nZeroPs%1000))
	  {
		cout << "WARNING in FitAmpPairList::addToHistograms"
		 << " " << ++nZeroPs << " th"
		 << " event with phase space = " << ps << endl;
		return; // should not happen.
	  }
  }

  for(unsigned int i=0; i< this->size(); i++){
    //    x += (*this)[i].add(evtPtr, weight*efficiency(evtPtr));
    x += (*this)[i].add(evtPtr, weight, efficiency(evtPtr));
  }
  if(dbThis) cout << "FitAmpPairList::addEvent(): adding event" << endl;
  //if(_cov.isValid()) 
  _cov.addLastEventFromList();

  _sum   += x;
  _sumsq += x*x;
  
  //if(0 == evtPtr) return;
  //double ps = evtPtr->phaseSpace();
  _psSum   += ps;
  _psSumSq += ps*ps;

  return;
}
void FitAmpPairList::addEvent(counted_ptr<IDalitzEvent> evtPtr, double weight){
  return addEvent(evtPtr.get(), weight);
}

bool FitAmpPairList::isCompatibleWith(const FitAmpPairList& otherList) const{
  if(this->size() != otherList.size()) return false;
  for(unsigned int i=0; i < this->size(); i++){
    if((*this)[i].name() != otherList[i].name()) return false;
  }
  return true;
}
bool FitAmpPairList::add(const FitAmpPairList* otherListPtr){
  if(0 != otherListPtr){
    return add(*otherListPtr);
  }
  return true;
}
bool FitAmpPairList::add(const_counted_ptr<FitAmpPairList> otherListPtr){
  if(0 != otherListPtr){
    return add(*otherListPtr);
  }
  return true;
}
bool FitAmpPairList::add(const FitAmpPairList& otherList){
  if(! isCompatibleWith(otherList)){
    cout << "ERROR in FitAmpPairList::add "
	 << "trying to add incompatible lists"
	 << endl;
    return false;
  }

  _Nevents  += otherList._Nevents;
  _sum      += otherList._sum;
  _sumsq    += otherList._sumsq;
  _psSum    += otherList._psSum;
  _psSumSq  += otherList._psSumSq;

  _cov      += otherList._cov;

  for(unsigned int i=0; i< this->size(); i++){
    (*this)[i] += otherList[i];
  }
  
  return true;

}

bool FitAmpPairList::append(MINT::const_counted_ptr<FitAmpPairList> otherListPtr){
  return append(*otherListPtr);
}
bool FitAmpPairList::append(const FitAmpPairList* otherListPtr){
  return append(*otherListPtr);
}
bool FitAmpPairList::append(const FitAmpPairList& otherList){
  // WARNING: this will reset everything to zero.

  for(unsigned int i=0; i< otherList.size(); i++){
    this->push_back(otherList[i]);
  }
  this->reset();

  return true;

}
bool FitAmpPairList::reset(){
  _Nevents=0;
  _sum=0;
  _sumsq=0;
  _psSum=0;
  _psSumSq=0;
  _cov.reset();
  _singleAmpFractions.clear();
  _interferenceFractions.clear();

  return true;
}
int FitAmpPairList::numEvents() const{
  return _Nevents;
}

double FitAmpPairList::integral() const{
  double sum = 0;
  for(unsigned int i=0; i< this->size(); i++){
    sum += (*this)[i].integral();
  }
  return sum;
}

double FitAmpPairList::phaseSpaceIntegral()const{
  // only for debug
  if(_Nevents <= 0) return -9999;
  return _psSum/((double) _Nevents);
}

double FitAmpPairList::variance() const{
  bool dbThis=false;
  static int printouts=0;
  static bool printedWarning=false;
  if(_Nevents <=0)return 0;

  if(_cov.isValid()){
    if(dbThis && printouts < 100){
      printouts++;
      cout << "FitAmpPairList::variance() with "
	   << _Nevents << "(" << _cov.Nevents() << ") events. Compare: "
	   << " old: " << oldVariance()
	   << " sum: " << sumOfVariances()
	   << ", new: " << _cov.getIntegralVariance()
	   << endl;
    }
    if(_Nevents < 10000) return oldVariance();
    double var = _cov.getIntegralVariance();
    if(var < 1.e-6 * oldVariance()){
      if(dbThis) cout << "returning OLD variance " << var << endl;
      // (if var is tiny, it usually means something went wrong)
      return oldVariance(); 
    }
    if(dbThis) cout << "returning new variance " << var << endl;
    return var;
  }else{
    if(! printedWarning){
      cout << "WARNING in FitAmpPairList::variance()"
	   << " ignoring correlations because _cov is invalid."
	   << " (I'll print this only once)"
	   << endl;
      printedWarning=true;
    }
    return sumOfVariances();
  }
}

double FitAmpPairList::sumOfVariances() const{
  double sum=0;
  for(unsigned int i=0; i < this->size(); i++){    
    sum += (*this)[i].variance();
  }
  return sum;
}
double FitAmpPairList::oldVariance() const{
  // this does not (yet) take into account
  // that the variance changes with the
  // fit paramters.
  bool dbThis=false;

  double dN = (double) _Nevents;
  double mean   = _sum/dN;
  double meansq = _sumsq/dN;
  double v = (meansq - mean*mean)/dN;
  double sf=1.0;
  if(0 != integral()){
    sf = integral()/mean;
    v        *= sf*sf;
    // this construct allows for a scaling factor (such as _weightSum)
    // in the integral.
  }
  if(dbThis){
    cout << "FitAmpPairList::oldVariance() "
	 << " N = " << dN
	 << " sum = " << _sum
	 << " sumsq = " << _sumsq
	 << " sf " << sf
	 << " v " << v << endl;
  }
  return v;
}
double FitAmpPairList::efficiency(IDalitzEvent* evtPtr){
  if(0 == _efficiency) return 1;
  _efficiency->setEvent(evtPtr);
  double ef = _efficiency->RealVal();
  _efficiency->resetEventRecord();
  if(ef < 0) return 0;
  return ef;
}
void FitAmpPairList::setEfficiency(counted_ptr<IGetDalitzEvent> eff){
  _efficiency=eff;
}
void FitAmpPairList::unsetEfficiency(){
  _efficiency=counted_ptr<IGetDalitzEvent>(0);
}
DalitzHistoSet FitAmpPairList::histoSet() const{
  DalitzHistoSet sum;
  for(unsigned int i=0; i< this->size(); i++){
    sum += (*this)[i].histoSet();
  }
  sum /= integral();
  //  if(_Nevents > 0) sum /= (double) _Nevents;
  // above two lines normalise this to 1.

  return sum;
}

void FitAmpPairList::saveEachAmpsHistograms(const std::string& prefix) const{
  DalitzHistoSet sum;
  int counter=0;
  for(unsigned int i=0; i< this->size(); i++){
    if((*this)[i].isSingleAmp()){
      counter++;
      std::string name = prefix + "_" + anythingToString(counter) + ".root";
      DalitzHistoSet hs((*this)[i].histoSet());
      std::string title = (*this)[i].amp1()->name();
      hs.setTitle(title);
      cout << "FitAmpPairList::saveEachAmpsHistograms: "
	   << "saving " << title << " as " << name << endl;
      hs.save(name);
    }
  }
  return;
}

void FitAmpPairList::doFinalStats(Minimiser* mini){
  bool dbThis=true;
  if(dbThis) cout << "FitAmpPairList::doFinalStats() called" << endl;
  makeAndStoreFractions(mini);
}

bool FitAmpPairList::makeAndStoreFractions(const std::string& fname
					   , Minimiser* mini
					   ){
  bool dbThis=true;
  if(this->empty()) return 0;
  counted_ptr<FitAmpPairFitCovariance> fcov(0);
  if(0 != mini){
    if(mini->parSet() != (*this)[0].amp1()->FitAmpPhase().p1().parSet()){
      cout << "ERROR in FitAmpPairList::makeAndStoreFractions"
	   << "\n Minuit parameter set and fit parameters incompatible"
	   << endl;
    }else{
      counted_ptr<FitAmpPairFitCovariance> 
	fc(new FitAmpPairFitCovariance(this, mini->covMatrixFull()));
      fcov = fc;
    }
  }
  bool printFitErrors = (fcov != 0 && fcov->isValid());

  cout << "\n============================================"
       << "============================================"
       << "\n        Amplitude Fractions";
  if(0 != fcov) cout << " +/- fit error ";
  cout << " +/- integration error "
       << endl;
  double norm = integral();
  if(norm <= 0){
    cout << "ERROR in FitAmpPairList::makeAndStoreFractions()"
	 << " integral = " << integral()
	 << " won't do fractions."
	 << endl;
    return false;
  }

  for(unsigned int i=0; i < this->size(); i++){    
    double frac = (*this)[i].integral()/norm;
    string name;
    if((*this)[i].isSingleAmp()){
      name = (*this)[i].amp1()->name();
    }else{
      name = (*this)[i].name();
    }
    FitFraction f(name, frac);
    if(printFitErrors){
      f.sigmaFit() = fcov->getFractionError(i);
    }
    if(_cov.isValid()){ 
      f.sigmaInteg() = _cov.getFractionError(i);
    }

    if((*this)[i].isSingleAmp()){
      (*this)[i].amp1()->setFraction(frac);
      _singleAmpFractions.add(f);
    }else{
      _interferenceFractions.add(f);
    }
  }


  if(dbThis)cout << "filled FitFractionLists" << endl;
  if(printFitErrors){
    _singleAmpFractions.setSumFitError(fcov->getFractionSumError());
    _interferenceFractions.setSumFitError(fcov->getInterferenceFracSumError());
  }
  if(_cov.isValid()){
    _singleAmpFractions.setSumIntegError(_cov.getFractionSumError());
  }

  if(dbThis) cout << "... now including errors" << endl;
  if(dbThis)cout  << " sorting" << endl;
  _interferenceFractions.sortByMagnitudeDecending();
  if(dbThis)cout << "sorted" << endl;

  cout <<   "================================================="
       << "\n================================================="
       << "\n FRACTIONS:"
       << "\n ^^^^^^^^^^" << endl;
  cout << _singleAmpFractions << endl;
  cout <<   "================================================="
       << "\n================================================="
       << "\n Interference terms (sorted by size)"
       << "\n ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;
  cout << _interferenceFractions << endl;
  cout <<   "================================================="
       << "\n=================================================" << endl;

  cout << "\n\n\t X-check: total sum of all terms"
       << " (amplitude fractions + interference terms): "
       << _singleAmpFractions.sum().frac() 
    + _interferenceFractions.sum().frac();
  cout << endl;
  ofstream os(fname.c_str());
  if(os){
    os << _singleAmpFractions << endl;
    os << _interferenceFractions << endl;
    os.close();
  }
  
  return true;
}

double FitAmpPairList::getFractionChi2() const{
  bool ErrorProportionalToSqrtTarget = true;
  double norm = integral();
  if(norm <= 0){
    cout << "ERROR in FitAmpPairList::getFractionChi2()"
	 << " integral = " << integral()
	 << " won't do fractions. Returning " << 9999.0e30
	 << endl;
    return 9999.0e30;
  }

  double canonicalError=0.001;

  int counter=0;
  double sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    if((*this)[i].isSingleAmp()){
      counter++;
      double frac = (*this)[i].integral()/norm;
      double target_f = (*this)[i].amp1()->getFraction();
      if(i < 10){
	cout << "(" << frac << " - " << target_f << ")" << endl;
      }
      double dfs=(frac - target_f)/canonicalError;
      if(ErrorProportionalToSqrtTarget && target_f > 0) dfs /= sqrt(0.01*target_f);
      sum += dfs*dfs;
    }
  }
  return sum;
}

std::string FitAmpPairList::dirName() const{
  return "FitAmpPairList";
}
bool FitAmpPairList::save(const std::string& asSubdirOf) const{
  bool sc=true;
  makeDirectory(asSubdirOf);
  ofstream os((asSubdirOf +"/"+ dirName() + "values.txt").c_str());
  os << "N " << _Nevents << endl;
  os << "sum " << setprecision(20) << _sum << endl;
  os << "sumsq " << setprecision(20) << _sumsq << endl;
  os << "psSum " << setprecision(20) << _psSum << endl;
  os << "psSumSq " << setprecision(20) << _psSumSq << endl;
  os.close();

  for(unsigned int i=0; i < this->size(); i++){
    sc &= (*this)[i].save(asSubdirOf +"/"+ dirName());
  }
  if(_cov.isValid()){
    _cov.save(asSubdirOf + "/" + dirName());
  }else{
    cout << "WARNING IN FitAmpPairList::save(" << asSubdirOf
	 << "): invalid _cov" << endl;
  }

  return sc;
}
bool FitAmpPairList::retrieve(const std::string& asSubdirOf){
  bool sc=true;
  bool verbose=true;
  if(verbose){
    cout << "FitAmpPairList::retrieve: reading from "
	 << asSubdirOf << endl; 
  }

  if(access(asSubdirOf.c_str(), 0) != 0){
    if(verbose) cout << "FitAmpPairList::retrieve: " << asSubdirOf
		     << " does not exist" << endl;
    return false;
  }

  ifstream is((asSubdirOf +"/"+ dirName() + "values.txt").c_str());
  std::string dummy;
  is >> dummy >> _Nevents;
  is >> dummy >> _sum;
  is >> dummy >> _sumsq;
  is >> dummy >> _psSum;
  is >> dummy >> _psSumSq;
  is.close();
  for(unsigned int i=0; i < this->size(); i++){
    sc &= (*this)[i].retrieve(asSubdirOf +"/"+ dirName());
  }
  _cov.retrieve(asSubdirOf + "/" + dirName());
  return sc;
}

bool FitAmpPairList::makeDirectory(const std::string& asSubdirOf)const{
  /*
    A mode is created from or'd permission bit masks defined
     in <sys/stat.h>:
           #define S_IRWXU 0000700     RWX mask for owner 
           #define S_IRUSR 0000400     R for owner 
           #define S_IWUSR 0000200     W for owner 
           #define S_IXUSR 0000100     X for owner 

           #define S_IRWXG 0000070     RWX mask for group 
           #define S_IRGRP 0000040     R for group 
           #define S_IWGRP 0000020     W for group 
           #define S_IXGRP 0000010     X for group 

           #define S_IRWXO 0000007     RWX mask for other 
           #define S_IROTH 0000004     R for other 
           #define S_IWOTH 0000002     W for other 
           #define S_IXOTH 0000001     X for other 

           #define S_ISUID 0004000     set user id on execution 
           #define S_ISGID 0002000     set group id on execution 
           #define S_ISVTX 0001000     save swapped text even after use
   */

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO 
              | S_ISUID | S_ISGID; 
  // see above for meaning. I want everybody to be allowed to read/write/exec.
  // Not sure about the last two bits.

  int zeroForSuccess = 0;
  zeroForSuccess |= mkdir( (asSubdirOf ).c_str(), mode );
  zeroForSuccess |= mkdir( (asSubdirOf + "/" + dirName() ).c_str(), mode );
  return (0 == zeroForSuccess);
}

void FitAmpPairList::print(std::ostream& os) const{
  os << "FitAmpPairList with " << this->size() << " FitAmpPairs:";
  for(unsigned int i=0; i < this->size(); i++){
    os << "\n\t" << i << ") " << (*this)[i];
  }
}

FitAmpPairList& FitAmpPairList::operator+=(const FitAmpPairList& other){
  this->add(other);
  return *this;
}
FitAmpPairList FitAmpPairList::operator+(const FitAmpPairList& other) const{
  FitAmpPairList returnVal(*this);
  returnVal.add(other);
  return returnVal;
}

std::ostream& operator<<(std::ostream& os, const FitAmpPairList& fap){
  fap.print(os);
  return os;
}

/*
double FitAmpPairList::variance() const{
  double sum = 0;
  for(unsigned int i=0; i< this->size(); i++){
    sum += (*this)[i].variance();
  }
  return sum;
}
*/

/*
double FitAmpPairList::variance() const{
  double sumReSquared = 0;
  double sumImSquared = 0;
  double sumImRe      = 0;

  for(unsigned int i=0; i< this->size(); i++){
    sumReSquared += (*this)[i].ReSquared();
    sumImSquared += (*this)[i].ImSquared();
    sumImRe      += (*this)[i].ImRe();
  }
  double sumsq = sumReSquared + sumImSquared - 2.0*sumImRe;

  double meansq = sumsq/((double)_Nevents);
  
  double mean = integral();

  double var = (meansq - mean*mean)/((double)_Nevents);

  return var;
}
*/

//
