// $Id: GiGaUtil.h,v 1.7 2002-05-07 12:21:30 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_GIGAUTIL_H 
#define GIGA_GIGAUTIL_H 1
// ============================================================================
/// STD & STL 
#include <string>
#include <functional>
/// GaudiKernel 
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/System.h"
/// forward declarations 
class IObjManager;
class ISvcLocator;
class IFactory;

/** @namespace  GiGaUtil GiGaUtil.h "GiGa/GiGaUtil.h"
 *  @brief  helper utilities for GiGa 
 *
 *  collection of simple helper utilities for GiGa package 
 * 
 *  @author Ivan Belyaev
 *  @date   23/07/2001
 */

namespace GiGaUtil 
{
  
  /** split a "Type/Name" string into "Type" and "Name"
   *  
   *  rules:
   * -  "Type/Name"  =  "Type" + "Name"
   * -  "Type"       =  "Type" + "Type"
   * -  "Type/"      =  "Type" + "Type"
   * 
   *  error conditions 
   *
   *  -  empty input string  
   *  -  more than 1 separator ('/') 
   *  -  empty type ("/Name")
   *
   *  @param TypeAndName   the string to be splitted 
   *  @param Type          returned "Type" 
   *  @param Name          returned "Name" 
   *  @return status code
   */  
  StatusCode SplitTypeAndName ( const std::string& TypeAndName ,
                                std::string      & Type        , 
                                std::string      & Name        );
  
  /** useful utility(function) to extract the object type name 
   *  @param obj pointer to object of type "TYPE"
   *  @return type name of the object
   */ 
  template <class TYPE>
  inline const std::string ObjTypeName( TYPE obj )
  { 
    return 
      obj ? 
      std::string( System::typeinfoName( typeid(*obj) ) ) : 
      std::string( "'UNKNOWN_type'" );
  };
  
  /** @class Delete 
   * 
   *  useful utility(templated class) to delete the object 
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  class Delete
  { 
  public:
    
    /** delete the object 
     *  @param obj pointer to object to be deleted 
     *  @return NULL pointer 
     */
    template <class TYPE>
    inline TYPE* operator() ( TYPE* obj ) const 
    { 
      if( 0 != obj ) { delete obj ; obj = 0 ; }
      return obj ;
    };
  };
  
  /** @class Eraser
   * 
   *  useful utility(functor) to delete the object 
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  template <class TYPE>
  class Eraser: public std::unary_function<TYPE*,TYPE*>
  { 
  public:
    
    /** delete the object 
     *  @param obj pointer to object to be deleted 
     *  @return NULL pointer 
     */
    inline TYPE* operator() ( TYPE* obj ) const 
    { 
      if( 0 != obj ) { delete obj ; obj = 0 ; }
      return obj ;
    };
  };
  
  /** useful utility(templated function)  
   *  to delete the object through the pointer  
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   *
   *  @param obj pointer to object to be deleted 
   *  @return NULL pointer 
   */ 
  template <class TYPE>
  inline TYPE* Delete_Ptr( TYPE* obj )
  { 
    if( 0 != obj ) { delete obj ; obj = 0 ; }
    return obj ;
  };

}; ///< end of namespace 

// ============================================================================
// The END
// ============================================================================
#endif ///< GIGA_GIGAUTIL_H
// ============================================================================
