// $Id $
#ifndef MERGEEVENTALG_H
#define MERGEEVENTALG_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/Service.h"

// Forward declarations
class IDataManagerSvc;
class IDataProviderSvc;
class IConversionSvc;
class EventSelector;
class DataStoreItem;

/** @class MergeEvent MergeEvent.h
 *
 *  Merge events from different OO files and put them in different
 *  TES locations. Initialize a second EventSelector and 
 *  read LHC additional events from that second source.
 *  Based on SpillOverAlg.
 *  For the moment implement merging of flat event from LHC background
 *
 *  @author Gloria CORTI
 *  @date   2003-06-23  
 */

class MergeEventAlg : public Algorithm {

public:

  /// Typedefs
  typedef std::vector<DataStoreItem*> Items;
  typedef std::vector<std::string>    ItemNames;

  /// Standard constructor
  MergeEventAlg(const std::string& name, ISvcLocator* pSvcLocator); 

  virtual ~MergeEventAlg( );  ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  /// Clear list of data objects to load in new path
  void clearItems(Items& itms);
  /// Add item to list of data objects to load new path
  void addItem(Items& itms, const std::string& descriptor);

  /// Read additional event and load it into transient Event Store
  StatusCode readAndLoadEvent( std::string& subPath );

  /// Read leaves of unil specified depth
  StatusCode readLeaves( std::string& subPath, const DataObject* pObj,
                         long depth );

  /// Reset path of references attached to object
  StatusCode resetLinks( std::string& subPath, const DataObject* pObj );

  /* Reset path. When "/Event" in in the original path if subPath is ""
   * "/Event/" will be removed from the path, otherwise subPath will be
   * introduced after "/Event/"
   * When "/Event" is not in the original path, subPath is prepended to
   * the original path
   */
  std::string resetPath( std::string& oldPath, const std::string& subPath="");

  /// Type of events to merge
  std::string               m_mergeType;
  /// Name of additional event selector
  std::string               m_mergeSelectorName;

  /// Vector of item names
  ItemNames                m_itemNames;
  /// Vector of items to be loaded from this event selector
  Items                    m_itemList;

  /// Service of additional event selector 
  Service*                  m_mergeSelector; 
  /// IEvtSelector interface of additional event selector 
  IEvtSelector*             m_mergeISelector;
  /// EventSelector iterator
  IEvtSelector::Iterator*   m_mergeIt;
 
};
#endif    // MERGEEVENTALG_H
