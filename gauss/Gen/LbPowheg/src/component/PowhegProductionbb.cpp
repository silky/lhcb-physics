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
#include "PowhegProductionbb.h"


// ============================================================================
/** @class PowhegProductionbb PowhegProductionbb.cpp
 */

//=============================================================================
// Standard constructor
//=============================================================================
  PowhegProductionbb::PowhegProductionbb
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
  PowhegProductionbb::~PowhegProductionbb() {}
  
// =========================================================================
// Initialize method
//=============================================================================
StatusCode PowhegProductionbb::initialize() {
  powhegInitialize("bb");
  return PowhegProduction::initialize() ;
}

//=============================================================================
//Finalize
//=============================================================================
StatusCode PowhegProductionbb::finalize() {
  
  return PowhegProduction::finalize () ;
  
}

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( PowhegProductionbb )
// ============================================================================


// The END 
// ============================================================================
