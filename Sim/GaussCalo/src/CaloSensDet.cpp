// $Id: CaloSensDet.cpp,v 1.13 2003-10-10 16:17:44 witoldp Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.12  2003/10/10 14:53:00  robbep
// Temporary ix in CaloSensDet against bug in copy number. Use
// directly geometry instead.
//
// Revision 1.11  2003/07/08 19:54:48  ibelyaev
//  minor fix for format printout
//
// Revision 1.10  2003/07/08 19:40:57  ibelyaev
//  Sensitive Plane Detector + improved printout
//
// ============================================================================
/// SRD & STD 
#include <algorithm>
#include <vector>
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/Stat.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaHashMap.h"
/// GaussTools 
#include "GaussTools/GaussTrackInformation.h"
/// Geant4 
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4EnergyLossTables.hh"
#include "G4Material.hh"
// GiGaCnv 
#include "GiGaCnv/GiGaVolumeUtils.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
/// local
#include "CaloSensDet.h"
#include "CaloHit.h"
#include "CaloSimHash.h"
///
#include "AIDA/IHistogram1D.h"

// ============================================================================
/** @file 
 * 
 *  Implementation of class CaloSensDet
 *  
 *  @author Vanya Belyaev
 *  @date   23/01/2001 
 */
// ============================================================================


// ============================================================================
/** standard constructor 
 *  @see GiGaSensDetBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
CaloSensDet::CaloSensDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaSensDetBase      ( type , name , parent ) 
  , G4VSensitiveDetector ( name  )
  , m_endVolumeName      ( "World" )  
  , m_end                ( 0       ) 
  , m_startVolumeNames   (         ) 
  , m_start              (         )
  , m_volumesLocated     ( false   )
  ///
  , m_collectionName     ( "Hits"  ) 
  , m_table              ()
  , m_hitmap             ()
  //
  , m_caloName           ( DeCalorimeterLocation::Ecal ) 
  , m_calo               ( 0 ) 
  ///
  , m_zmin               ( -1 * km )  // minimal z of forbidden zone 
  , m_zmax               (  1 * km )  // maximal z of forbidden zone 
  ///
  , m_collection         ( 0 )
  // input histograms 
  , m_histoNames         (   ) 
  , m_histos             (   ) 
  , m_histoSvc           ( 0 ) 
  //
  , m_birk_c1            ( 0.013  * g/MeV/cm2           ) // 1st coef. of Birk's
  , m_birk_c2            ( 9.6E-6 * g*g/MeV/MeV/cm2/cm2 ) // 2nd coef
  //. of Birk's law correction of c1 for 2e charged part.
  , m_birk_c1correction  ( 0.57142857                   ) 
  /// correction to t0
  , m_dT0                ( 0.5 * ns                     )
  /// counters 
  , m_stat               ( true                         ) 
  , m_events             (     0      ) 
  , m_hits               (     0      ) 
  , m_hits2              (     0      )
  , m_hitsMin            (     1.e+10 ) 
  , m_hitsMax            (    -1.e+10 ) 
  , m_shits              (     0      ) 
  , m_shits2             (     0      ) 
  , m_shitsMin           (     1.e+10 ) 
  , m_shitsMax           (    -1.e+10 ) 
  , m_energy             (     0      )
  , m_energy2            (     0      )
  , m_energyMin          (  1000 * TeV ) 
  , m_energyMax          (    -1 * TeV ) 
{
  setProperty     ( "DetectorDataProvider" ,  "DetectorDataSvc"   ) ;
  
  declareProperty ( "StartVolumes"         ,  m_startVolumeNames  ) ;
  declareProperty ( "EndVolume"            ,  m_endVolumeName     ) ;
  declareProperty ( "CollectionName"       ,  m_collectionName    ) ;
  declareProperty ( "Detector"             ,  m_caloName          ) ;
  declareProperty ( "zMin"                 ,  m_zmin              ) ;
  declareProperty ( "zMax"                 ,  m_zmax              ) ;
  //
  declareProperty ( "BirkC1"               ,  m_birk_c1           ) ;
  declareProperty ( "BirkC1cor"            ,  m_birk_c1correction ) ;
  declareProperty ( "BirkC2"               ,  m_birk_c2           ) ;
  //
  declareProperty ( "dT0"                  ,  m_dT0               ) ;
  // input histograms(parametrization)
  declareProperty ( "Histograms"           ,  m_histoNames        ) ;
  // perform statistical analysis?
  declareProperty ( "EvaluateStatistics"   ,  m_stat              ) ;
};
// ============================================================================

// ============================================================================
/** standard initialization (Gaudi) 
 *  @see GiGaSensDetBase
 *  @see GiGaBase 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return statsu code 
 */
// ============================================================================
StatusCode CaloSensDet::initialize   ()
{  
  // initialze the base class 
  StatusCode sc = GiGaSensDetBase::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class!",sc);}
  //
  // clear collection name vector 
  collectionName.clear  () ;
  collectionName.insert ( m_collectionName );
  ///
  if( 0 == detSvc() ) { return Error("detSvc() points to NULL!")       ; }
  m_calo = get( detSvc() , m_caloName , m_calo );
  if( 0 == m_calo   ) { return StatusCode::FAILURE                     ; }
  m_caloID   = CaloCellCode::CaloNumFromName( caloName()             ) ;
  if( 0 >  caloID() ) { return Error("Invalid detector name/number!" ) ; }  
  ///
  { // load the histogram service 
    if( 0 != m_histoSvc ) { m_histoSvc->release () ; m_histoSvc = 0 ; }
    sc = service("HistogramDataSvc" , m_histoSvc , true );
    if( sc.isFailure() ) 
      { return Error ( "HistogramDataSvc is not available!" , sc ) ; }
  }
  { // load all input histos 
    for( Names::const_iterator ihist = m_histoNames.begin() ; 
         m_histoNames.end() != ihist ; ++ihist ) 
      {
        SmartDataPtr<IHistogram1D> pHist( histoSvc() , *ihist );
        IHistogram1D* hist = pHist ;
        if( 0 == hist ) 
          { return Error("Cannot load histogram '"+(*ihist)+"'"); }       
        m_histos.push_back ( hist ) ;
      }
    if( histos().empty() ) 
      { Warning ( "Empty vector of input time-histograms" ) ; }
  }
  ///
    
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** standard finalization (Gaudi) 
 *  @see GiGaSensDetBase
 *  @see GiGaBase 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return statsu code 
 */
// ============================================================================
StatusCode CaloSensDet::finalize    ()
{  
  if( m_stat ) 
    { /// statistical printout 
      MsgStream log( msgSvc() , name() ) ;
      log << MSG::DEBUG << 
        format ( " <#Hits>/Min/Max=(%3d+-%3d)/%d/%4d "                  , 
                 (long) m_hits                                          , 
                 (long) sqrt ( fabs( m_hits2 - m_hits * m_hits ) )      ,
                 (long) m_hitsMin                                       ,
                 (long) m_hitsMax                                       ) ;
      log << 
        format ( " <#SubHits>/Min/Max=(%4d+-%4d)/%d/%4d "               , 
                 (long) m_shits                                         , 
                 (long) sqrt ( fabs( m_shits2 - m_shits * m_shits ) )   ,
                 (long) m_shitsMin                                      ,
                 (long) m_shitsMax                                      ) ;
      log <<
        format ( " <E>/Min/Max[GeV]=(%.3g+-%.3g)/%g/%.3g"         , 
                 m_energy                                         / GeV , 
                 sqrt ( fabs( m_energy2 - m_energy * m_energy ) ) / GeV ,
                 m_energyMin                                      / GeV ,
                 m_energyMax                                      / GeV ) <<  
        endreq ;
    }
  // reset the detector element 
  m_calo         = 0 ;
  // clear the translation table 
  m_table  .clear () ;
  // clear hit map 
  m_hitmap .clear () ;
  // clear volumes 
  m_start  .clear () ;
  // clear histograms 
  m_histos .clear () ;
  // release histo svc 
  if( 0 != m_histoSvc ) { m_histoSvc->release() ; m_histoSvc = 0 ; }
  // finalize the base class 
  return GiGaSensDetBase::finalize();
};
// ============================================================================

// ============================================================================
/** helpful method to locate start and end volumes 
 *  @return status code
 */
// ============================================================================
StatusCode  CaloSensDet::locateVolumes()
{ 
  // locate start volumes  
  for( Names::const_iterator vol =  m_startVolumeNames.begin() ; 
       m_startVolumeNames.end() != vol ; ++vol )
    {
      // look through converted volumes 
      const G4LogicalVolume* lv = GiGaVolumeUtils::findLVolume   ( *vol );
      if( 0 == lv ) 
        { return Error("G4LogicalVolume* points to 0 for "+ (*vol) );}
      m_start.push_back( lv );
    }
  if( m_start.empty() ) { return Error("Size of 'StartVolumes' is 0 "); }
  // locate end volume : look through converted volumes 
  m_end = GiGaVolumeUtils::findLVolume   ( m_endVolumeName );
  if( 0 == m_end ) 
    { return Error("G4LogicalVolume* points to 0 for '"+m_endVolumeName+"'");}
  // set flag 
  m_volumesLocated = true ;
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================
 
// ============================================================================
/** method from G4 
 *  (Called at the begin of each event)
 *  @see G4VSensitiveDetector 
 *  @param HCE pointer to hit collection of current event 
 */
// ============================================================================
void CaloSensDet::Initialize( G4HCofThisEvent* HCE )
{
  //
  if( !m_volumesLocated ) {
    StatusCode sc = locateVolumes();
    if( sc.isFailure() ) { 
      Error("Error from 'locateVolumes' method",sc); 
    }
  }
  Assert( m_volumesLocated , "Could not locate volumes!");
  //
  
  m_collection = new CaloHitsCollection ( SensitiveDetectorName , 
                                          collectionName[0]     ) ; 
  //
  const int id  = GetCollectionID( 0 ) ;
  
  HCE -> AddHitsCollection( id , m_collection );
  
  //
  Print(" Initialize(): CollectionName='" + m_collection->GetName   () +
        "' for SensDet='"                 + m_collection->GetSDname () + 
        "'" , StatusCode::SUCCESS , MSG::VERBOSE                       ) ;
  //
  m_hitmap.clear() ;
};
// ============================================================================

// ============================================================================
/** method from G4 
 *  (Called at the end of each event)
 *  @see G4VSensitiveDetector 
 *  @param HCE pointer to hit collection of current event 
 */
// ============================================================================
void CaloSensDet::EndOfEvent( G4HCofThisEvent* HCE ) 
{
  /// clear the map 
  m_hitmap.clear();
  
  // perform the simple stat analysis ? 
  if( !m_stat ) { return ; }                                // RETURN !!
  
  /// increase the counter of processed events
  ++m_events ;
  const double f1 = 1.0 / ( (double) ( m_events     ) ) ;
  const double f2 =  f1 * ( (double) ( m_events - 1 ) ) ;
  
  if ( 0 == m_collection ) 
    { Warning ( " EndOfEvent(): HitCollection points to NULL " ) ; return ; } 
  typedef std::vector<CaloHit*> Hits ;
  const Hits* hits = m_collection ->GetVector() ;
  if ( 0 == hits ) 
    { Error   (" EndOfEvent(): HitVector* points to NULL "     ) ; return ; } 
  
  // initialize counters 
  size_t nshits = 0 ;
  size_t nthits = 0 ;
  double energy = 0 ;
  
  // the loop over all hits 
  for( Hits::const_iterator ihit = hits->begin() ; 
       hits->end() != ihit ; ++ihit ) 
    {
      const CaloHit* hit = *ihit ;
      if( 0 == hit ) { continue ; }                           // CONTINUE 
      nshits += hit -> size      () ;
      nthits += hit -> totalSize () ;
      energy += hit -> energy    () ;    
   }

  
  // number of hits 
  const size_t nhits = hits->size()  ;
  m_hits    = m_hits    * f2   +   nhits           * f1 ;
  m_hits2   = m_hits2   * f2   +   nhits  * nhits  * f1 ;
  
  m_shits   = m_shits   * f2   +   nshits          * f1 ;
  m_shits2  = m_shits2  * f2   +   nshits * nshits * f1 ;

  m_energy  = m_energy  * f2   +   energy          * f1 ;
  m_energy2 = m_energy2 * f2   +   energy * energy * f1 ;

  if ( nhits  > m_hitsMax   ) { m_hitsMax   = nhits  ; }
  if ( nhits  < m_hitsMin   ) { m_hitsMin   = nhits  ; }
  if ( nshits > m_shitsMax  ) { m_shitsMax  = nshits ; }
  if ( nshits < m_shitsMin  ) { m_shitsMin  = nshits ; }
  if ( energy > m_energyMax ) { m_energyMax = energy ; }
  if ( energy < m_energyMin ) { m_energyMin = energy ; }
  
  MsgStream log ( msgSvc() , name() ) ;
  log << MSG::DEBUG <<
    format 
    ( " #CaloHits=%4d #CaloSubHits=%4d #TimeSlots=%4d Energy[GeV]=%.3g ",
      nhits , nshits , nthits , energy/GeV ) << endreq ;

};
// ============================================================================

// ============================================================================
/** process the hit
 *  @param step     pointer to current Geant4 step 
 *  @param history  pointert to touchable history 
 */
// ============================================================================
bool CaloSensDet::ProcessHits( G4Step* step                      , 
                               G4TouchableHistory* /* history */ ) 
{

  if( 0 == step ) { return false ; } 
  ///
  const double                      deposit  = step-> GetTotalEnergyDeposit () ;
  if( deposit <= 0            ) { return false ; }                 // RETURN 
  
  const G4Track*              const track    = step     -> GetTrack      () ;
  const int                         trackID  = track    -> GetTrackID    () ;
  const G4ParticleDefinition* const particle = track    -> GetDefinition () ;
  const double                      charge   = particle -> GetPDGCharge  () ;
  
  if( 0 == int ( charge * 10 ) ) { return false ; }                // RETURN 
  
  const G4StepPoint* const          preStep  = step    -> GetPreStepPoint () ;
  const HepPoint3D&                 prePoint = preStep -> GetPosition     () ;
  const double                      time     = preStep -> GetGlobalTime   () ;
  const G4MaterialCutsCouple* const material = preStep -> 
    GetMaterialCutsCouple () ;
  
  //  G4TouchableHistory* tHist  = (G4TouchableHistory*)
  //    ( step->GetPreStepPoint()->GetTouchable() ) ;
  
  //  CaloSim::Path path ;
  //  path.reserve( 16 ) ;
  
  //  for( int level = 0 ; level < tHist->GetHistoryDepth() ; ++level ) 
  //    {
  //      const G4VPhysicalVolume* pv = tHist -> GetVolume( level ) ;
  //      if( 0 == pv ) { continue ; }                          // CONTINUE 
  //      const G4LogicalVolume*   lv = pv    -> GetLogicalVolume();
  //      if( 0 == lv ) { continue ; }                          // CONTINUE
      
      //  start volume ? 
  //      if      (  m_end == lv  )    { break ; } // BREAK    
      // "useful" volume 
  //      else if ( !path.empty() ) 
  //        { path.push_back( tHist->GetReplicaNumber( level ) ); 
  //      else if ( m_start .end() != std::find( m_start .begin () , 
  //                                             m_start .end   () , lv ) )
  //        { path.push_back( tHist->GetReplicaNumber( level ) ); 
      // end volume ?
  //    }

  //  if( path.empty() ) 
  //    { Error("Replica Path is invalid!")      ; return false ; } // RETURN 
  
  // find the cellID 
  //  CaloCellID  cellID( m_table( path ) ) ;

  CaloCellID cellID = calo() -> Cell ( prePoint ) ;
  
  //  if( CaloCellID() == cellID ) 
  //    {
  //      cellID          = calo() -> Cell ( prePoint );
      // skip the invalid cells 
      if ( !( calo() -> valid( cellID ) ) ) { return false ; }  // RETURN 
      //      m_table( path ) = cellID ;
      //    }
      //  if( CaloCellID() == cellID ) 
      //    { Error ("Invalid cell is found") ;       return false ; }  // RETURN 
  
  // get the existing hit 
  CaloHit*&    hit = m_hitmap( cellID );                        // ATTENTION 
  if( 0 == hit )  // hit does not exists 
    {
      // create new hit 
      hit = new CaloHit      ( cellID ) ; 
      // add it into collection 
      m_collection -> insert ( hit    ) ;
    }
  
  // check the status of the track
  GaussTrackInformation* info = 
    gaussTrackInformation( track->GetUserInformation() );
  if( 0 == info ) 
    { Error("Invalid Track information") ; return false ; }     // RETURN
  
  // ID of the track to be stored 
  int sTrackID     = track -> GetParentID () ;
  
  // already marked to be stored:
  if     ( info -> toBeStored()     ) { sTrackID = trackID ; }  
  else 
    {
      // z-position of production vertex 
      const double z0 = track->GetVertexPosition().z() ;
      // outside the "forbidden zone" ?  
      if ( z0 < zMin() || z0 > zMax ()  ) { sTrackID = trackID ; }
    }
  
  // Does the hit exist for the given track? 
  CaloSubHit*& sub  = hit->hit( sTrackID ) ;                   // ATTENTION
  // create new subhit if needed 
  if( 0 == sub ) { sub = new CaloSubHit ( cellID , sTrackID ) ; }
  // update the track informoation
  if( trackID == sTrackID ) { info->addToHits( sub ) ; }
  
  // perform the specific sub-detector action
  StatusCode sc = fillHitInfo ( sub       , 
                                prePoint  , 
                                time      ,
                                deposit   ,
                                track     , 
                                particle  ,
                                material  ,
                                step      ) ;
  
  if( sc.isFailure() ){ Error("The SubHit information is not filled!",sc) ; }
  
  return true ;
};


// ============================================================================
// Birk's Law
// ============================================================================
/** Correction factor from Birk's Law
 *  Factor = 1/(1+C1*dEdx/rho+C2*(dEdx/rho)^2)
 *  Where :
 *      - C1 = 0.013 g/MeV/cm^2 [Ref NIM 80 (1970) 239]
 *      - C2 = 9.6.10^-6 g^2/MeV^2/cm^4
 *      - dEdx in MeV/cm
 *      - rho = density in g/cm^3
 */
// ============================================================================
double CaloSensDet::birkCorrection(const G4Step* step) const
{
  double result = 1. ;
  if (step) {
    
    // Track
    const G4Track* track  = step->GetTrack() ;
    const double charge   = track->GetDefinition()->GetPDGCharge()  ;
    
    // Only charged tracks
    if (abs(charge)>0.1) {
      
      // Birk's law coefficients
      double C1       = birk_c1 () ;
      double C2       = birk_c2 () ;
      
      // Correction for charge 2 particles
      if (abs(charge)>1.5) { C1 *= birk_c1cor () ; }
      
      // Material
      const G4Material* mat = step->GetPreStepPoint()->GetMaterial() ;
      double density  = mat->GetDensity() ;
      
      // dEdx
      
      const G4MaterialCutsCouple *aMaterialCut = 
        track->GetMaterialCutsCouple() ;
      
      double dEdx = 
        G4EnergyLossTables::GetDEDX( track->GetDefinition() ,
                                     track->GetKineticEnergy() ,
                                     aMaterialCut ) ;
      
      result = 1./(1. + C1*dEdx/density + C2*dEdx*dEdx/density/density ) ;
    }
  }
  
  return result ;
};

// ============================================================================
/** Birk's correction for given particle with given kinetic energy 
 *  for the given material
 *  @param  particle pointer to particle definition 
 *  @param  Ekine    particle kinetic energy
 *  @param  maerial  pointer ot teh material 
 */
// ============================================================================
double CaloSensDet::birkCorrection 
( const G4ParticleDefinition* particle , 
  const double                eKine    ,
  const G4MaterialCutsCouple* material ) const 
{
  if (  0 == particle || 0 == material ) 
    { Error("birkCorrection(): invalid parameters " ) ; return 1.0 ; } // RETURN
  
  const double charge = particle -> GetPDGCharge() ;
  if( 0 == int ( charge * 30 ) ) 
    { Warning("birkCorrection for neutral particle!") ; return 1.0 ; }
  
  // get the nominal dEdX 
  const double dEdX  = 
    G4EnergyLossTables::GetDEDX ( particle , eKine , material ) ;
  
  return birkCorrection ( charge , 
                          dEdX   , material->GetMaterial()->GetDensity() ) ;
} ;
// ============================================================================

// ============================================================================
/** evaluate the correction for Birk's law 
 *  @param charge   the charge of the particle 
 *  @param dEdX     the nominal dEdX in the material
 *  @param material the pointer ot teh material 
 *  @return the correction coefficient 
 */
// ============================================================================
double  CaloSensDet::birkCorrection 
( const double      charge   ,
  const double      dEdX     , 
  const G4Material* material ) const 
{
  if ( 0 == material ) 
    { Error("birkCorrection(): invalid material " ) ; return 1. ; } // RETURN
  return birkCorrection( charge , dEdX , material->GetDensity() ) ;
};
// ============================================================================

// ============================================================================
/** evaluate the correction for Birk's law 
 *  @param charge   the charge of the particle 
 *  @param dEdX     the nominal dEdX in the material
 *  @param density 
 *  @return the correction coefficient 
 */
// ============================================================================
double  CaloSensDet::birkCorrection 
( const double      charge   ,
  const double      dEdX     , 
  const double      density  ) const 
{
  const double C1 = 
    fabs( charge )  < 1.5 ? birk_c1() : m_birk_c1 * birk_c1cor() ;
  const double C2 = birk_c2() ;
  
  const double alpha = dEdX/ density ;
  
  return 1.0 / ( 1.0 + C1 * alpha + C2 * alpha * alpha ) ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
