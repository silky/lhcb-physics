/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
// GiGa 
#include "GiGa/GiGaEventActionBase.h"

/** implementation fo class GiGaEventActionBase
 * 
 *  @author Vanya Belyaev 
 */

/// ===========================================================================
/// ===========================================================================
GiGaEventActionBase::GiGaEventActionBase( const std::string& nick , 
                                          ISvcLocator* svc )
  : GiGaBase          ( nick , svc ) 
{};

/// ===========================================================================
/// virtual destructor
/// ===========================================================================
GiGaEventActionBase::~GiGaEventActionBase(){};


/// ===========================================================================
/// ===========================================================================
StatusCode GiGaEventActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaEventActionBase::finalize() { return GiGaBase::finalize();  };

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaEventActionBase::queryInterface( const InterfaceID& iid , 
                                                void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if   ( IGiGaEventAction::interfaceID() == iid ) 
    { *ppI = static_cast<IGiGaEventAction*>        ( this ) ; } 
  else { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};

/// ===========================================================================
/// ===========================================================================
void GiGaEventActionBase::BeginOfEventAction ( const G4Event* /* event */ ) {};

/// ===========================================================================
/// ===========================================================================
void GiGaEventActionBase::EndOfEventAction   ( const G4Event* /* event */ ) {};


/// ===========================================================================












