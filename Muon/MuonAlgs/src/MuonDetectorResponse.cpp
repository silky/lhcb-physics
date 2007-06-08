
// $Id: MuonDetectorResponse.cpp,v 1.10 2005/04/05 09:21:08 cattanem Exp 

#include <vector>
#include <cmath>

#include "GaudiKernel/SmartDataPtr.h"       
#include "GaudiKernel/MsgStream.h"  

#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonReadoutCond.h"  
#include "MuonDet/DeMuonDetector.h"

#include "MuonChamberResponse.h" 
#include "MuonDetectorResponse.h"
#include "MuonPhysicalChannelResponse.h" 
#include "MuonPhChID.h"

static std::string spill[] = {"","/Prev","/PrevPrev","/Next","/NextNext"};
static std::string numreg[] = {"1","2","3","4"};
static std::string numsta[] = {"1","2","3","4","5"};

static std::string geoBase="/dd/Conditions/ReadoutConf/Muon/";





void MuonDetectorResponse::initialize(IToolSvc* toolSvc,IRndmGenSvc * randSvc,
                                      IDataProviderSvc* detSvc, 
                                      IMessageSvc * msgSvc){  
  	MsgStream log(msgSvc, "MuonDetectorResponse"); 
  SmartDataPtr<DeMuonDetector>  mudd(detSvc,DeMuonLocation::Default);
  m_muonDetector=mudd;
  
  
  // MuonGeometryStore::Parameters usefull( toolSvc,detSvc, msgSvc);
  //  toolSvc->retrieveTool("MuonGetInfoTool",m_pGetInfo);
  //if(sc.isFailure())return StatusCode::FAILURE;
  m_toolSvc=toolSvc;  
  // usefullPointer= 
  //new MuonGeometryStore::Parameters( toolSvc,detSvc, msgSvc);
	m_gaussDist.initialize( randSvc, Rndm::Gauss(0.0,1.0));
	m_flatDist.initialize( randSvc, Rndm::Flat(0.0,1.0)); 
	std::vector<double> alreadyInsertedValue;
  std::string regionName[20];
  MuonBasicGeometry basegeometry(detSvc,msgSvc); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();
  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    log<<MSG::DEBUG<<" station "<<i<<" "<<numsta[i]<<endreq;
    i++;    
  }
  m_partition=basegeometry.getPartitions();    
  
  for(i=0;i<m_stationNumber;i++){
    for (int k=0;k<m_regionNumber;k++){
      regionName[i*4+k]=geoBase+numsta[i]+"R"+numreg[k]+"Readout";
      log<<MSG::DEBUG<<"region name "<<regionName[i*4+k]<<endreq;
    }
  }
  
  //	std::vector<Rndm::Numbers*>::iterator iterOnPoisson;
  //	bool found=false;
	double newMean;
  for (int indexRegion=0;indexRegion<m_partition;indexRegion++){    
    std::string pathReadout=regionName[indexRegion];
    int station=indexRegion/4;
    int region=indexRegion-station*4;
    
    SmartDataPtr<MuonReadoutCond>  muReadout(detSvc,
                                             pathReadout);
    log<<MSG::DEBUG<<" detRegionPointer "<<endreq;
    if(muReadout)log<<MSG::VERBOSE<<" gg "<<endreq;
    newMean=muReadout->chamberNoise(0)/100; //tranform from cm^2 to mm^2
    Rndm::Numbers*	poissonDist=new Rndm::Numbers;
    //int areadout=0;
    double areaOfChamber =m_muonDetector-> areaChamber(station, region);
    int	 numberOfGaps=m_muonDetector-> gapsInRegion(station, region);
    double totalArea=areaOfChamber* numberOfGaps;
    double meanOfNoisePerChamber=newMean*totalArea*25*1.0E-9;
    log<<MSG::DEBUG<<"region name "<<regionName[indexRegion]
       <<" noise level "<<meanOfNoisePerChamber<<endreq;			
    StatusCode sc=poissonDist->initialize( randSvc, Rndm::Poisson(meanOfNoisePerChamber));  
    if(sc.isFailure())log<<MSG::DEBUG<<" not able to ini poisson dist "<<endreq;
    m_poissonDist.push_back(poissonDist);
    
    MuonChamberResponse* responseOfChamber= new 
      MuonChamberResponse(&m_flatDist,poissonDist,newMean );
    responseChamber[indexRegion]=responseOfChamber;
    log<<MSG::DEBUG<<" stat "<<station<<" "<<region<<
      " "<<m_muonDetector->readoutInRegion(station,region)<<endreq;
    
    
    for(int readout=0;readout<m_muonDetector->
          readoutInRegion(station, region);readout++){
      double min,max; 
      std::vector<double> timeJitterPDF=muReadout->
        timeJitter( min, max ,  readout);
      log<<MSG::VERBOSE<<" time jitter "<<min<<" "<<max<<endreq;
      
      Rndm::Numbers*	m_time=new Rndm::Numbers;
      StatusCode sc=m_time->initialize(randSvc, Rndm::DefinedPdf(timeJitterPDF,0));
      if(sc.isFailure())log<<MSG::DEBUG<<" not able to ini time jitter "<<endreq;

      m_timeJitter.push_back(m_time);
      std::vector<double>::iterator itPDF;
      for (itPDF=timeJitterPDF.begin();itPDF<timeJitterPDF.end();itPDF++){		
     	log<<MSG::VERBOSE<<" time jitter "<<*itPDF<<endreq;
      }
      Rndm::Numbers*	p_electronicNoise=new Rndm::Numbers;
      double noiseCounts=muReadout->electronicsNoise(readout)*
        m_muonDetector->getPhChannelNX( readout,station, region)*
        m_muonDetector->getPhChannelNY( readout,station, region)*25*1.0E-9;
log<<MSG::VERBOSE<<"noisecounts "<<noiseCounts<<endreq;
      sc=p_electronicNoise->initialize( randSvc, Rndm::Poisson(noiseCounts));	
      m_electronicNoise.push_back(p_electronicNoise);	
      if(sc.isFailure())log<<MSG::DEBUG<<" not able to ini ele noise dist "<<endreq;

      MuonPhysicalChannelResponse* response= new
        MuonPhysicalChannelResponse(&m_flatDist,&m_gaussDist,
                                    m_time,p_electronicNoise,min,max,
                                    muReadout, readout );
log<<MSG::DEBUG<<"inserting "<<readout<<" "<<indexRegion<<endreq;
      responseVector[readout][indexRegion]=response;
log<<MSG::DEBUG<<"inserted "<<endreq;
    }
  }  
  
}




MuonDetectorResponse::~MuonDetectorResponse(){
  for(int indexRegion=0;indexRegion<m_partition;indexRegion++){
    int station=indexRegion/4;
    int region=indexRegion-station*4;
	  delete responseChamber[indexRegion];
		delete m_poissonDist[indexRegion];
    for(int readout=0;readout<=m_muonDetector->readoutInRegion(station, region);
        readout++){
      delete responseVector[readout][indexRegion];      
		}
	}
  std::vector<Rndm::Numbers*>::iterator  iterJitter;
  for(iterJitter=m_timeJitter.begin();iterJitter<m_timeJitter.end();
      iterJitter++){
    (*iterJitter)->finalize();
    delete *iterJitter;    
  }
 
  std::vector<Rndm::Numbers*>::iterator iterNoise;	
	for(iterNoise=m_electronicNoise.begin();iterNoise<m_electronicNoise.end();
      iterNoise++){
    (*iterNoise)->finalize();
    delete *iterNoise;
	}
  //  if( m_pGetInfo ) m_toolSvc->releaseTool( m_pGetInfo );
   // delete m_pGetInfo;  
}



MuonPhysicalChannelResponse* MuonDetectorResponse::getResponse(MuonPhChID& 
                                                               phChID ){

//        MsgStream log(msgSvc, "MuonDetectorResponse");

    int station=phChID.getStation();
    int region=phChID.getRegion(); 
    int part=station*4+region; 
    unsigned int readoutType=phChID.getReadout();
    int readout=-1;
    for(int ireadout=0;ireadout<m_muonDetector->readoutInRegion(station, region);ireadout++){

    if(readoutType==m_muonDetector->getReadoutType(ireadout,station, region)){
      readout=(int)ireadout;
    }   
  }
//std::cout<<"deter "<<station<<" "<<region<<" "<<readoutType<<" "<<readout<<std::endl;
//  	if(part==4)cout<<" dentro detector response "<<readoutType<<" " 
//  << readout<<" part "<<endl;
  if(readout>=0){    
    return responseVector[readout][part]; 
  }else{
    return 0; }	
}
