#ifndef RichG4SvcLocator_h 
#define RichG4SvcLocator_h 1

/// STD & STL 
#include <string>
/// GaudiKernel
class IDataProviderSvc;
class IMessageSvc;
class IHistogramSvc;
class INTupleSvc;

namespace RichG4SvcLocator
{

  static const std::string a_RichG4DataSvcName("DetectorDataSvc");
  /// name of Message  Service 
  static const std::string a_RichG4MessageSvcName("MessageSvc");
  //
  static const std::string a_RichG4HistoSvcName("HistogramDataSvc");

  static const std::string a_RichG4NtupSvcName("NTupleSvc");

  IDataProviderSvc*  RichG4detSvc ();

  IMessageSvc*       RichG4MsgSvc ();

  IHistogramSvc*     RichG4HistoSvc();

  INTupleSvc *       RichG4NtupleSvc();
};

#endif

