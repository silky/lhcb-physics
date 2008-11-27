// $Id: PythiaLSP.h,v 1.1 2008-11-27 16:04:00 robbep Exp $
#ifndef COMPONENT_PYTHIALSP_H 
#define COMPONENT_PYTHIALSP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "Generators/IGenCutTool.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"

/** @class PythiaLSP PythiaLSP.h component/PythiaLSP.h
 *  
 * CutTool to select events with (Susy) LSP satisfying certain cuts.
 * Usage :
 * Generation.Special.CutTool="PythiaLSP";
 * Condition to apply :
 * LSPCond = 1 : LSP in acceptance, = 2 : daughter quarks in acceptance
 *         = 3 : daughter leptons in acceptance
 *         = 4 : daughters quarks and leptons
 *         = 5 : all daughters in acceptance
 * Generation.Special.PythiaLSP.LSPCond = 1 ; // =d
 * Number of desired LSP :
 * Generation.Special.PythiaLSP.NbLSP = 1 ; //=d
 * Set if at least NbLSP should respect conditions or just NbLSP
 * Generation.Special.PythiaLSP.LSPID = { 1000022, 1000024 };//d=1000022
 * Availabe cuts :
 * Generation.Special.PythiaLSP.DistToPVMin (Max) = x*mm ; // d=0,infty
 * Generation.Special.PythiaLSP.ZPosMin (Max) = y*mm ;// d=-infty,infty
 * Generation.Special.PythiaLSP.EtaMax = 1.8 ; // =d
 * Generation.Special.PythiaLSP.EtaMin = 4.9 ; // =d
 *
 *  @author Neal Gueissaz
 *  @date   2008-September-1
 */
class PythiaLSP : public GaudiTool , virtual public IGenCutTool {
public: 
  /// Standard constructor
  PythiaLSP( const std::string & type , const std::string & name ,
               const IInterface * parent ); 

  virtual ~PythiaLSP( ); ///< Destructor

  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent ,
                         const LHCb::GenCollision * theCollision ) const ;
  
protected:

private:

  //void ResetParticles( HepMC::GenParticle *,CLHEP::HepLorentzVector &) const;
  bool IsQuark( HepMC::GenParticle * ) const;
  bool IsLepton( HepMC::GenParticle * ) const;
  bool IsLSP( HepMC::GenParticle * ) const;
  double Dist( HepMC::GenVertex *, HepMC::GenVertex *) const;

  std::vector<int> m_LSPID;    
  int m_NbLSP;
  int m_LSPCond;
  bool m_AtLeast;
  double m_EtaMin, m_EtaMax;
  double m_DistToPVMin, m_DistToPVMax, m_ZPosMin, m_ZPosMax;
};
#endif // COMPONENT_PYTHIALSP_H
