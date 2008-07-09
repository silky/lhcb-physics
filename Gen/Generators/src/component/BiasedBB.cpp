// $Id: BiasedBB.cpp,v 1.6 2008-07-09 14:32:00 robbep Exp $
// Include files 

// local
#include "BiasedBB.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Point4DTypes.h"
#include "GaudiKernel/Transform4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BiasedBB
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( BiasedBB );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BiasedBB::BiasedBB( const std::string& type , const std::string& name ,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "EtaMax" , m_etaMax = 4.7 ) ;
    declareProperty( "EtaMin" , m_etaMin = 2.2 ) ;
    declareProperty( "PtMin" , m_ptMin = 8.4 * Gaudi::Units::GeV ) ;
    declareProperty( "VMin" , m_vMin = 26. * Gaudi::Units::GeV ) ;
    declareProperty( "CTauMin" , m_ctauMin = 0.16 * Gaudi::Units::mm ) ;
  }

//=============================================================================
// Destructor 
//=============================================================================
BiasedBB::~BiasedBB( ) { ; }

//=============================================================================
// Acceptance function
//=============================================================================
bool BiasedBB::applyCut( ParticleVector & theParticleVector ,
                         const HepMC::GenEvent * /* theGenEvent */ ,
                         const LHCb::GenCollision * /* col */ ) const {
  double eta , pT , v , ctau ;
  ParticleVector::iterator it ;
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {
    // Original Lorentz vector
    Gaudi::LorentzVector fourM( (*it) -> momentum() ) ;
    // Compute eta
    eta = fabs( fourM.Eta() ) ;    

    // Compute pT 
    pT = fourM.Pt() ;
    
    // Compute v
    v = pT + 5.360 * Gaudi::Units::GeV * eta ;

    // Compute ctau
    Gaudi::LorentzVector pEnd , pBegin ; 

    pEnd.SetXYZT( (*it) -> end_vertex() -> position() . x() ,
                  (*it) -> end_vertex() -> position() . y() , 
                  (*it) -> end_vertex() -> position() . z() ,
                  (*it) -> end_vertex() -> position() . t() * Gaudi::Units::c_light ) ;

    pBegin.SetXYZT( (*it) -> production_vertex() -> position() . x() ,
                    (*it) -> production_vertex() -> position() . y() ,
                    (*it) -> production_vertex() -> position() . z() ,
                    (*it) -> production_vertex() -> position() . t() * 
                    Gaudi::Units::c_light ) ;  

    Gaudi::LorentzVector disp = pEnd - pBegin ;
    // Boost transformation
    ROOT::Math::Boost theBoost( fourM.BoostToCM() ) ;

    Gaudi::LorentzVector restDisp = theBoost( disp ) ;

    ctau = restDisp.t() ;

    if ( ( eta < m_etaMin ) || ( eta > m_etaMax ) || ( pT < m_ptMin ) ||
         ( v < m_vMin ) || ( ctau < m_ctauMin ) ) {
      debug() << "Particle rejected: [eta = " << eta 
              << "][pT = " << pT / Gaudi::Units::GeV << " GeV][v = " 
              << v / Gaudi::Units::GeV << " GeV][ctau = " 
              << ctau / Gaudi::Units::mm << " mm]" 
              << endmsg ;
      it = theParticleVector.erase( it );
    } else {
      debug() << "Particle accepted: [eta = " << eta 
              << "][pT = " << pT / Gaudi::Units::GeV << " GeV][v = " 
              << v / Gaudi::Units::GeV << " GeV][ctau = " 
              << ctau / Gaudi::Units::mm << " mm]" 
              << endmsg ;
      ++it ;
    }
  }
  
  return ( ! theParticleVector.empty() ) ;
}

