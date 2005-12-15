// $Id: MergeEventAlg.cpp,v 1.7 2005-12-15 13:56:37 cattanem Exp $
#define MERGEEVENTALG_CPP 
// Include files 

// from STL
#include <iostream>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataStoreItem.h"

// from LHCb
#include "Event/MCHeader.h"

// local
#include "MergeEventAlg.h"
#include "ILumiTool.h"

/** @file MergeEventAlg.cpp
 *  Implementation file for class : MergeEventAlg
 *  @date   2003-06-23
 *  @author Gloria CORTI
 */

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MergeEventAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MergeEventAlg::MergeEventAlg(const std::string& name, 
                             ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
  , m_mergeType( "LHCBackground" )
  , m_mergeSelectorName( "LHCBkgSelector" )
  , m_mergeSelector(0)
  , m_mergeIt(0)
  , m_lumiTool(0)
{
  m_subPaths.push_back( "LHCBackground" );
  
  // Declare the algorithm's properties
  declareProperty( "MergeType",     m_mergeType );
  declareProperty( "EventSelector", m_mergeSelectorName );
	declareProperty( "ItemList",      m_itemNames );
  declareProperty( "PathList",      m_subPaths );
}

//=============================================================================
// Destructor
//=============================================================================
MergeEventAlg::~MergeEventAlg( ) {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MergeEventAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) return Error( "Error initializing the base class", sc );

  // Check that the merging type is of supported type
  if( "LHCBackground" != m_mergeType && "Spillover" != m_mergeType ) {
    return Error( "Unknown merging type" );
  }

  // Print the fact that this background will be loaded
  info() << m_mergeType << " events will be added to the main event in paths";
  
  ItemNames::const_iterator itSubPaths = m_subPaths.begin();
  while( itSubPaths != m_subPaths.end() ) {
    info() << " " << *itSubPaths;
    itSubPaths++;
  }
  info() << endmsg;
  
  // Get the service manager interface of the service locator
  SmartIF<ISvcManager> svcMgr  ( IID_ISvcManager, serviceLocator());

  // Create and initialize the Merge Event Selector
  IService* pISvc;
  sc = svcMgr->createService( "EventSelector", m_mergeSelectorName, pISvc );
  if( sc.isFailure() ) return Error( "Error creating mergeSelector", sc );

  // Get the necessary base class and interface
  m_mergeSelector = dynamic_cast<Service*>(pISvc);
  m_mergeISelector = dynamic_cast<IEvtSelector*>(pISvc);
  sc = m_mergeSelector->initialize();
  if( sc.isFailure() )  {
    err() << "Error initializing " << m_mergeSelectorName << endreq;
    return sc;
  }
 
  // Clear the item list that will be loaded by this event selector
  clearItems(m_itemList);

  ItemNames::iterator i;
  // Take the new item list from the properties.
  for(i = m_itemNames.begin(); i != m_itemNames.end(); i++)   {
    addItem( m_itemList, *i );
  }

  // Get the luminosity tool
  if( "Spillover" == m_mergeType ) m_lumiTool = tool<ILumiTool>( "LumiTool" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Remove all items from the list
//=============================================================================
void MergeEventAlg::clearItems(Items& itms)     {
  for ( Items::iterator i = itms.begin(); i != itms.end(); i++ )    {  
    delete (*i);
  }
  itms.erase(itms.begin(), itms.end());
}

//=============================================================================
// Add item to the list
//=============================================================================
void MergeEventAlg::addItem(Items& itms, const std::string& descriptor)   {

  int sep = descriptor.rfind("#");
  int level = 0;
  std::string obj_path (descriptor,0,sep);
  std::string slevel   (descriptor,sep+1,descriptor.length());
  if ( slevel == "*" )  {
    level = 9999999;
  }
  else   {
    level = atoi(slevel.c_str());
  }

  std::string newPath = resetPath( obj_path );

  DataStoreItem* item = new DataStoreItem(newPath, level);
  debug() << "Will load and reset path for item " << item->path()
          << " with " << item->depth() << " level(s)." << endmsg;
  itms.push_back( item );
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MergeEventAlg::execute() {

  StatusCode sc = StatusCode::FAILURE;

  if( "LHCBackground" == m_mergeType ) {
    sc = readLHCBackground();
  }
  else if( "Spillover" == m_mergeType ) {
    sc = readSpillover();
  }
  else {
    err() << "Unknown merge event type " << m_mergeType << endmsg;
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MergeEventAlg::finalize() {

  clearItems(m_itemList);
  m_mergeSelector->finalize();
  m_mergeSelector->release();

  return GaudiAlgorithm::finalize();
} 

//=============================================================================
// Read an LHC background event
//=============================================================================
StatusCode MergeEventAlg::readLHCBackground( ) {

  // Get requested LHC background events
  std::string subPath = "LHCBackground";
  return readAndLoadEvent( subPath );
}

//=============================================================================
// Read an LHC spillover event
//=============================================================================
// N.B.: This implementation does not correctly handle pileup in spillover
//=============================================================================
StatusCode MergeEventAlg::readSpillover( ) {

  ItemNames::const_iterator itSubPaths = m_subPaths.begin();
  while( itSubPaths != m_subPaths.end() ) {
    int numInt = 0;
    StatusCode sc = m_lumiTool->numInteractions( numInt );
    if( !sc.isSuccess() ) return Error( "Error getting the luminosity", sc );

    if( 0 < numInt ) {
      std::string subPath = *itSubPaths;
      sc = readAndLoadEvent( subPath );
      if( !sc.isSuccess() ) return Error( "Error in loading " + subPath, sc );
    }
    itSubPaths++;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Read events via additional event selector and load in new path all
//  specified items
//=============================================================================
StatusCode MergeEventAlg::readAndLoadEvent( std::string& subPath ) {

  debug() << "Loading " << subPath << endmsg;
 
  if ( m_mergeIt == 0 ) {  // first event from this algorithm event selector
    m_mergeISelector->createContext(m_mergeIt);
  }
  // Read next event
  if( m_mergeISelector->next(*m_mergeIt).isFailure() ) {
    return Error("Last event reached on "+m_mergeSelectorName+" input stream");
  }
  
  // Find the /Event directory node of the background event.
  std::string eventPath = "/Event/"+subPath; 
  IOpaqueAddress* iadd;
  m_mergeISelector->createAddress( *m_mergeIt, iadd );
  SmartIF<IDataManagerSvc> dataMgr( eventSvc() );
  StatusCode sc = dataMgr->registerAddress( eventPath, iadd );
  if( !sc.isSuccess() ) {
    return Error( "Error setting event root to " + eventPath, sc );
  }

  // Load MC Header
  LHCb::MCHeader* evt = get<LHCb::MCHeader>(
                            eventPath + "/" + LHCb::MCHeaderLocation::Default);
  info() << "Loading "   << eventPath << " event " << evt->evtNumber() 
         << ",     Run " << evt->runNumber() << endmsg;
  
  // Read and load list as in properties
  for ( Items::iterator i = m_itemList.begin(); i != m_itemList.end(); i++ ) {
    std::string mcPath = eventPath + "/" + (*i)->path();
    DataObject* pMC = get<DataObject>( mcPath );

    // Reset links for object
    sc = resetLinks( subPath, pMC );
    if( !sc.isSuccess() ) {
      return Error( "Error in setting links for " + mcPath, sc );
    }
    // Read all leaves down to specified depth
    sc = readLeaves( subPath, pMC, (*i)->depth() );
    if( !sc.isSuccess() ) {
      return Error( "Error in loading leaves of " + subPath, sc );
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Read leaves attached to an object until specified depth and reset 
// links' path
//=============================================================================
StatusCode MergeEventAlg::readLeaves( std::string& subPath,
                                      const DataObject* pObj, long depth ) {
  
  depth = depth-1;
  if( 0 >= depth ) return StatusCode::SUCCESS;
  
  // Find and load all the leafs out of the object
  std::vector<IRegistry*> leaves;
  SmartIF<IDataManagerSvc> dataMgr( eventSvc() );
  StatusCode sc = dataMgr->objectLeaves(pObj, leaves);
  if( !sc.isSuccess() ) return sc;
  DataObject *pMCObj = 0;
  std::string objPath;
  for( std::vector<IRegistry*>::iterator ileaf=leaves.begin();
       ileaf!=leaves.end(); ++ileaf) {
    sc = eventSvc()->retrieveObject(*ileaf, objPath, pMCObj );
    if( sc.isFailure() ) {
      return sc;
    } 
    sc = resetLinks( subPath, pMCObj );
    if( !sc.isSuccess() ) return sc;
    sc = readLeaves( subPath, pMCObj, depth );
    if( !sc.isSuccess() ) return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Reset links to be consistent with new path
//=============================================================================
StatusCode MergeEventAlg::resetLinks( std::string& subPath,
                                      const DataObject* pMCObj ) {

  LinkManager::LinkVector oldlinks = pMCObj->linkMgr()->m_linkVector;
  if( oldlinks.empty() ) return StatusCode::SUCCESS;
  std::string mainPath = "/Event";
  LinkManager::LinkIterator ilink;
  std::vector< std::pair< std::string, long > > refs;
  for(ilink = oldlinks.begin(); ilink!= oldlinks.end(); ++ilink) {
    refs.push_back( std::make_pair( (*ilink)->path(), (*ilink)->ID() ) );
  }
  pMCObj->linkMgr()->clearLinks();
  for( std::vector< std::pair< std::string, long > >::iterator il=refs.begin();
       refs.end()!=il; ++il ) {
    std::string newPath = resetPath( (*il).first, subPath );
    long itemp = (*il).second;
    long lid = pMCObj->linkMgr()->addLink( newPath, NULL );
    if( lid != itemp ) {
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Reset path if subPath empty remove event if it exist from path
// if not add subPath after event, if event is not in path
//=============================================================================
std::string MergeEventAlg::resetPath( std::string& oldPath,
                                      const std::string& subPath ) {

  // Check if /Event already in path and reset
  std::string newPath = oldPath;
  std::string mainPath = "/Event";
  unsigned int pos = newPath.find(mainPath);
  if( pos != std::string::npos ) {
    newPath = newPath.erase( pos, mainPath.length()+1 );
    if( subPath != "" ) {
      newPath = mainPath+"/"+subPath+"/"+newPath;
    }
  }
  else {
    newPath = subPath+newPath;
  }
  
  return newPath;
}
