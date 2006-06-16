#ifndef MUONALGS_MUONDIGITIZATIONDATA_CPP
#define MUONALGS_MUONDIGITIZATIONDATA_CPP 1

#include <string>
#include "MuonDigitizationData.h"    
#include "MuonPhyChannelInput.h"    
#include "MuonPhysicalChannel.h" 
#include "MuonPhysicalChannelOutput.h" 

template<class T> 
MuonDigitizationData<T>::MuonDigitizationData(const std::string &path, 
                                              MsgStream* mlog, 
                                              IDataProviderSvc*
																							eveSvc,const std::string 
                                              &specpath)
{
  //  MsgStream log(msgSvc(), name());
  s_basePathInTES = path ;
  m_registered = 0 ;
  log = (mlog);
  eventSvc=eveSvc ;
	s_specificPath=specpath ; 
  for(int i=0;i<MuonDigitizationData_m_partitionNumber;i++){    
    KeyedContainer<T>* pippo=new  KeyedContainer<T>;
    muonDataPartition[i]=pippo;    
  }
  
// *log << MSG::INFO<<"ciao"<<endreq;

endPathInTES[0]=std::string("M1/R1");
endPathInTES[1]=std::string("M1/R2");
endPathInTES[2]=std::string("M1/R3");
endPathInTES[3]=std::string("M1/R4");
endPathInTES[4]=std::string("M2/R1");
endPathInTES[5]=std::string("M2/R2");
endPathInTES[6]=std::string("M2/R3");
endPathInTES[7]=std::string("M2/R4");
endPathInTES[8]=std::string("M3/R1");
endPathInTES[9]=std::string("M3/R2");
endPathInTES[10]=std::string("M3/R3");
endPathInTES[11]=std::string("M3/R4");
endPathInTES[12]=std::string("M4/R1");
endPathInTES[13]=std::string("M4/R2");
endPathInTES[14]=std::string("M4/R3");
endPathInTES[15]=std::string("M4/R4");
endPathInTES[16]=std::string("M5/R1");
endPathInTES[17]=std::string("M5/R2");
endPathInTES[18]=std::string("M5/R3");
endPathInTES[19]=std::string("M5/R4");


}


template<class T> MuonDigitizationData<T>::~MuonDigitizationData(){
 //*log << MSG::INFO<<"ciao"<<m_registered<<endreq;
  if(m_registered==0){
    *log << MSG::DEBUG 
         <<" the contatiners have not been registed in the TES"
         << " => deleting objects "<<endreq;
    typename KeyedContainer<T>::iterator it;
    for(int i=0;i<MuonDigitizationData_m_partitionNumber;i++){      
      for(it=(*muonDataPartition[i]).begin();
          it<(*muonDataPartition[i]).end();it++){
        delete  (*it);
      }
      delete muonDataPartition[i];
      
    }
  }
  else if(m_registered>0){
    *log << MSG::DEBUG 
         <<" the contatiners have been registed in the TES " <<endreq;
  }
  else if(m_registered<0){
    *log << MSG::DEBUG
         << " the contatiners have not been registed in the TES"
         << " for some error."
         << endreq;
    
  } ;  
// *log << MSG::INFO<<"cavolo "<<endreq;

}

template<class T> StatusCode MuonDigitizationData<T>::registerPartitions(){
  std::string path;
  for(int i=0;i<MuonDigitizationData_m_partitionNumber;i++){
    path=s_basePathInTES+"/"+endPathInTES[i]+"/"+s_specificPath;
//    *log << MSG::INFO <<path<<" "<<eventSvc<<endreq;
    	 *log << MSG::DEBUG <<path<<endreq;
    
    StatusCode result= eventSvc->registerObject(path, muonDataPartition[i]);
    // StatusCode result=StatusCode::SUCCESS;
    if(result==StatusCode::SUCCESS){
//      *log << MSG::INFO <<i<<" result "<<result<<endreq;
    }else{
      *log << MSG::INFO <<i<<" non funziona "<<muonDataPartition[i]->size()<<endreq;        
	m_registered=-1;
    } 
  }
  m_registered=1;
  *log << MSG::INFO<<"register "<<m_registered<<endreq;
  return StatusCode::SUCCESS;
}

template<class T> StatusCode
MuonDigitizationData<T>::addMuonObject(int i, T* p0){
  (*muonDataPartition[i]).add(p0);
  return StatusCode::SUCCESS;
}

template<class T> StatusCode
MuonDigitizationData<T>::eraseMuonObject(int i, T* p0){
  (*muonDataPartition[i]).remove(p0);
  delete p0;
  return StatusCode::SUCCESS;
}

template<class T> bool MuonDigitizationData<T>::isObjectIn(int i,T* p0, T*&
                                                           pFound, 
                                                           bool (*comp)
                                                           (T* p1,T* p2))
{  
  typename KeyedContainer<T>::iterator iter ;
  iter=(*muonDataPartition[i]).begin();
  bool found=false;
  if(iter<(*muonDataPartition[i]).end()){    
    do{
      found=(*comp)(p0,(*iter));
      if(found) pFound=*iter ;
      ++iter ;
    }while(iter<(*muonDataPartition[i]).end()&&!found);
  }
	return found;
}	


template<class T> KeyedContainer<T>*
MuonDigitizationData<T>::getPartition(int i)
{
	return muonDataPartition[i];
}

template<class T> bool MuonDigitizationData<T>::isEmpty(int i){
//  return  ((getPartition(i)->size()=0)?true:false);
  long size=(*muonDataPartition[i]).size() ;
	if(size==0) return true;
	else return false; 
}


template class  MuonDigitizationData<MuonPhyChannelInput>;
template class  MuonDigitizationData<MuonPhysicalChannel>;
template class  MuonDigitizationData<MuonPhysicalChannelOutput>;
#endif


