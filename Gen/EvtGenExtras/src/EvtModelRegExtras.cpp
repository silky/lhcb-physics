#include "EvtGenModels/EvtModelRegExtras.hh"

#include "EvtGenModels/EvtBToKStarllDurham07.hh"
#include "EvtGenModels/EvtbTosllMS.hh"
#include "EvtGenModels/Evtbs2llGammaMNT.hh"
#include "EvtGenModels/EvtDTohhhh.hh"
#include "EvtGenModels/EvtbsToLLLL.hh"

//we use auto_ptr here to show explicit transfer of ownership
std::auto_ptr<const EvtModelList> EvtModelRegExtras::getModels(){

	EvtModelList* models = new EvtModelList();
	models->push_back(new EvtBToKStarllDurham07);
        models -> push_back( new EvtbTosllMS() );
        models -> push_back( new Evtbs2llGammaMNT() );
        models -> push_back( new EvtDTohhhh() );
        models -> push_back( new EvtbsToLLLL() );
	return std::auto_ptr<const EvtModelList>(models);
	
}

