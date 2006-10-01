// $Id: Generators_load.cpp,v 1.3 2006-10-01 22:43:38 robbep Exp $
// Include files 

//
//  Package    : Gen/Generators
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(Generators) {
  
  DECLARE_ALGORITHM( DumpMC );
  DECLARE_ALGORITHM( DumpMCDecay );

  // New structure:
 
  // Main algorithm
  DECLARE_ALGORITHM( Generation ) ;
 
  // Luminosity Tools
  DECLARE_TOOL( VariableLuminosity ) ;
  DECLARE_TOOL( FixedLuminosity ) ;
  DECLARE_TOOL( FixedNInteractions ) ;
 
  // Sample Generation tools
  DECLARE_TOOL( MinimumBias ) ;
  DECLARE_TOOL( Inclusive ) ;
  DECLARE_TOOL( SignalPlain ) ;
  DECLARE_TOOL( SignalRepeatedHadronization ) ;
  DECLARE_TOOL( SignalForcedFragmentation ) ;
  DECLARE_TOOL( Special ) ;
  DECLARE_TOOL( StandAloneDecayTool ) ;
                                                                                                                 
  // Beam Tools
  DECLARE_TOOL( CollidingBeams ) ;
  DECLARE_TOOL( FixedTarget ) ;
 
  // Decay Tools
  DECLARE_TOOL( EvtGenDecay ) ;
 
  // Cut Tools
  DECLARE_TOOL( LHCbAcceptance     ) ;
  DECLARE_TOOL( BiasedBB           ) ;
  DECLARE_TOOL( DaughtersInLHCb    ) ;
 
  // Full event cut tools
  DECLARE_TOOL( LeptonInAcceptance ) ; 
  DECLARE_TOOL( MuXMaxBias         ) ;

}
