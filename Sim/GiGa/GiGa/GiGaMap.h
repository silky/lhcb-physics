// $Id: GiGaMap.h,v 1.1 2002-05-04 20:20:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_GIGAMAP_H 
#define GIGA_GIGAMAP_H 1
// Include files
#ifdef WIN32
#include "GaudiKernel/HashTable.h" // GaudiKernel (only for Visual-C Win32)
#include "GiGa/GiGaHash.h"         // GiGa        (only for Visual-C Win32)
#include <vector>
#else 
#include <map>                     // STD & STL   (except for Visual-C Win32)
#endif 

/** @class GiGaMap GiGaMap.h GiGa/GiGaMap.h
 *  
 *  Map adaptor class to avoid the 
 *  usage of std::map on Win32 platform for 
 *  MicroSoft C++ compiler.
 * 
 *  The code actually is imported from LHCb Calo software.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.
 *  @date   04/05/2002
 */
template<class KEY, class VALUE>
class GiGaMap 
{
public:
  
  /// type of the key 
  typedef  KEY                   Key      ;
  /// type of the mapped value  
  typedef  VALUE                 Value    ;
  
#ifdef WIN32 
  /** the  map itself
   *  @warning the actual type is platform-dependent!
   */
  typedef HashTable<Key,Value,GiGaHash<Key> >  Map  ; 
  typedef std::vector<Key>                     Keys ;
#else 
  /** the  map itself
   *  @warning the actual type is platform-dependent!
   */
  typedef  std::map<Key,Value>   Map      ;
#endif
  
  /** the type of iterator 
   *  @warning the actual type is platform-dependent!
   */
  typedef  Map::iterator         iterator       ;
  
  /** the type of const_iterator 
   *  @warning the actual type is platform-dependent!
   */
  typedef  Map::const_iterator   const_iterator ;
  
public:
  
  /// Standard constructor
  GiGaMap() 
    : m_map  () 
#ifdef WIN32 
    , m_keys () 
#endif 
  {};
  
  /// destructor (virtual)
  virtual ~GiGaMap() { clear() ; };
  
  /** access to elements by the key    (non-const!)
   *  for Linux the implementation is trivial
   * 
   *  @warning the implementation is platform-dependent!
   *
   *   - for Linux the implementation is trivial 
   *   - for WIN32 some tricks are applied 
   *
   *  @param key  key
   *  @return reference to the value
   */
  inline Value& operator[] ( const Key& key )
#ifdef WIN32 
  {
    // find the value 
    Map::value_type* value = m_map.find( key ) ;
    if( 0 != value ) { return value->second ; } // RETURN 
    // insert the key 
    m_map.insert( key , Value() );
    value = m_map.find( key ) ;
    return value->second ;
  };
#else 
  { return m_map[key]; };
#endif
  
  /** erase the sequence from the map
   *  @warning implementation  is platform-dependent
   *  @param first the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase( iterator first , 
              iterator last  )
  {
#ifndef WIN32 
    m_map.erase( first , last ); 
#else 
    if( !m_keys.empty() ) { m_keys.clear() ; }
    for( ; first != last ; ++first ) 
      { m_keys.push_back( first->first ); }
    for( Keys::const_iterator key = m_keys.begin() ; 
         m_keys.end() != key ; ++key ) { m_map.remove( *key  ) ; }
    m_keys.clear();
#endif;
  };
  
  /** erase the sequence from the map
   *  @warning implementation  is platform-dependent
   *  @param it the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase( iterator it )
  {
#ifndef WIN32 
    m_map.erase  ( it        ); 
#else 
    m_map.remove ( it->first );
#endif;
  };
  
  /** remove/erase element from the map (by key) 
   *  @warning implementation  is platform-dependent
   *  @param key key of element to be removed 
   *  @return true if element is removed
   */
  void erase( const Key& key ) 
  {
#ifndef WIN32
    m_map.erase  ( key ) ;
#else
    m_map.remove ( key ) ;
#endif 
  };
  
  /// clear the content of all containers 
  void clear() { m_map.clear() ; }
  
  /** get the size ( == number of elements )
   *  @return number of elements 
   */
  unsigned int  size() const { return m_map.size() ; }
  
  /** iterator for sequential access to the content of the "map"
   *  @return begin-iterator (non-const version) 
   */
  Map::iterator       begin ()       { return m_map.begin () ; }

  /** iterator for sequential access to the content of the "map"
   *  @return begin-iterator (const version) 
   */
  Map::const_iterator begin () const { return m_map.begin () ; }

  /** iterator for sequential access to the content of the "map"
   *  @return end-iterator (non-const version) 
   */
  Map::iterator       end   ()       { return m_map.end   () ; }

  /** iterator for sequential access to the content of the "map"
   *  @return end-iterator (const version) 
   */
  Map::const_iterator end   () const { return m_map.end   () ; }

private:
  
  Map m_map                ;   ///< the underlying  MAP container
  
#ifdef WIN32 
  std::vector<Key>  m_keys ; ///< auxillary small container ;
#endif
  
};

// ============================================================================'
// The END 
// ============================================================================
#endif // GIGA_GIGAMAP_H
// ============================================================================
