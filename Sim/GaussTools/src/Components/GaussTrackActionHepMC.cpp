/// STD & STL 
#include <functional>
#include <algorithm>
// G4
#include "G4TrackingManager.hh"
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
// GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaMACROs.h"
#include "GiGaCnv/GiGaPrimaryParticleInformation.h"
// LHCb
#include "Event/MCVertex.h"
// Gauss
#include "GaussTools/GaussTrackInformation.h"
// local 
#include "GaussTrackActionHepMC.h"

// ============================================================================
/** @file 
 *  
 *  Implementation file for class : GaussTrackActionHepMC
 *  @date 24/07/2001 
 *  @author Witek Pokorski
 */
// ============================================================================

// ============================================================================
/// Factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GaussTrackActionHepMC ) ;

// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussTrackActionHepMC::GaussTrackActionHepMC
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent  ) 
{
  m_mcMgr = MCTruthManager::GetInstance();

  m_hadronicProcesses.clear();
  m_hadronicProcesses.push_back( "KaonPlusInelastic"             ) ;
  m_hadronicProcesses.push_back( "PionMinusAbsorptionAtRest"     ) ;
  m_hadronicProcesses.push_back( "KaonZeroLInelastic"            ) ;
  m_hadronicProcesses.push_back( "KaonZeroSInelastic"            ) ;
  m_hadronicProcesses.push_back( "MuonMinusCaptureAtRest"        ) ; 
  m_hadronicProcesses.push_back( "TritonInelastic"               ) ;
  m_hadronicProcesses.push_back( "KaonMinusAbsorption"           ) ;
  m_hadronicProcesses.push_back( "LambdaInelastic"               ) ;
  m_hadronicProcesses.push_back( "SigmaMinusInelastic"           ) ;
  m_hadronicProcesses.push_back( "LCapture"                      ) ;
  m_hadronicProcesses.push_back( "AntiNeutronAnnihilationAtRest" ) ;
  m_hadronicProcesses.push_back( "AntiProtonAnnihilationAtRest"  ) ;  
  m_hadronicProcesses.push_back( "AntiLambdaInelastic"           ) ;
  m_hadronicProcesses.push_back( "AntiXiZeroInelastic"           ) ;
  m_hadronicProcesses.push_back( "AntiSigmaPlusInelastic"        ) ;
  m_hadronicProcesses.push_back( "SigmaPlusInelastic"            ) ;
  m_hadronicProcesses.push_back( "XiMinusInelastic"              ) ;
  m_hadronicProcesses.push_back( "XiZeroInelastic"               ) ;
  m_hadronicProcesses.push_back( "AntiSigmaMinusInelastic"       ) ;
  m_hadronicProcesses.push_back( "AntiXiMinusInelastic"          ) ;
  m_hadronicProcesses.push_back( "OmegaMinusInelastic"           ) ;
  m_hadronicProcesses.push_back( "AntiOmegaMinusInelastic"       ) ;
  m_hadronicProcesses.push_back( "AlphaInelastic"                ) ;
  
  std::sort ( m_hadronicProcesses.begin () ,
              m_hadronicProcesses.end   () ) ;
  
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GaussTrackActionHepMC::~GaussTrackActionHepMC() {};
// ============================================================================

// ============================================================================
/** initialise the action object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GaussTrackActionHepMC::initialize () 
{
  StatusCode sc = GiGaTrackActionBase::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class!", sc ); }

  m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc", true );

  return Print("Iinitialized successfully" , 
               StatusCode::SUCCESS         , MSG::VERBOSE );
};

// ============================================================================
// finalize
// ============================================================================
StatusCode GaussTrackActionHepMC::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GiGaTrackActionBase::finalize();  // must be called after all other actions
}

// ============================================================================
/** perform the pre-action
 *  @param track pointer to Geant4 track object 
 */ 
// ============================================================================
void GaussTrackActionHepMC::PreUserTrackingAction  ( const G4Track* track )
{
  // new track is being started
  // we record its initial momentum 
  fourmomentum = HepLorentzVector( track->GetMomentum(), track->GetTotalEnergy() );

  // This should have been done in GaussPostTrackAction, but check here
  if( !track->GetUserInformation() ) {    
    GaussTrackInformation* mcinf = new GaussTrackInformation();
    trackMgr()->SetUserTrackInformation(mcinf);
  }
  
};
// ============================================================================

// ============================================================================
/** perform the post-action
 *  @param track pointer to Geant4 track object 
 */ 
// ============================================================================
void GaussTrackActionHepMC::PostUserTrackingAction  ( const G4Track* track )
{
  
//   HepMC::GenEvent* genevt = m_mcMgr->GetCurrentEvent();
  
  // if track is to be stored, create new GenParticle, and its decay vertex
  // we check the flag (store or not store) in the GaussTrackInformation
  GaussTrackInformation* ginf = (GaussTrackInformation*) track->GetUserInformation();

  if( ginf->storeHepMC() ) {
    HepLorentzVector prodpos(track->GetVertexPosition(),
                             track->GetGlobalTime() - track->GetLocalTime());
    HepLorentzVector endpos(track->GetPosition(), track->GetGlobalTime());

    // Get the pdgID+LHCb extension
    int pdgID = track->GetDefinition()->GetPDGEncoding();
    if( 0 == pdgID ) {
      ParticleProperty* pProp = m_ppSvc->find( track->GetDefinition()->GetParticleName() );
      if( NULL != pProp ) {
        pdgID = pProp->pdgID();
      } else {
        std::string message = "PDGEncoding does not exist, G4 name is ";
        message += track->GetDefinition()->GetParticleName();
        Warning( message, StatusCode::SUCCESS, 10 );
      }
    }

    // get the process type of the origin vertex
    int creatorID = processID( track->GetCreatorProcess() );
    if( creatorID == 0 ) {
      std::cout << "id = 0 " << std::endl;
    }

    // Get User information from primary particle to set Vertex type 
    // OscillatedAndDecay and pointer to HepMC event in generator and barcode
    //    LHCb::HepMCEvent* genEvent = NULL;
    bool hasOscillated = false;
    if( NULL != track->GetDynamicParticle() ) {
      if( NULL != track->GetDynamicParticle()->GetPrimaryParticle() ) {
        G4VUserPrimaryParticleInformation* g4uInf = 
          track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation();
        if( g4uInf ) {
          GiGaPrimaryParticleInformation* uInf = 
            (GiGaPrimaryParticleInformation*) g4uInf;
//           uInf->signalBarcode();
//          genEvent = uInf->pHepMCEvent();
          hasOscillated = uInf->hasOscillated();
          if( uInf->hasOscillated() ) {
            std::cout << "Particle has oscillated" << std::endl;
          }
        }
      }
    }


    m_mcMgr->AddParticle( fourmomentum, prodpos, endpos,
                          pdgID, track->GetTrackID(), track->GetParentID(), 
                          ginf->directParent(), creatorID, hasOscillated );
  }

  // For the moment the following is in GaussPostTrackAction
//   else {
//     // If track is not to be stored, propagate it's parent ID (stored) to its
//     // secondaries.    
//     G4TrackVector* childrens = trackMgr()->GimmeSecondaries() ;
//     //
//     for( unsigned int index = 0 ; index < childrens->size() ; ++index )
//     {
//       G4Track* tr = (*childrens)[index] ;
//       //
//       tr->SetParentID( track->GetParentID() );
//       // set the flag saying that the direct mother is not stored
//       GaussTrackInformation* mcinf = (GaussTrackInformation*) tr->GetUserInformation();
//       if(!mcinf) tr->SetUserInformation( mcinf = new GaussTrackInformation() );
//       mcinf->setDirectParent( false ); 
//       //
//     }     
//   }
  
};

// ============================================================================
// processID
// ============================================================================
int GaussTrackActionHepMC::processID(const G4VProcess* creator ) { 

  int processID = LHCb::MCVertex::Unknown;
  if( NULL == creator ) {
    processID = LHCb::MCVertex::DecayVertex;
    return processID;
  }
  
  if( fDecay == creator->GetProcessType() ) {
    processID = LHCb::MCVertex::DecayVertex;
  }
  else if ( fHadronic == creator->GetProcessType() ) { 
    processID = LHCb::MCVertex::HadronicInteraction;
  }
  else {
    const std::string& pname = creator->GetProcessName();
    if( "conv"== pname ) { 
      processID = LHCb::MCVertex::PairProduction;
    } else if( "compt" == pname ) {
      processID = LHCb::MCVertex::Compton;
    } else if( "eBrem" == pname || "muBrems" == pname ) {
      processID = LHCb::MCVertex::Bremsstrahlung;
    } else if( "annihil" == pname ) {
      processID = LHCb::MCVertex::Annihilation;
    } else if( "phot" == pname ) {
      processID = LHCb::MCVertex::PhotoElectric;
    } else if( "RichHpdPhotoelectricProcess" == pname ) {
      processID = LHCb::MCVertex::RICHPhotoElectric;
    } else if( "RichG4Cerenkov" == pname ) {
      processID = LHCb::MCVertex::Cerenkov;
    } else if( "eIoni" == pname || "hIoni" == pname || "ionIoni" == pname ||
               "muIoni" == pname ) {
      processID = LHCb::MCVertex::DeltaRay;
    } else {
      const bool found = std::binary_search( m_hadronicProcesses.begin() , 
                                             m_hadronicProcesses.end(), pname );
      if( found ) { 
        processID = LHCb::MCVertex::HadronicInteraction; 
      }
    }
  }
  
  if( processID == 0 ) {
    // here we have an intertsting situation
    // the process is *KNOWN*, but the vertex type 
    // is still 'Unknown'
    std::string message = "The process is known '" + 
      G4VProcess::GetProcessTypeName ( creator->GetProcessType() ) + "/" +
      creator -> GetProcessName() + "', but vertex type is still 'Unknown'";
    Warning( message, StatusCode::SUCCESS, 10 );
  }        

  return processID;
  
}

// ============================================================================
