// ============================================================================
// Include files 
// ============================================================================
#include <unistd.h>
#include <sys/wait.h>
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Local 
// ============================================================================
#include "PowhegProductionWbb.h"


// ============================================================================
/** @class PowhegProductionWbb PowhegProductionWbb.cpp
 */

//=============================================================================
// Standard constructor
//=============================================================================
  PowhegProductionWbb::PowhegProductionWbb
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) 
    : PowhegProduction ( type , name , parent ) 
  {
    declareInterface< IProductionTool >( this ) ;
    //InputFile() = "pwgevents.lhe";
  }
  
//=============================================================================
// Destructor 
//=============================================================================
  PowhegProductionWbb::~PowhegProductionWbb() {}
  
// =========================================================================
// Initialize method
//=============================================================================
StatusCode PowhegProductionWbb::initialize() {
  powhegInitialize ("Wbb");
  return PowhegProduction::initialize() ;
}

//=============================================================================
//Finalize
//=============================================================================
StatusCode PowhegProductionWbb::finalize() {
  
  return PowhegProduction::finalize () ;
  
}

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( PowhegProductionWbb )
// ============================================================================


// The END 
// ============================================================================
