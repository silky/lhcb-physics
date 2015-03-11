// $Id: AlpGen.cpp,v 1.2 2007-08-02 11:23:04 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <cstdlib>
// ============================================================================
/** @file
 *  The *MAIN* ALPGEN program
 *  @author Vanya BELYAEV ibelyaev@physuics.syr.edu
 *  @date 2007-08-02
 */
// ============================================================================
extern "C"
{
#ifdef WIN32 
  void __stdcall  ALPGEN   () ;
#define  alpgen   ALPGEN 
#else 
  void            alpgen_ () ;
#define  alpgen   alpgen_
#endif 
}
// ============================================================================
int main() 
{  
  alpgen () ;
  
  exit ( 1 ) ;
}
// ============================================================================
// The END 
// ============================================================================
