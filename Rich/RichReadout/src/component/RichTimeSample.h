
#ifndef RICHREADOUT_RICHTIMESAMPLE_H
#define RICHREADOUT_RICHTIMESAMPLE_H 1

#include <vector>
#include <ostream>

#include "RichPixel.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace MC
   *
   *  General namespace for RICH MC related software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   05/12/2006
   */
  //-----------------------------------------------------------------------------
  namespace MC
  {

    //-----------------------------------------------------------------------------
    /** @namespace Digi
     *
     *  General namespace for RICH Digitisation simuation related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   17/01/2007
     */
    //-----------------------------------------------------------------------------
    namespace Digi
    {

      //===============================================================================
      /** @class RichTimeSample MCRichDigitsToRawBufferAlg.h
       *
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   2003-11-06
       */
      //===============================================================================
      class RichTimeSample
      {

      public:

        RichTimeSample( const int isize    = 25,
                        const double value = 50 )
          : TimeFrame ( isize, value ) { }

        ~RichTimeSample(){}

        inline void insert( const unsigned int i, const double value )
        {
          TimeFrame[i] = value;
        }

        inline unsigned int size() const
        {
          return TimeFrame.size();
        }

        inline double& operator[](const unsigned int i)
        {
          return TimeFrame[i];
        }

        inline const double operator[](const unsigned int i) const
        {
          return TimeFrame[i];
        }

        inline RichTimeSample& operator *= ( const double c )
        {
          for ( std::vector<double>::iterator i = TimeFrame.begin();
                i != TimeFrame.end(); ++i )
          {
            *i *= c;
          }
          return *this;
        }

        /// Operator overloading for string output
        friend std::ostream & operator << (std::ostream& os,
                                           const RichTimeSample & obj)
        {
          for ( unsigned int i = 0; i < obj.size(); ++i )
          {
            os << i << "=" << obj[i] << " ";
          }
          return os;
        }


      private:

        std::vector<double> TimeFrame;

      };

    }
  }
}

#endif // RICHREADOUT_RICHTIMESAMPLE_H
