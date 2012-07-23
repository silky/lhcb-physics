#ifndef LBALPGEN_ALPGENPRODUCTION_H 
#define LBALPGEN_ALPGENPRODUCTION_H 1

// Include files
#include "LbPythia/PythiaProduction.h"

// Forward declarations
class IRndmGenSvc ;
class IRndmEngine ;

/** @class AlpGenProduction AlpGenProduction.h 
 *  
 *  Interface tool to produce events with AlpGen
 * 
 *  @author Stephane Tourneur
 *  @author Patrick Robbe
 *  @date   2012-07-13
 */

class AlpGenProduction : public PythiaProduction {
 public:
  /// Standard constructor
  AlpGenProduction( const std::string & type , 
                    const std::string & name ,
                    const IInterface * parent ) ;
  
  virtual ~AlpGenProduction( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method

  virtual StatusCode finalize( ) ; ///< Finalize method
  
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;


 protected:

 private:
  StatusCode generateWeightedEvents( ) ;
  void generateUnweightedEvents( ) ;
  int           m_nevents ;
  IRndmGenSvc * m_randSvc ;
  IRndmEngine * m_engine ;
  std::string   m_fileLabel ;
};
#endif // LBBCVEGPY_BCVEGPYPRODUCTION_H
