
#include "RichPixelReadout.h"
#include "RichShape.h"
#include "RichShape_flat.h"
//#include "RichNoisifier.h"
#include "RichFrontEndDigitiser.h"

RichPixelReadout::RichPixelReadout()
  :  m_shape     (0),
     m_shape_flat(0),
     m_frontEnd  (0),
     m_frameSize (100),
     m_baseline  (50),
     m_sigmaElecNoise(0.9)
{

  const double peakTime    = 25.0;
  const double peakTime2   = 25.0;
  const double calib       = 4.42;
  const double threshold   = 8.8;
  m_shape      = new RichShape( peakTime, 2.7 );
  m_shape_flat = new RichShape_flat( peakTime2, 2.7 );
  m_frontEnd   = new RichFrontEndDigitiser( threshold * m_sigmaElecNoise,
                                            calib );

}

void RichPixelReadout::cleanUp() 
{
  if ( m_shape )     { delete m_shape;      m_shape = 0;      }
  if ( m_shape_flat ){ delete m_shape_flat; m_shape_flat = 0; }
  if ( m_frontEnd )  { delete m_frontEnd;   m_frontEnd = 0;   }
}
