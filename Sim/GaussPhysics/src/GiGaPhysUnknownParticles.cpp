// $Id: GiGaPhysUnknownParticles.cpp,v 1.5 2009-07-03 09:11:33 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/PhysicalConstants.h"

// G4 
#include "G4UnknownParticle.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"

// local
#include "G4BcMesonMinus.h"
#include "G4BcMesonPlus.h"
#include "G4AntiLambdabZero.h"
#include "G4LambdabZero.h"
#include "G4AntiXibZero.h"
#include "G4XibZero.h"
#include "G4AntiXiccStarMinus.h"
#include "G4XiccStarPlus.h"
#include "G4Neutralino.h"
#include "G4Neutralino2.h"
#include "G4CharginoMinus.h"
#include "G4CharginoPlus.h"

//
#include "GiGaPhysUnknownParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaPhysUnknownParticles
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaPhysUnknownParticles );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaPhysUnknownParticles::GiGaPhysUnknownParticles
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent ) 
{ }

//=============================================================================
// Destructor
//=============================================================================
GiGaPhysUnknownParticles::~GiGaPhysUnknownParticles(){}

//=============================================================================
// ConstructParticle
//=============================================================================
void GiGaPhysUnknownParticles::ConstructParticle()
{
  G4BcMesonMinus::BcMesonMinusDefinition() ;
  G4BcMesonPlus::BcMesonPlusDefinition() ;  
  G4AntiLambdabZero::AntiLambdabZeroDefinition();
  G4LambdabZero::LambdabZeroDefinition();
  G4AntiXibZero::AntiXibZeroDefinition();
  G4XibZero::XibZeroDefinition();
  G4AntiXiccStarMinus::AntiXiccStarMinusDefinition();
  G4XiccStarPlus::XiccStarPlusDefinition();
  G4Neutralino::NeutralinoDefinition();
  G4Neutralino2::Neutralino2Definition();
  G4CharginoPlus::CharginoPlusDefinition();
  G4CharginoMinus::CharginoMinusDefinition();
  
  G4UnknownParticle::UnknownParticleDefinition();  
}

//=============================================================================
// ConstructProcess
//=============================================================================
void GiGaPhysUnknownParticles::ConstructProcess()
{
  theParticleIterator -> reset() ;
  while ( (*theParticleIterator)() ) {
    G4ParticleDefinition * particle = theParticleIterator -> value() ;
    G4ProcessManager     * pmanager = particle -> GetProcessManager() ;
//    if ( m_decayProcess.IsApplicable( *particle ) ) {
//      debug() << "### Decays for " << particle -> GetParticleName() 
//              << endreq ;
//      pmanager -> AddProcess( &m_decayProcess ) ;
      // set ordering for PostStepDoIt and AtRestDoIt
//      pmanager -> SetProcessOrdering( &m_decayProcess , idxPostStep ) ;
//      pmanager -> SetProcessOrdering( &m_decayProcess , idxAtRest   ) ;
//      pmanager -> DumpInfo() ;      
//    }
    if ( particle -> GetParticleName() == "unknown" ) {
      pmanager -> AddProcess( &m_unknownDecay ) ;
      pmanager -> SetProcessOrdering( &m_unknownDecay , idxPostStep ) ;
      debug() << "### Unknown Decays for " << particle -> GetParticleName() 
              << endreq ;
      if ( msgLevel( MSG::DEBUG) ) 
        pmanager -> DumpInfo() ;
    }
  }
}

//=============================================================================
