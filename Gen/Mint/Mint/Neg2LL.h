#ifndef NEG2LL_HH
#define NEG2LL_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/Minimisable.h"
#include "Mint/IPdf.h"
#include "Mint/IEventList.h"
#include "Mint/counted_ptr.h"

#include <iostream>

/*
  
  this implements:
  class Neg2LL: public Minimisable

  in a way that its constructor

  Neg2LL( pdf, eventlist, MinuitParameterSet mps=0)

  swallows any type of pdf or eventlist as long as:

  o pdf implements IPdf<EVENT_TYPE>
  o eventlist as the method size() and the operator [unsigned int i]
  o eventList must return an event type that pdf can digest.

  This specific setup below with Neg2LLClass (defined first, don't
  use) and Neg2LL (defined 2nd) is chosen such that you can declare
  Neg2LL w/o the need to specify template arguments, i.e. like this:


   Neg2LL fcnP(BpPdf, BpEventList);

  It will take any PDF and Eventlist, as described above.

 */


namespace MINT{
  class Neg2LL;

  template<typename PDF_TYPE
    , typename EVENTLIST_TYPE>
    class Neg2LLClass : public Minimisable{
    friend class Neg2LL;

    int _NCalls;
  protected:
    //IPdf<EVENT_TYPE>* _pdf;
    //IEventList<EVENT_TYPE>* _eventList;
    PDF_TYPE & _pdf;
    EVENTLIST_TYPE & _eventList;
    
    // Putting the constructors into "protected" means that nobody can 
    // use this class except for Neg2LL. That's the idea. Use Neg2LL,
    // not this, this is just a helper class.

  Neg2LLClass( PDF_TYPE & pdf // IPdf<EVENT_TYPE>* pdf
	       , EVENTLIST_TYPE & erptr // IEventList<EVENT_TYPE>* erptr
	       , MinuitParameterSet* mps = 0)
    : Minimisable(mps)
      , _NCalls(0)
      , _pdf(pdf)
      , _eventList(erptr)
    { 
      init();
    }
  Neg2LLClass(const Neg2LLClass<PDF_TYPE, EVENTLIST_TYPE>& other)
    : Minimisable(other)
      , _NCalls(other._NCalls)
      , _pdf(other._pdf)
      , _eventList(other.erptr)
      {
	init();
      }
    
  public:
    /*
      Neg2LLClass( IPdf<EVENT_TYPE>* pdf
      , MinuitParameterSet* mps = 0)
      : Minimisable(mps)
      , _NCalls(0)
      , _pdf(pdf)
      , _eventList(0)
      { 
      init();
      }
    */
    // Neg2LLClass(const Neg2LLClass<EVENT_TYPE>& other)
    
    virtual void beginFit(){
      _pdf.beginFit();
    };
    virtual void parametersChanged(){
      _pdf.parametersChanged();
    };
    
    virtual void endFit(){
      _pdf.endFit();
    };
    
    bool init(){
      // just a place holder
      bool dbThis=true;
      if(dbThis){
	std::cout << "Neg2LLClass: I got initialised with an event list with "
		  << _eventList.size() << " events." << std::endl;
	std::cout << "Neg2LLClass: with pointer: " << &_eventList << std::endl;
	if(_eventList.size() > 0){
	  std::cout << "The first one is:\n"
		    << _eventList[0]
		    << std::endl;
	}
      }
      return true;
    }
    
    virtual double logPdf(unsigned int evtNumber){
      bool dbThis = true;
      if(dbThis) {
	std::cout << "about to call _pdf.getVal() " 
		  << "for event:\n " << _eventList[evtNumber]
		  << std::endl;
      }
      double valPdf = _pdf.getVal(_eventList[evtNumber]);
      if(dbThis) std::cout << " that worked! the value is " 
			   << valPdf << std::endl;
      if(valPdf <= 0){
	if(dbThis) std::cout << "ERROR in Neg2LLClass::logPdf()"
			     << " the pdf is " << valPdf
			     << " which is <= 0." 
			     << std::endl;
	return -9999.e20 * (1.0 + fabs(valPdf));
      }
      return log(valPdf);
    }
    virtual double getVal(){
      _NCalls ++;
      bool verbose=false;
      bool dbThis=false;
      int printFreq = 100;
      printFreq =    100;
      if(_NCalls >   500) printFreq =   1000;
      if(_NCalls > 10000) printFreq =   5000;
      
      bool printout = (0 == _NCalls%printFreq) || _NCalls <= 1;
      printout |= dbThis;
      verbose  |= dbThis;
      
      double sum=0;
      
      if(verbose && printout){
	std::cout << "Neg2LLClass::getVal after " << _NCalls << " calls."
		  << " pdf ptr is non zero - that's a start." 
		  << std::endl;
      }
      unsigned int counter=0;
      for(counter=0; counter < _eventList.size(); ++counter){
	//EVENT_TYPE & evt( (*_eventList)[counter] );
	// sum += logPdf((*_eventList)[counter]);
	sum += logPdf(counter);
      }
      
      if(printout){
	std::cout << "Neg2LLClass::getVal after " << _NCalls << " calls."
		  << "for " << counter
		  << " events, I'm about to return " 
		  << -2.0*sum << std::endl;
      }
      return -2.* sum;
    }
    
    virtual double getNewVal(){
      // forces re-calculation after parameter change
      parametersChanged();
      return getVal();
    }
    
  };

  
  // ===================================

  class Neg2LL: public Minimisable{
  protected:
    counted_ptr<IMinimisable> _imini;
    
  public:
    template <typename PDF_TYPE, typename EVENTLIST_TYPE>
      Neg2LL(PDF_TYPE & pdf, EVENTLIST_TYPE & evtList
	     , MinuitParameterSet* mps=0)
      : _imini(new Neg2LLClass <PDF_TYPE, EVENTLIST_TYPE>(pdf, evtList, mps))
      {
      }

    Neg2LL(const Neg2LL& other): _imini(other._imini){  
    }

    virtual MinuitParameterSet* getParSet(){ return _imini->getParSet();}
    
    virtual void beginFit(){_imini->beginFit();}
    virtual void parametersChanged(){_imini->parametersChanged();}
    virtual void endFit(){_imini->endFit();}
    
    virtual double getVal(){return _imini->getVal();}
    virtual double getNewVal(){return _imini->getNewVal();}
    virtual ~Neg2LL(){}

  };

} // namespace MINT

#endif
//
