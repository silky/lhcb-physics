/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef GIGA_IGIGARUNACTIONFACTORY_H 
#define GIGA_IGIGARUNACTIONFACTORY_H 1
/// ===========================================================================
/// STD & STL 
#include <string>
/// GaudiKernlel
#include "GaudiKernel/IFactory.h"
/// GiGa
#include "GiGa/IIDIGiGaRunActionFactory.h"
/// forward declarations 
class IGiGaRunAction;
class ISvcLocator;

/** @class IGiGaRunActionFactory IGiGaRunActionFactory.h
 *  
 *  Abstract factory for creation of IGiGaRunAction objects
 *
 *  @author Ivan Belyaev
 *  @date   26/07/2001
 */


class IGiGaRunActionFactory : virtual public IFactory 
{
  ///
 public:
  
  /** retrieve interface ID
   *  @return unique interafce identifier 
   */
  static const InterfaceID& interfaceID () 
  { return IID_IGiGaRunActionFactory; }
  
  /** create an instance of a concrete GiGaRunAction object 
   *  @param name name of the instance 
   *  @param svc  pointer to Service Locator 
   *  @return pointer to created Run Action object 
   */
  virtual IGiGaRunAction* instantiate ( const std::string& name , 
                                        ISvcLocator*       svc  ) const = 0;
  
  /** access to the GiGaEventAction type
   *  @return type of GiGaEventAction 
   */
  virtual const std::string&  runActionType ()    const  = 0;
  ///
};

/// ===========================================================================
#endif ///< GIGA_IGIGARUNACTIONFACTORY_H
/// ===========================================================================
