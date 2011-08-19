#include "Mint/SumOfFitAmpLists.h"

using namespace std;
using namespace MINT;

SumOfFitAmpLists::SumOfFitAmpLists(const DalitzEventPattern& pat)
  : DalitzEventAccess(pat)
{
}
SumOfFitAmpLists::SumOfFitAmpLists(IDalitzEventAccess* eventAccess)
  : DalitzEventAccess(eventAccess)
{
}
SumOfFitAmpLists::SumOfFitAmpLists(IDalitzEventList* events)
  : DalitzEventAccess(events)
{
}

SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list)
  : DalitzEventAccess(list->getEventRecord())
{
  addList(list);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2)
  : DalitzEventAccess(list_1->getEventRecord())
{
  addList(list_1);
  addList(list_2);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_3)
  : DalitzEventAccess(list_1->getEventRecord())
{
  addList(list_1);
  addList(list_2);
  addList(list_3);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_3
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_4)
  : DalitzEventAccess(list_1->getEventRecord())
{
  addList(list_1);
  addList(list_2);
  addList(list_3);
  addList(list_4);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_3
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_4
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_5)
  : DalitzEventAccess(list_1->getEventRecord())
{
  addList(list_1);
  addList(list_2);
  addList(list_3);
  addList(list_4);
  addList(list_5);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_3
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_4
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_5
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_6)
  : DalitzEventAccess(list_1->getEventRecord())
{
  addList(list_1);
  addList(list_2);
  addList(list_3);
  addList(list_4);
  addList(list_5);
  addList(list_6);
}

void SumOfFitAmpLists::addList(const counted_ptr<ILookLikeFitAmpSum>& 
			       list){
  list->setDaddy(this);
  _listOfLists.push_back(list);
}

counted_ptr<IIntegrationCalculator> 
SumOfFitAmpLists::makeIntegrationCalculator(){
  return makeIntegCalculator();
}

counted_ptr<IntegCalculator> 
SumOfFitAmpLists::makeIntegCalculator(){
  counted_ptr<IntegCalculator> ptr(0);
  if(_listOfLists.empty()) return ptr;

  ptr = _listOfLists[0]->makeIntegCalculator();
  if(_listOfLists.size() == 1) return ptr;

  for(unsigned int i=1; i < _listOfLists.size(); i++){
    ptr->append(_listOfLists[i]->makeIntegCalculator());
  }
  return ptr;
}

double SumOfFitAmpLists::RealVal(){
  double sum=0;
  if(_listOfLists.empty()) return 0;

  for(unsigned int i=0; i < _listOfLists.size(); i++){
    sum += _listOfLists[i]->RealVal();
  }
  return sum;
}

DalitzBWBoxSet SumOfFitAmpLists::makeBWBoxes(TRandom* rnd){
  if(_listOfLists.empty()){
    DalitzBWBoxSet emptyBoxes;
    return emptyBoxes;
  }
  DalitzBWBoxSet boxset(_listOfLists[0]->makeBWBoxes(rnd));
  if(_listOfLists.size() == 1) return boxset;

  for(unsigned int i=1; i < _listOfLists.size(); i++){
    DalitzBWBoxSet boxes(_listOfLists[i]->makeBWBoxes(rnd));
    boxset.add(boxes);
  }
  return boxset;
}

MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
SumOfFitAmpLists::makeEventGenerator(TRandom* rnd){
  MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    ptr(new DalitzBWBoxSet(makeBWBoxes(rnd)));
  return ptr;
}

void SumOfFitAmpLists::print(std::ostream& os) const{
  os << "SumOfFitAmpList with " << _listOfLists.size()
     << " components:" << endl;
  for(unsigned int i=0; i < _listOfLists.size(); i++){
    os << " ------ " << endl;
    if(0 != _listOfLists[i]) _listOfLists[i]->print(os);
  }
}

// ------

std::ostream& operator<<(std::ostream& os, const SumOfFitAmpLists& sofal){
  sofal.print(os);
  return os;
}

//===========
//

