// $Id: Pythia8Production.h,v 1.1.1.1 2007-07-31 17:02:19 robbep Exp $
#ifndef LBPYTHIA8_PYTHIA8PRODUCTION_H 
#define LBPYTHIA8_PYTHIA8PRODUCTION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"
#include "Generators/ICounterLogFile.h"

// from Pythia8
#include "Pythia8/Pythia.h"

// LbPythia8.
#include "LbPythia8/LhcbHooks.h"

// Forward declaration
class IBeamTool ;
class GaudiRandomForPythia8 ;
class ILHAupFortranTool ;

/** @class Pythia8Production Pythia8Production.h 
 *  
 *  Interface tool to produce events with Pythia
 * 
 *  @author Arthur de Gromard
 *  @date   2007-06-28
 */
class Pythia8Production : public GaudiTool, virtual public IProductionTool {
private:
  // XML Log file to store cross-sections 
  ICounterLogFile * m_xmlLogTool ; 

  IBeamTool * m_beamTool ;

public:
  typedef std::vector<std::string> CommandVector ;
  
  /// Standard constructor
  Pythia8Production( const std::string & type , const std::string & name ,
                     const IInterface * parent ) ;
  
  virtual ~Pythia8Production( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
  
  virtual StatusCode finalize( ) ;   ///< Finalize method
  
  /// Generate one event
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;

  /// initialization specific to Pythia8
  virtual StatusCode initializeGenerator( ) ;
  
  /// Set particle stable
  virtual void setStable( const LHCb::ParticleProperty * thePP ) ;

  /// Modify particle property
  virtual void updateParticleProperties( const LHCb::ParticleProperty * thePP ) ;

  /// Turn on fragmentation
  virtual void turnOnFragmentation( ) ;

  /// Turn off fragmentation
  virtual void turnOffFragmentation( ) ;

  /// Hadronize unfragmented event
  virtual StatusCode hadronize( HepMC::GenEvent * theEvent , 
                                LHCb::GenCollision * theCollision ) ;
  
  /// Not needed in Pythia8
  virtual void savePartonEvent( HepMC::GenEvent * theEvent ) ;

  /// Not needed in Pythia8
  virtual void retrievePartonEvent( HepMC::GenEvent * theEvent ) ;

  /// Print parameters
  virtual void printRunningConditions( ) ;

  /// Set particles not to update
  virtual bool isSpecialParticle( const LHCb::ParticleProperty * thePP ) const ;

  /// Apply settings for forced fragmentation method
  virtual StatusCode setupForcedFragmentation( const int thePdgId ) ;
  
protected:

  // Central engine
  Pythia8::Pythia * m_pythia;   // Pythia8 engine
  Pythia8::LhcbHooks *m_hooks;  // LHCb user hooks
  Pythia8::Event m_event;       // generated event

  // Various input channels
  std::string m_tuningFile;       // from $LBPYTHIA8ROOT
  std::string m_tuningUserFile;   // Options file supplied by user
  CommandVector m_commandVector;  // Options lines from Gauss job  

  
  /// Print Pythia8 parameters
  void printPythiaParameter( ) ;
  
  /// Retrieve hard process information
  void hardProcessInfo( LHCb::GenCollision * theCollision ) ;
  
  /// PYTHIA  -> HEPMC conversion 
  StatusCode toHepMC ( HepMC::GenEvent*     theEvent    , 
                       LHCb::GenCollision * theCollision ) ;

  void checkPassedParticleProperties( );


private:

  /// get pythai8Id from the PP
  int getPythia8ID( const LHCb::ParticleProperty * thePP ) ;

  /// retrieve the processCode
  int processCode( ) ;

  /// retrieve the process Name
  string processName( int i );
    
  // Beam tool 
  std::string m_beamToolName ;
  BeamToolForPythia8 * m_pythiaBeamTool; ///< beam tool for Pythia8
  
  std::vector<int> m_pdtlist ;
  int m_nEvents ;
  
  GaudiRandomForPythia8 * m_randomEngine ; ///< Random Generator for Pythia8
  ILHAupFortranTool *     m_fortranUPTool ; ///< Tool to access Fortran User Processes
  std::string             m_fortranUPToolName ;


  // ==========================================================================
  bool m_validate_HEPEVT ; // force the valiadation of IO_HEPEVT 
  // ==========================================================================
  std::string   m_inconsistencies ; // the file to dump the HEPEVT incinsistencies 
  // ==========================================================================
  std::ostream* m_HEPEVT_errors ;
  // ==========================================================================
  
  bool m_listAllParticles ; ///list particles.
  bool m_checkParticleProperties ; //check part prop

  bool m_showBanner ; //flag to show banner or not

  
  std::string m_LHAupOptionFile;
  //kept for backward compatibilit for the time being
  
} ;
#endif // LBPYTHIA8_PYTHIA8PRODUCTION_H
