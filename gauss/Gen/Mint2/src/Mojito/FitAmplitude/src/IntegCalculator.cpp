
#include "Mint/IntegCalculator.h"
#include "Mint/FitAmpPairList.h"
#include "Mint/Minimiser.h"
#include "Mint/ParsedParameterLine.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <sstream>


using namespace MINT;
using namespace std;

//static
std::string IntegCalculator::dirNameWithEff(){
  return "With_EfficiencyEffects";
}
std::string IntegCalculator::dirNameNoEff(){
  return "Without_EfficiencyEffects";
}

// constructors
IntegCalculator::IntegCalculator()
  : _withEff(), _noEff()
{
}
IntegCalculator::IntegCalculator(const FitAmpPairList& wEff)
  : _withEff(wEff), _noEff(wEff)
{
  _noEff.unsetEfficiency();
}

IntegCalculator::IntegCalculator(const IntegCalculator& other)
  : IIntegrationCalculator()
  , _withEff(other._withEff)
  , _noEff(other._noEff)
{
}
counted_ptr<IIntegrationCalculator> 
IntegCalculator::clone_IIntegrationCalculator() const{
  counted_ptr<IIntegrationCalculator> ptr(new IntegCalculator(*this));
  return ptr;
}


// the rest
void IntegCalculator::setEfficiency(MINT::counted_ptr<IReturnRealForEvent<IDalitzEvent> > eff){
  withEff().setEfficiency(eff);
}
void IntegCalculator::unsetEfficiency(){
  withEff().unsetEfficiency();
}

void IntegCalculator::addAmps(FitAmplitude* a1, FitAmplitude* a2){
  withEff().addAmps(a1, a2);
  noEff().addAmps(a1, a2);
}
void IntegCalculator::addEvent(IDalitzEvent* evtPtr, double weight){
  withEff().addEvent(evtPtr, weight);
  noEff().addEvent(evtPtr, weight);
}
void IntegCalculator::addEvent(MINT::counted_ptr<IDalitzEvent> evtPtr
			       , double weight){
  withEff().addEvent(evtPtr, weight);
  noEff().addEvent(evtPtr, weight);
}
bool IntegCalculator::add(const IntegCalculator& other){
  bool sc=true;
  sc |= withEff().add(other.withEff());
  sc |= noEff().add(other.noEff());
  return sc;
}
bool IntegCalculator::add(const IntegCalculator* other){
  if(0 != other) return add(*other);
  return true;
}
bool IntegCalculator::add(const const_counted_ptr<IntegCalculator>& other){
  if(0 != other) return add(*other);
  return true;
}

bool IntegCalculator::append(const IntegCalculator& other){
  bool sc=true;
  sc |= withEff().append(other.withEff());
  sc |= noEff().append(other.noEff());
  return sc;
}
bool IntegCalculator::append(const IntegCalculator* other){
  if(0 != other) return append(*other);
  return true;
}
bool IntegCalculator::append(const const_counted_ptr<IntegCalculator>& other){
  if(0 != other) return append(*other);
  return true;
}

int IntegCalculator::numEvents()const{
  int Nw = withEff().numEvents();
  int Nn = noEff().numEvents();
  if(Nw != Nn){
    cout << "WARNING in IntegCalculator::numEvents()!"
	 << " number of events in FitAmpPairList with efficiency"
	 << " is not the same as without: "
	 << Nw << " != " << Nn << endl;
    cout << "I'll return the one with efficiency, Nw = " << Nw << endl;
  }
  return Nw;
}
double IntegCalculator::integral()const{
  return withEff().integral();
}
double IntegCalculator::variance() const{
  return withEff().variance();
}

bool IntegCalculator::makeAndStoreFractions(Minimiser* mini){
  bool dbThis=true;
  if(dbThis){
    cout << "Fractions WITH efficiency (for debug only)" << endl;
    withEff().makeAndStoreFractions("wrongFractions_WithEff");
    cout << "\n and now Fractions WITHOUT efficiency"
	 << " (i.e. what you really want):" << endl;
  }
  return noEff().makeAndStoreFractions(mini);
}
double IntegCalculator::getFractionChi2() const{
  return noEff().getFractionChi2();
}

DalitzHistoSet IntegCalculator::histoSet() const{
  return withEff().histoSet();
}
void IntegCalculator::saveEachAmpsHistograms(const std::string& prefix) const{
  return noEff().saveEachAmpsHistograms(prefix);
}

std::vector<DalitzHistoSet> IntegCalculator::GetEachAmpsHistograms(){
  return noEff().GetEachAmpsHistograms();
}

void IntegCalculator::doFinalStats(Minimiser* mini){
  makeAndStoreFractions(mini);
}

bool IntegCalculator::makeDirectories(const std::string& asSubdirOf)const{
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
  zeroForSuccess |= mkdir( (asSubdirOf+"/"+dirNameWithEff() ).c_str(), mode );
  zeroForSuccess |= mkdir( (asSubdirOf+"/"+dirNameNoEff() ).c_str(), mode );
  return (0 == zeroForSuccess);
}


bool IntegCalculator::save(const std::string& dirname) const{
  bool sc=true;
  makeDirectories(dirname);
  sc &= withEff().save(dirname + "/" + dirNameWithEff());
  sc &= noEff().save(dirname + "/" + dirNameNoEff());
  return sc;
}
bool IntegCalculator::retrieve(const std::string& commaSeparatedList){
  bool dbThis=true;
  bool sc=true;
  std::stringstream is(commaSeparatedList);
  std::string dirname;
  int counter=0;
  while(getline(is, dirname, ',')){
    dirname = ParsedParameterLine::removeLeadingAndTrailingBlanks(dirname);
    if("" == dirname) continue;
    if(dbThis){
      cout << "IntegCalculator::retrieve: reading file: "
	   << dirname
	   << endl;
    }
    if(0 == counter){
      this->retrieveSingle(dirname);
    }else{
      IntegCalculator n(*this);
      sc &= n.retrieveSingle(dirname);
      this->add(n);
    }
    counter++;
  }
  return sc;
}
bool IntegCalculator::retrieveSingle(const std::string& dirname){
  bool sc=true;
  sc &= withEff().retrieve(dirname + "/" + dirNameWithEff());
  sc &= noEff().retrieve(dirname + "/" + dirNameNoEff());
  return true;
}

FitFractionList IntegCalculator::getFractions() const{
  return withEff().getFractions();
}

void IntegCalculator::print(ostream& os) const{
  os << "IntegCalculator"
     << "my FitAmpPairList with eff:\n" << withEff()
     << "\n the integral: " << integral();
}
