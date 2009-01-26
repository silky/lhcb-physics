//$Id: MuonDigitization.cpp,v 1.42 2009-01-26 15:14:53 cattanem Exp $

#include <algorithm>
#include <vector>
#include <cmath>

#include "GaudiKernel/AlgFactory.h"  
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/SmartIF.h"

#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/DeMuonGasGap.h"

#include "MuonDigitization.h"
#include "MuonChamberResponse.h" 
#include "MuonPhPreInput.h"
#include "SortPhChID.h" 
#include "ComparePC.h"

DECLARE_ALGORITHM_FACTORY( MuonDigitization );

//reserve space for static variable
std::string MuonDigitization::spill[6] = 
{"/LHCBackground","","/Prev","/PrevPrev","/Next","/NextNext"};
std::string MuonDigitization::numreg[4] = 
{"1","2","3","4"};
std::string MuonDigitization::numsta[5]= 
{"1","2","3","4","5"};
 std::string MuonDigitization::TESPathOfHitsContainer[4]=
{"Hits","ChamberNoiseHits","FlatSpilloverHits",
"BackgroundHits"};
const int MuonDigitization::OriginOfHitsContainer[5]=
{LHCb::MuonOriginFlag::GEANT,LHCb::MuonOriginFlag::CHAMBERNOISE,
 LHCb::MuonOriginFlag::FLATSPILLOVER,LHCb::MuonOriginFlag::BACKGROUND, 
LHCb::MuonOriginFlag::LHCBACKGROUND};

MuonDigitization::MuonDigitization(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  :  GaudiAlgorithm(name, pSvcLocator)
{
  //declareProperty("NmbOfSpilloverEvents" , m_numberOfSpilloverEvents=3) ;
  declareProperty("BXTime" , m_BXTime=25.0) ;
  declareProperty("TimeGate" , m_gate=25.0) ;
  declareProperty("TimeBits" , m_TimeBits=4) ;
  declareProperty("VerboseDebug" , m_verboseDebug=false) ;
  declareProperty("ApplyTimeJitter" , m_applyTimeJitter=true) ;
  declareProperty("ApplyChamberNoise" , m_applyChamberNoise=false) ;
  declareProperty("ApplyElectronicNoise" , m_applyElectronicNoise=true) ;
  declareProperty("ApplyXTalk" , m_applyXTalk=true) ;
  declareProperty("ApplyEfficiency" , m_applyEfficiency=true) ;
  declareProperty("ApplyDeadtime" , m_applyDeadtime=true) ;  
  declareProperty("ApplyDialogDeadtime" , m_applyDialogDeadtime=true) ;
  declareProperty("ApplyTimeAdjustment",m_applyTimeAdjustment=true);
  declareProperty("RegisterPhysicalChannelOutput",
                   m_registerPhysicalChannelOutput=false);

}

StatusCode MuonDigitization::initialize()
{  
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by  GaudiAlgorithm	
      
  // Get the number of spillover events from the SpilloverAlg
  IAlgManager* algmgr =svc<IAlgManager>("ApplicationMgr");
  IAlgorithm*  spillAlg;
  sc = algmgr->getAlgorithm( "SpilloverAlg", spillAlg );
  if( !sc.isSuccess() ) {
    warning() << "SpilloverAlg not found" << endmsg;
    m_numberOfSpilloverEvents = 0;
  }
  else {
    SmartIF<IProperty> spillProp( spillAlg );
    if( !spillProp ) {
      warning() << "Problem locating properties of SpilloverAlg" << endmsg;
    }
    else {
      StringArrayProperty evtPaths;
      sc=evtPaths.assign( spillProp->getProperty("PathList") );
      if( !sc.isSuccess() ) {
        warning()<<" problem in spillover "<<endmsg;
      }
      m_numberOfSpilloverEvents = evtPaths.value().size();
    }
    
    // Release the interfaces no longer needed
    spillAlg->release();
  }

  algmgr->release();

  info() << "number of spillover events read from aux stream "
      << m_numberOfSpilloverEvents << endmsg;
  m_numberOfEvents=m_numberOfSpilloverEvents+1;
  m_numberOfEventsNeed=5;	
  // m_numberOfEvents=5;
//  sc = toolSvc()->retrieveTool( "MuonTileIDXYZ", m_pMuonTileXYZ );
 // if( !sc.isSuccess() ) {
   // err() << "Failed to retrieve MuonTileIDXYZ tool" << endmsg;
   // return sc;
  //}
  // initialize some basic geometrical information
  MuonBasicGeometry basegeometry(detSvc(),msgSvc()); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();
  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    debug()<<" station "<<i<<" "<<numsta[i]<<endreq;
    i++;    
  }
  m_partition=basegeometry.getPartitions();
  //  sc=toolSvc()->retrieveTool("MuonGetInfoTool",m_pGetInfo);
  //if(sc.isFailure())return StatusCode::FAILURE;
  
  debug()<<"qui"<<endreq;
  //  MuonGeometryStore::Parameters usefull( toolSvc(), 
  //                                                detSvc(), msgSvc());
  //debug()<<m_pGetInfo->getChamberPerRegion(0)<<endreq;
  debug()<<" ciao "<<endreq;
  sc=m_flatDist.initialize( randSvc(), Rndm::Flat(0.0,1.0));	 
  if(sc.isFailure())warning()<<" error in flat ini"<<endmsg;
  debug()<<"due "<<endreq; 
  detectorResponse.initialize( toolSvc(),randSvc(), detSvc(), msgSvc());
  debug()<<" detectorResponseInitialized "<<endreq;
  m_spill=6;
  m_container=4;
  unsigned int count=1;
  for (int i=0;i<4;i++){
    count=count*2;    
  }
  debug()<<"tre "<<endreq;
  m_timeBin=m_BXTime/(count);  
  debug()<<" fine "<<endreq;
  m_muonDetector=getDet<DeMuonDetector>(DeMuonLocation::Default);
  return StatusCode::SUCCESS;
}
 
 					
StatusCode MuonDigitization::execute()
{

MsgStream log(msgSvc(), name());
 	StatusCode sc=StatusCode::SUCCESS ;
  
  debug()<<"starting the Muon Digitization algorithm "<<endreq;


  SmartDataPtr<LHCb::MCHits> 
    hitPointer(eventSvc(),LHCb::MCHitLocation::Muon);
  
  
  if(hitPointer!=0){
    LHCb::MCHits::const_iterator i;
    for (i=(hitPointer)->begin();i<(hitPointer)->end();i++){         
      if(m_verboseDebug){
        info()<<"muon x , y, z , exit  "<< (*i)->exit().x() <<" " <<
          (*i)->exit().y() << "  " <<                          
          (*i)->exit().z() << endreq ;																		
        debug()<<"muon x , y, z entry ,  "<< (*i)->entry().x() <<" " <<
          (*i)->entry().y() << "  " <<(*i)->entry().z() << endreq ;   
        debug()<<"time of flight ,  "<< (*i)->time()<< endreq;
        int det=(*i)->sensDetID(); 
        debug()<<" chamber and gap ID	"<<
          m_muonDetector->chamberID(det)<<" "<<
          m_muonDetector->gapID(det)<<endreq;
        const LHCb::MCParticle* origparticle=(*i)->mcParticle();
        if(origparticle)  {
          debug()<<"Particle from which it originates (PDG code)"<<
            origparticle->particleID().abspid()<<endreq;
        }
        else{
          warning()<<
            "Particle from which it originates is not defined "<< endreq;
        }
      }	
    }
	}	
	else{
    err()<<"unable to retrieve the hit container"<<endreq; 
  } 
    

	
	if(m_applyChamberNoise) sc=addChamberNoise();
	if(sc.isFailure())return StatusCode::FAILURE;
	
	MuonDigitizationData<MuonPhyChannelInput> PhyChaInput("MuPI",&log,
                                                        eventSvc(),"MuPI") ;
	sc=createInput(PhyChaInput);
	if(sc.isFailure())return StatusCode::FAILURE;	
 	MuonDigitizationData<MuonPhysicalChannel> PhyChaOutput("MuPO",&log,
                                                         eventSvc(),"MuPO") ;
  sc=elaborateMuonPhyChannelInputs(PhyChaInput,PhyChaOutput);	
	if(sc.isFailure())return StatusCode::FAILURE;	 	
	if(m_applyElectronicNoise)	sc=addElectronicNoise(PhyChaOutput);
	if(sc.isFailure())return StatusCode::FAILURE;	
  
  debug()<<"qui "<<endreq;  
  sc=applyPhysicalEffects(PhyChaOutput);
  //info()<<" quiquo"<<endreq;
	if(sc.isFailure())return StatusCode::FAILURE;	
  debug()<<" ale "<<endreq;
  if(m_verboseDebug){	
    for(int i=0; i<m_partition; i++){
	    info()<<"  last print container number "<<i<<endreq;
      MuonPhysicalChannels::const_iterator iter=
        PhyChaOutput.getPartition(i)->begin();
		  for(iter=PhyChaOutput.getPartition(i)->begin();iter<
            PhyChaOutput.getPartition(i)->end();iter++){		    
        info()<<"#FE ID "<<(*iter)->phChID()->getID()<<endreq;
        info()<<"#Station "<<(*iter)->phChID()->getStation()<<endreq;
        info()<<"#Region "<<(*iter)->phChID()->getRegion()<<endreq;
        //info()<<"#Quadrant "<<(*iter)->phChID()->getQuadrant()<<endreq;
        info()<<"#Chamber "<<(*iter)->phChID()->getChamber()<<endreq;
        info()<<"# ch X "<<(*iter)->phChID()->getPhChIDX()<<endreq;
        info()<<"# ch Y "<<(*iter)->phChID()->getPhChIDY()<<endreq;
        info()<<"#frontend "<<(*iter)->phChID()->getFrontEnd()<<endreq;
      }
    }
    
  }
  
  if(m_registerPhysicalChannelOutput)
    PhyChaOutput.registerPartitions();  
 
	MuonDigitizationData<MuonPhysicalChannelOutput> 
    PhysicalChannelOutput("MULC",&log,eventSvc(),"MULC") ;
	sc=fillPhysicalChannel(PhyChaOutput,PhysicalChannelOutput);
	if(sc.isFailure())return StatusCode::FAILURE;	 			
  if(m_verboseDebug){	
    for(int i=0; i<m_partition; i++){
	    info()<<"  last print container number "<<i<<endreq;
      MuonPhysicalChannelOutputs::const_iterator iter=
        PhysicalChannelOutput.getPartition(i)->begin();
		  for(iter=PhysicalChannelOutput.getPartition(i)->begin();iter<
            PhysicalChannelOutput.getPartition(i)->end();iter++){		    
        info()<<"FE ID "<<(*iter)->phChID()->getID()<<endreq;
        info()<<"Station "<<(*iter)->phChID()->getStation()<<endreq;
        info()<<"Region "<<(*iter)->phChID()->getRegion()<<endreq;
        //info()<<"Quadrant "<<(*iter)->phChID()->getQuadrant()<<endreq;
        info()<<"Chamber "<<(*iter)->phChID()->getChamber()<<endreq;
        info()<<" ch X "<<(*iter)->phChID()->getPhChIDX()<<endreq;
        info()<<" ch Y "<<(*iter)->phChID()->getPhChIDY()<<endreq;
        info()<<" frontend "<<(*iter)->phChID()->getFrontEnd()<<endreq;
        info()<<" fired "<<(*iter)->phChInfo().isAlive()<<endreq;
        info()<<" nature "<<(*iter)->phChInfo().natureOfHit()<<endreq;
        std::vector<MuonHitTraceBack> vector_traceBack=(*iter)->
          hitsTraceBack();
        std::vector<MuonHitTraceBack>::iterator iterTraceBack=
          (vector_traceBack).begin(); 
        for(iterTraceBack=(vector_traceBack).begin();iterTraceBack<
              (vector_traceBack).end();iterTraceBack++){
          info()<<"hit time "<<(*iterTraceBack).hitArrivalTime() 
             <<endreq; 
        }
		  }
    }
	}

	MuonDigitizationData<MuonCardiacChannelOutput> 
    CardiacChannelOutput("MUCC",&log,eventSvc(),"MUCC") ;
  //debug()<<"pappa hk "<<endreq; 
	sc=fillCardiacChannel(PhysicalChannelOutput,CardiacChannelOutput);
	if(sc.isFailure())return StatusCode::FAILURE;	 	
  LHCb::MCMuonDigits* mcDigitContainer= new LHCb::MCMuonDigits;
  //bool test=true;
  // debug()<<"pappa rrr"<<endreq; 
  if(m_applyDialogDeadtime){
    
    sc=createLogicalChannel(CardiacChannelOutput, *mcDigitContainer);
  }
  else{
    sc=createLogicalChannel(PhysicalChannelOutput, *mcDigitContainer);
  }

	if(sc.isFailure())return StatusCode::FAILURE;	 			
	put( mcDigitContainer, LHCb::MCMuonDigitLocation::MCMuonDigit );
  LHCb::MuonDigits* digitContainer= new LHCb::MuonDigits;	
	sc=createRAWFormat(*mcDigitContainer, *digitContainer);
	if(sc.isFailure())return StatusCode::FAILURE;	 				
	put( digitContainer, LHCb::MuonDigitLocation::MuonDigit );
  
 	debug()<<"End of the Muon Digitization"<<endreq;
  return StatusCode::SUCCESS;
  
}

StatusCode MuonDigitization::finalize()
{
  // Release the tools
  //if( m_pMuonTileXYZ ) toolSvc()->releaseTool( m_pMuonTileXYZ );
  //if( m_pGetInfo ) toolSvc()->releaseTool( m_pGetInfo );
  detectorResponse.finalize();
  
  return GaudiAlgorithm::finalize();
}

StatusCode MuonDigitization::addChamberNoise(){

  MsgStream log(msgSvc(), name()); 
  int container=1;
  for (int ispill=1;ispill<m_numberOfEvents+1;ispill++){
		//double shiftOfTOF=-m_BXTime*ispill;
    ObjectVector<LHCb::MCHit>* hitsContainer = new 
      ObjectVector<LHCb::MCHit>();
    std::string path="/Event"+spill[ispill]+"/MC/Muon/"+"/"
      +TESPathOfHitsContainer[container];
    for(int k=0;k<m_stationNumber;k++){ 
      for(int s=0;s<m_regionNumber;s++){
        
        int partitionNumber=k*m_regionNumber+s;
        int chamberInRegion=m_muonDetector->chamberInRegion(k,s);
        int gapPerChamber=m_muonDetector->gapsInRegion(k,s);
        for (int chamber=0;chamber<chamberInRegion;chamber++){			   
          int numberOfNoiseHit=(detectorResponse.getChamberResponse
                                (partitionNumber))->extractNoise();          
          // the readout number is essentially meaningless... 
          //it is chamber noise.....					 
          if(numberOfNoiseHit==0)continue;
          DeMuonChamber* p_chamber=m_muonDetector->getChmbPtr(k, s,chamber);
          for (int hit=1;hit<=numberOfNoiseHit;hit++){
            //define position of hit 
            //first gap number
            int gap=(int)((m_flatDist()*gapPerChamber)); 
            int gapMax=m_muonDetector->gapsInRegion(k,s);
            if(gap>gapMax-1)gap=gapMax-1;
            IDetectorElement::IDEContainer::iterator itGap;
            int countGap=0;
            DeMuonGasGap*  p_Gap=NULL;
            for(itGap=(p_chamber)->childBegin(); 
                itGap<(p_chamber)->childEnd(); 
                itGap++){
              // debug()<<(*itGap)->name()<<endmsg;
              
              if(countGap==gap){
                p_Gap=dynamic_cast<DeMuonGasGap*>(*((*itGap)->childBegin()));
                float zhalfgap= 1.0;
                
                if(p_Gap!=NULL){
                  const ISolid* gapSolid=(p_Gap->geometry())->lvolume()
                    ->solid();
                  const SolidBox* gapBox = 
                    dynamic_cast<const SolidBox *>(gapSolid);
                  zhalfgap= gapBox->zHalfLength() ; 
                }
                
                //then x&y	
                double x = m_flatDist()* 
                m_muonDetector->getSensAreaX(k,s);
                double y = m_flatDist()*  
                  m_muonDetector->getSensAreaY(k,s);
                double time = m_flatDist()*m_BXTime;	
                LHCb::MCHit* pHit = new LHCb::MCHit();
                //traslate xyz e local to global..
                
                Gaudi::XYZPoint point1=(p_Gap->geometry())->
                  toGlobal(Gaudi::XYZPoint(x,y,-1*zhalfgap+0.1));
                Gaudi::XYZPoint point2=(p_Gap->geometry())->
                  toGlobal(Gaudi::XYZPoint(x,y,zhalfgap-0.1));
                float z=(point1.z()+point2.z())/2.0F;
                
                //              p_chamber->
                
                pHit->setEntry(point1);
                pHit->setDisplacement(point2-point1);
                double tofOfLight=(sqrt(x*x+ y*y+z*z))/300.0;
                pHit->setTime(time+tofOfLight);
                Gaudi::XYZPoint hitMid=
                  Gaudi::XYZPoint((point1.x()+point2.x())/2.0F,
                                (point1.y()+point2.y())/2.0F,
                                  (point1.z()+point2.z())/2.0F);
                
                int sen=m_muonDetector->sensitiveVolumeID(hitMid);
                pHit->setSensDetID(sen);
                hitsContainer->push_back(pHit);
                if(m_verboseDebug){	
                  info()<<"adding chamber noise hit "<<
                    ispill<<" "<<k<<" "<<s<<numberOfNoiseHit<<endreq;
                }
                continue;              
              }            
            countGap++;            
            }
          }
        
          
          
          if(m_verboseDebug){	
            if(numberOfNoiseHit>0)
              info()<<"adding chamber noise hit "<<ispill<<" "<<
                k<<" "<<s<<" chamber "<<chamber <<" "<<
                numberOfNoiseHit<<endreq;
          }	
          
        }         
      }
    }
    eventSvc()->registerObject(path,hitsContainer);
    
  }
  
  return StatusCode::SUCCESS;
}





StatusCode
MuonDigitization::createInput(
                              MuonDigitizationData<MuonPhyChannelInput>& 
                              PhyChaInput)
{
  MsgStream log(msgSvc(), name()); 
  
  //loop over the containers
  std::vector<MuonPhPreInput> keepTemporary[20] ;	
  //  for(int iterRegion=0;iterRegion<m_partition;iterRegion++){    
    //  std::vector<MuonPhPreInput> keepTemporary ;		 
    //int station=iterRegion/m_regionNumber+1;
    //int region=iterRegion%m_regionNumber+1;   
  for (int ispill=0; ispill<=m_numberOfEvents;ispill++){
    long spillTime=0;     
    if(ispill==0){       
    }else {
      spillTime=(long)(-(ispill-1)*m_BXTime);
    }
     
    for(int container=0; container<m_container;container++){				
      std::string path="/Event"+spill[ispill]+"/MC/Muon/"+
        TESPathOfHitsContainer[container];
      if(m_verboseDebug) {info()<<"hit container path "<<
                            path<<endreq;}
      
      SmartDataPtr<LHCb::MCHits> hitPointer(eventSvc(),path);
      LHCb::MCHits::const_iterator iter;	 
      if(hitPointer!=0){
        for (iter=(hitPointer)->begin();iter<(hitPointer)->end();iter++){
          std::vector< std::pair<MuonFrontEndID, std::vector<float> > > 
            listph;
          debug()<<"eccoci "<<endreq;
      	  int det=(*iter)-> sensDetID();
          if(det<0)continue;
          
          unsigned int hitStation=m_muonDetector->stationID(det);
          unsigned int hitRegion=m_muonDetector->regionID(det);          
          unsigned int hitChamber=m_muonDetector->chamberID(det);
          unsigned int hitGap=m_muonDetector->gapID(det);
          unsigned int hitQuarter=m_muonDetector->quadrantID(det);
          listph= m_muonDetector->listOfPhysChannels((*iter)->entry(),
                                                     (*iter)->exit(), 
                                                     hitRegion,hitChamber);
          std::vector< std::pair<MuonFrontEndID, std::vector<float> > >::
            iterator itPh;
          verbose()<<" ga hit "<<hitStation<<" "<<hitRegion<<" "<<hitChamber<<" "
                <<hitGap<<" "<<hitQuarter<<endmsg;
          
          for(itPh=listph.begin();itPh<listph.end();itPh++){
            MuonFrontEndID fe=(*itPh).first;
            std::vector<float> dist=(*itPh).second;    
            double distanceFromBoundary[4];
            distanceFromBoundary[0]=dist[0];
            distanceFromBoundary[1]=dist[1];
            distanceFromBoundary[2]=dist[2];
            distanceFromBoundary[3]=dist[3];
            
            MuonPhPreInput* inputPointer = new  MuonPhPreInput;           
          //  LHCb::MuonTileID tile ;            
          //  m_muonDetector->Chamber2Tile(hitChamber, hitStation,  
           //                              hitRegion,tile);
           // unsigned int hitQuarter=tile.quarter();
            //             info()<<" hitQuarter "<<hitQuarter<<" "<<hitStation<<" "<<
            //  hitRegion<<" "<<hitChamber<<" "<<hitGap<<endreq;
            debug()<<" adding pch "<<hitStation<<" "<<
              hitRegion<<" "<<hitChamber<<" "<<hitQuarter<<"  "<<fe.getReadout()<<endreq;
            
            inputPointer->phChID()->setStation(hitStation);
            inputPointer->phChID()->setRegion(hitRegion);
            inputPointer->phChID()->setQuadrant(hitQuarter);
            inputPointer->phChID()->setChamber(hitChamber);
            
            inputPointer->phChID()->setPhChIDX(fe.getFEIDX());
            inputPointer->phChID()->setPhChIDY(fe.getFEIDY());					
            inputPointer->phChID()->setFrontEnd(fe.getLayer());
            inputPointer->phChID()->setReadout(fe.getReadout());
            inputPointer->phChID()->setGap(hitGap);
            inputPointer->getHitTraceBack()->setMCHit(*iter); 
            
            //correct for the  tof .... i.e. subtract the tof that 
            //a lightparticle impacting the center of the pc. has....
            
            double xcenter, ycenter,zcenter;
            //info()<<"before pccenter"<<hitChamber<<" "<<hitStation<<" "<<
            //hitRegion<<" "<<hitGap<<" "<<fe.getFEIDX()<<" "<<fe.getFEIDY()<<endreq;             
            //info()<<" dete "<<m_muonDetector->getPhChannelNX(0,hitStation,hitRegion)<<
            //" "<<m_muonDetector->getPhChannelNY(0,hitStation,hitRegion)<<endreq;
            
            
            StatusCode sc=m_muonDetector->
                          getPCCenter(fe,hitChamber,hitStation, hitRegion ,
                                        xcenter, ycenter,zcenter);
            if(sc.isFailure())warning()<<" getpc ch error"<<endmsg;
            double tofOfLight=sqrt(xcenter*xcenter+ycenter*ycenter+
                                   zcenter*zcenter)/300.0;
            //info()<<" tof "<<tofOfLight<<" "<<hitStation<<" "<<hitRegion<<" "<<zcenter<<endreq;
            inputPointer->getHitTraceBack()
              ->setHitArrivalTime((*iter)->time()+globalTimeOffset()+
                                  +spillTime-tofOfLight+0.5);
            inputPointer->getHitTraceBack()
              ->setCordinate(distanceFromBoundary);
            if(ispill>0){
              inputPointer->getHitTraceBack()->getMCMuonHitOrigin().
                setBX(ispill-1);
              inputPointer->getHitTraceBack()
                ->getMCMuonHitOrigin().setHitNature
                (OriginOfHitsContainer[container]);	 
              inputPointer->getHitTraceBack()
                  ->getMCMuonHistory().setBXOfHit(ispill-1);
              inputPointer->getHitTraceBack()
                 ->getMCMuonHistory().setNatureOfHit
                (OriginOfHitsContainer[container]);			
            }
            //patch for machine background
            if(ispill==0){
              inputPointer->getHitTraceBack()->getMCMuonHitOrigin().
                setBX(ispill); 
              inputPointer->getHitTraceBack()
                ->getMCMuonHitOrigin().setHitNature
                (OriginOfHitsContainer[4]);	 
              inputPointer->getHitTraceBack()
                ->getMCMuonHistory().setBXOfHit(ispill);
              inputPointer->getHitTraceBack()
                ->getMCMuonHistory().setNatureOfHit
                (OriginOfHitsContainer[4]);			
            }
            
            
            inputPointer->getHitTraceBack()->setCordinate
              (distanceFromBoundary);
            if(m_verboseDebug){	
               int code=0;
               if((*iter)->mcParticle())code=(*iter)
                                          ->mcParticle()->particleID().
                                          abspid();                    
               info()<<"hit processing "<<hitStation<<" "<< 
                 hitRegion<<" " << hitQuarter<<" "<< hitChamber
                     <<" "<<hitGap<<" "<<fe.getLayer()
                     <<" "<<fe.getReadout()<<" "                
                     <<" "<<tofOfLight<<" "<< 
                 OriginOfHitsContainer[container]<<" "<<
                 ispill<<" "<< code<<endreq;		
               info()<<" ph ch ID "<<
                 *(inputPointer->phChID())<<" id "<<
                 inputPointer->phChID()->getID()<<endreq;
            }	 	
             //info()<<" before keep "<<endreq;
            keepTemporary[hitStation*4+hitRegion].push_back(*inputPointer);
            delete inputPointer;
            //info()<<" last "<<endreq;
          }	//info()<<" last 1"<<endreq;
        }      //   	info()<<" last2 "<<endreq;
      }			//info()<<" last3 container "<<container<<endreq;
    }      	//info()<<" last4 spill "<<ispill<<endreq;
  }	 	//info()<<" last5 "<<station<<" "<<region<<endreq; 
  
  
  
  //info()<<"qui "<<endreq;
  
  for(int iterRegion=0;iterRegion<m_partition;iterRegion++){     
    if(m_verboseDebug){
      debug()<<"pre-sorted vector"<<endreq;}
    //		std::vector<MuonPhPreInput>::reverse_iterator iterPre;		
    std::stable_sort(keepTemporary[iterRegion].begin(),
                     keepTemporary[iterRegion].end(),SortPhChID());
    if(m_verboseDebug){
      debug()<<"sorted vector"<<endreq;}
    std::vector<MuonPhPreInput>::reverse_iterator iterPost;    
    for(iterPost=keepTemporary[iterRegion].rbegin();
        iterPost<keepTemporary[iterRegion].rend();
        ++iterPost){
      if(m_verboseDebug){
        /*           debug()<<"ID  Post "<< 
                     (iterPost)->phChID()->getID()<<
                     " hit origin "<< (iterPost)->getHitTraceBack()
                     ->getMCMuonHitOrigin().getFlatSpilloverNature() 
                     <<endreq;	*/	
      }
      MuonPhyChannelInput* phChPointer = 
        new MuonPhyChannelInput((iterPost)->phChID()->getID(),
                                *((iterPost)->getHitTraceBack())) ;	
      StatusCode asc=PhyChaInput.addMuonObject(iterRegion,phChPointer);
      if(asc.isFailure())debug()<<" not able to add requested obj "<<endreq;
      keepTemporary[iterRegion].pop_back(); 	
    } 	  
  }
  
  //  info()<<" exit "<<endreq;
  
  return StatusCode::SUCCESS;
}


StatusCode MuonDigitization::
elaborateMuonPhyChannelInputs(
                              MuonDigitizationData<MuonPhyChannelInput> & 
                              PhyChaInput,
                              MuonDigitizationData<MuonPhysicalChannel>& 
                              PhysicalChannel){
  MsgStream log(msgSvc(), name()); 					 
  for (int i=0; i<m_partition;i++){
    if(!PhyChaInput.isEmpty(i)){	  
      MuonPhyChannelInputs::const_iterator inputIter ;
      MuonPhyChannelInputs::const_iterator inputIterStart=
        PhyChaInput.getPartition(i)->begin();
      /* 		  int station=i/4+1;
              int region=i%4+1;	 
              std::string text="/Event/MC/Muon/"+numsta[station-1]+
              "/R"+numreg[region-1]+"/Hits";
              DataObject* pPart;
              StatusCode sc=eventSvc()->retrieveObject(text, 
              (DataObject*&)pPart);		*/
      unsigned int prevFE=(*inputIterStart)->phChID()->getFETile();
      unsigned int lastFE;
      //			unsigned int prevID=(*inputIterStart)->phChID()->getID();
      //			unsigned int lastID;
      MuonHitTraceBack* pointerToHitTraceBack=(*inputIterStart)
        ->getHitTraceBack();
      MuonPhysicalChannel* outputPointer = new MuonPhysicalChannel
        (prevFE,m_gate,m_BXTime);
      //if(i==4)cout<<" ciao "<<(*inputIterStart)
      //->phChID()->getReadout()<<endl;
      
      outputPointer->setResponse
        (detectorResponse.getResponse(*((*inputIterStart)->phChID())));
      if(!detectorResponse.getResponse(*((*inputIterStart)->phChID()))){
        err()<<"unable to retrieve the response of  ph. channel"
           <<endreq;
      }
      outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack);
      StatusCode asc=PhysicalChannel.addMuonObject(i,outputPointer);
      if(asc.isFailure()){
        warning()<<" error in inserting obj "<<endmsg;
      }
      for (inputIter=++inputIterStart;inputIter
             <PhyChaInput.getPartition(i)->end();inputIter++){
        lastFE=(*inputIter)->phChID()->getFETile();
        pointerToHitTraceBack=(*inputIter)->getHitTraceBack();
        if(lastFE==prevFE){
          outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack) ;
        }else{
          outputPointer = new MuonPhysicalChannel(lastFE,m_gate,m_BXTime);
          outputPointer->setResponse
            (detectorResponse.getResponse(*((*inputIter)->phChID())));
        
          
          outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack) ;
          prevFE=lastFE;
          StatusCode asc=PhysicalChannel.addMuonObject(i,outputPointer);
          if(asc.isFailure())debug()<<"error in adding obj "<<endreq;        
        }			
      }						
    }
  }
    return StatusCode::SUCCESS;	
}

StatusCode  MuonDigitization::
fillPhysicalChannel(MuonDigitizationData<MuonPhysicalChannel>& 
                    PhysicalChannel,
                    MuonDigitizationData<MuonPhysicalChannelOutput>& 
                    PhysicalChannelOutput){
  MuonPhysicalChannels::iterator iterInput;
  //		  MuonPhysicalChannelOutputs::iterator iterOutput;
  MsgStream log(msgSvc(), name()); 					 
  
  for (int i=0; i<m_partition;i++){
    if(!PhysicalChannel.isEmpty(i)){				
      for(iterInput=PhysicalChannel.getPartition(i)->begin();
          iterInput<PhysicalChannel.getPartition(i)->end();iterInput++){
        MuonPhysicalChannelOutput* objToAdd=new 
          MuonPhysicalChannelOutput(*(*iterInput));
        bool fired=false;
        bool interestingHit=false;
        double timeOfFiring;
        std::vector<MuonHitTraceBack>::iterator iterInHits ;
        if(m_verboseDebug)info()<<objToAdd->phChID()->
                            getID()<<" "<<*(objToAdd->phChID())<<endreq;
        // if(i==0)	cout<<" start nuovo mupnphysicalchannel "<<endl;
        for(iterInHits=objToAdd->hitsTraceBack().begin();
            iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){
          if(m_verboseDebug)info()<<" geo acce "<<
                              (*iterInHits).getMCMuonHistory().
                              isHitOutGeoAccemtance()<<
                              " cha ineff "<<
                              (*iterInHits).getMCMuonHistory().
                                isKilledByChamberInefficiency()<<
                              " in deadtime "<<
                              (*iterInHits).getMCMuonHistory().
                              isHitInDeadtime()<<
                              " time of firing "<< 
                              (*iterInHits).hitArrivalTime()<<endreq;				 
          if(!fired){
            if(!(*iterInHits).getMCMuonHistory().isHitOutGeoAccemtance()&&
               !(*iterInHits).getMCMuonHistory().
               isKilledByChamberInefficiency()&&
               !(*iterInHits).getMCMuonHistory().isHitInDeadtime()){
              timeOfFiring=(*iterInHits).hitArrivalTime();
              if(timeOfFiring>0&&timeOfFiring<m_gate){
                // just test
                //										 if(timeOfFiring>0&&timeOfFiring<20){
                
                fired=true;
                (*iterInHits).getMCMuonHistory().setFiredFlag((unsigned int)1);
                objToAdd->setFiringTime(timeOfFiring);		
                objToAdd->phChInfo().setAliveDigit(1);
                objToAdd->phChInfo().setBXIDFlagHit((*iterInHits).
                                                    getMCMuonHistory().BX());
                objToAdd->phChInfo().setNatureHit((*iterInHits).
                                                  getMCMuonHitOrigin().
                                                  getNature());
                if((*iterInHits).getMCMuonHistory().
                   isHitOriginatedInCurrentEvent()){
                  if((*iterInHits).getMCMuonHistory().
                     isInForTimeAdjustment()){
                    objToAdd->phChInfo().setAliveTimeAdjDigit(1);
                    if((*iterInHits).getMCMuonHistory().hasTimeJittered()){
                      objToAdd->phChInfo().setTimeJitteredDigit(1);
                    }
                  }
                }
              }
            }
          }
        }
        if(m_verboseDebug)info()<<" fired "<<fired<<endreq;
        if(!fired){
          for(iterInHits=objToAdd->hitsTraceBack().begin();
              iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){
            if((*iterInHits).getMCMuonHistory().
               isHitOriginatedInCurrentEvent()){
              interestingHit=true;
              
              if((*iterInHits).getMCMuonHistory().hasTimeJittered()){
                if(!((*iterInHits).getMCMuonHistory().
                     isInForTimeAdjustment())){
                  // first source of dead time jitter                  
                  objToAdd->phChInfo().setTimeJitteredDigit(1);						
                }else if ((*iterInHits).getMCMuonHistory().
                          isHitInDeadtime()){
                  // remember to check that the time adjustment do not 
                  // put back the hit int he gate....=> 
                  // only deadtime can kill this digit 											 
                  objToAdd->phChInfo().setDeadtimeDigit(1);
                }	 
              }
              if((*iterInHits).getMCMuonHistory().
                 isKilledByChamberInefficiency()){
                // hit is killed by chamber inefficiency
                objToAdd->phChInfo().setChamberInefficiencyDigit(1);
              }else   if((*iterInHits).getMCMuonHistory().
                         isHitOutGeoAccemtance()){
                // hit is killed by geomatrial acceptance 
                objToAdd->phChInfo().setGeometryInefficiency(1);
              }else if ((*iterInHits).getMCMuonHistory().isHitInDeadtime()){
                // hit in deadtime
                objToAdd->phChInfo().setDeadtimeDigit(1);
              }else   if((*iterInHits).getMCMuonHistory().
                         isOutForTimeAdjustment()){
                // hit is killed by time adjustment
                objToAdd->phChInfo().setTimeAdjDigit(1);								
              }  
            }
          }
        }
        if(!fired &&! interestingHit){
          iterInHits=objToAdd->hitsTraceBack().begin();
          objToAdd->phChInfo().setAliveDigit(0);
          objToAdd->phChInfo().setBXIDFlagHit((*iterInHits).
                                              getMCMuonHistory().BX());
          objToAdd->phChInfo().setNatureHit((*iterInHits).
                                                  getMCMuonHitOrigin().
                                                  getNature());
          // objToAdd->phChInfo().setSecondPart((*iterInHits).secondPart());
          if((*iterInHits).getMCMuonHistory().hasTimeJittered()){
            if(!((*iterInHits).getMCMuonHistory().
                 isInForTimeAdjustment())){
              // first source of dead time jitter                  
              objToAdd->phChInfo().setTimeJitteredDigit(1);						
            }else if ((*iterInHits).getMCMuonHistory().
                      isHitInDeadtime()){
              // remember to check that the time adjustment do not 
              // put back the hit int he gate....=> 
              // only deadtime can kill this digit 											 
              objToAdd->phChInfo().setDeadtimeDigit(1);
            }	 
          }
          if((*iterInHits).getMCMuonHistory().
             isKilledByChamberInefficiency()){
            // hit is killed by chamber inefficiency
            objToAdd->phChInfo().setChamberInefficiencyDigit(1);
          }else   if((*iterInHits).getMCMuonHistory().
                     isHitOutGeoAccemtance()){
            // hit is killed by geomatrial acceptance 
            objToAdd->phChInfo().setGeometryInefficiency(1);
          }else if ((*iterInHits).getMCMuonHistory().isHitInDeadtime()){
            // hit in deadtime
            objToAdd->phChInfo().setDeadtimeDigit(1);
          }else   if((*iterInHits).getMCMuonHistory().
                     isOutForTimeAdjustment()){
            // hit is killed by time adjustment
            objToAdd->phChInfo().setTimeAdjDigit(1);								
          }  
          
        }


   


     
        // debug printout
        
        bool muon=false;
        for(iterInHits=objToAdd->hitsTraceBack().begin();
            iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){				
          // search the muon first...
          if((iterInHits)->getMCHit()){
            const LHCb::MCParticle* particle=
              (iterInHits)->getMCHit()->mcParticle();
            if(particle){
              int pid= particle->particleID().abspid();
              if(pid==13||pid==-13){
                debug()<<"moun hit   time   ??????"<<
                    (iterInHits)-> hitArrivalTime()	<<endreq;	
                muon=true;
              }
            }
          }							 
        }
        if(muon){
          if(m_verboseDebug)
            info()<<"**** start new pc****   station  and region "
                  <<i <<" fired "<<fired<<endreq;	
          for(iterInHits=objToAdd->hitsTraceBack().begin();
              iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){	
            
            if(m_verboseDebug)info()<<"time"<<(iterInHits)-> 
                                hitArrivalTime()	<<" tile "<<
                                objToAdd->phChID()->getFETile()<<endreq;	
            if(m_verboseDebug)info()<<	" deadtime "<<
                                (*iterInHits).getMCMuonHistory().
                                isHitInDeadtime()<<" time jitter "<<
                                (*iterInHits).getMCMuonHistory().
                                hasTimeJittered() <<" efficiency  "<<
                                (*iterInHits).getMCMuonHistory().
                                isKilledByChamberInefficiency()<<endreq;
          }					  
        }
        // if(fired||interestingHit){  
        objToAdd->fillTimeList();
        StatusCode asc=PhysicalChannelOutput.addMuonObject(i,objToAdd);
        if(asc.isFailure())debug()<<" problem in adding obj "<<endreq;
        
        //}
        //else {
        //  delete 	objToAdd;
        //}        
      }
    }
  }	
  return StatusCode::SUCCESS;					   
}


StatusCode  MuonDigitization::
fillCardiacChannel(MuonDigitizationData<MuonPhysicalChannelOutput>& 
                    PhysicalChannelOutput,
                    MuonDigitizationData<MuonCardiacChannelOutput>& 
                    CardiacChannelOutput){
  MuonPhysicalChannelOutputs::iterator iterInput;
  MuonCardiacChannelOutputs::iterator iterOutput;
  //		  MuonPhysicalChannelOutputs::iterator iterOutput;
  //  MsgStream log(msgSvc(), name()); 					 

  
  for (int i=0; i<m_partition;i++){
    if(!PhysicalChannelOutput.isEmpty(i)){				
      for(iterInput=PhysicalChannelOutput.getPartition(i)->begin();
          iterInput<PhysicalChannelOutput.getPartition(i)->end();iterInput++){
        MuonCardiacChID cardiacChTileID;
        
        (*iterInput)->calculateCardiacORID(cardiacChTileID,
                       m_muonDetector);
        //        info()<<"ii "<<i<<" "<< cardiacChTileID<<endreq;
        bool alreadyExist=false;
        MuonPhysicalChannelOutput* jout=
          static_cast< MuonPhysicalChannelOutput* >(*(iterInput));
        //info()<<((*iterInput)->phChInfo()).isAlive()<<endreq;
      
        if(!CardiacChannelOutput.isEmpty(i)){
           for(iterOutput=CardiacChannelOutput.getPartition(i)->begin();
               iterOutput<CardiacChannelOutput.getPartition(i)->end();
               iterOutput++){
             if(((*iterOutput)->chID())==cardiacChTileID){
               (*iterOutput)->addPhyChannel(jout);
               alreadyExist=true;
             }
             
           }
           
        }
        //info()<<" alreadyExist= "<<alreadyExist<<endreq;
        
        
        if(!alreadyExist){
          
          MuonCardiacChannelOutput* objToAdd=new 
            MuonCardiacChannelOutput(cardiacChTileID);
          objToAdd->addPhyChannel(jout);
          StatusCode asc=CardiacChannelOutput.addMuonObject(i,objToAdd);
          if(asc.isFailure())debug()<<"unable to add chardiac channel "<<endreq;
          
        }
        
      }
      
    }


   if(!CardiacChannelOutput.isEmpty(i)){				
      for(iterOutput=CardiacChannelOutput.getPartition(i)->begin();
          iterOutput<CardiacChannelOutput.getPartition(i)->end();iterOutput++){
        (*iterOutput)-> processForDeadTime(25, m_gate);
        //        (*iterOutput)-> setFiringTime(); 
        //  info()<<" dopo il deadtime "<< (*iterOutput)->chInfo().isAlive()<<endreq;
        
      }
   }
   
  }
  

  //info()<<" pre new ch  size "<<endreq;
  
 
   return StatusCode::SUCCESS;
}

 

StatusCode MuonDigitization::
applyPhysicalEffects(MuonDigitizationData<MuonPhysicalChannel>&
                     PhysicalChannel){
	MsgStream log(msgSvc(), name()); 	
  
  //loop over the 20 containers 
  
  for (int i=0; i<m_partition;i++){	
    //    log<<MSG::INFO<<" part "<<i<<endreq;
    int station=i/4;
    int region=i%4;	 
	  if(!PhysicalChannel.isEmpty(i)){
      //log<<MSG::INFO<<" non empty "<<i<<endreq;
      
		  std::vector<MuonPhysicalChannel*>  XTalkPhysicalChannel;
			std::vector<MuonPhysicalChannel*>::iterator iterOnSTD;
		  MuonPhysicalChannels::iterator iter ;
			std::vector<MuonPhysicalChannel*> channelsDueToXTalk;	   
			std::vector<MuonPhysicalChannel*>::iterator iterXTalk;
			MuonPhysicalChannel* pFound;
			int phChInX[2] ;
			int phChInY[2] ;
      for (int iloop=0; iloop<m_muonDetector->readoutInRegion(station,region);
           iloop++){
			 	phChInX[(int)m_muonDetector->getReadoutType(iloop,station,region)]	=
          m_muonDetector->getPhChannelNX( iloop,station,region);        
			 	phChInY[m_muonDetector->getReadoutType(iloop,station,region)]	=
          m_muonDetector->getPhChannelNY( iloop,station,region);
      }
      //log<<MSG::INFO<<" start iter "<<endreq;
      
      for(iter=PhysicalChannel.getPartition(i)->begin();
			    iter<PhysicalChannel.getPartition(i)->end();iter++){
        //					if(i==0)cout<<" alessia diventa scema "<<endl;
        // apply per pc the time jitter on each hit	
        //  log<<MSG::INFO<<" pre jitter "<<endreq;
        if( m_applyTimeJitter)(*iter)->applyTimeJitter();
        //log<<MSG::INFO<<" jitter "<<endreq;
        // apply per pc the geometry inefficiency 	on each hit			
        if(m_applyEfficiency)(*iter)->applyGeoInefficiency();					 
        //log<<MSG::INFO<<" eff "<<endreq;
        
        // apply per pc the chamber inefficiency 	on each hit				
        if(m_applyEfficiency)(*iter)->applyChamberInefficiency();	
        //log<<MSG::INFO<<" effdue "<<endreq;
        
        // apply per pc the X Talk on each hit				
        // start Xtalk   
        
        if(m_applyXTalk)(*iter)->
                          applyXTalk(phChInX, phChInY,channelsDueToXTalk);
        for(iterXTalk=channelsDueToXTalk.begin();
            iterXTalk<channelsDueToXTalk.end();iterXTalk++){
          pFound=0;
          bool already=PhysicalChannel.isObjectIn(i,*iterXTalk,
                                                  pFound, ComparePC);
          if(!already){
            (*iterXTalk)->setResponse(detectorResponse.
                                      getResponse(*((*iterXTalk)->phChID())));
            XTalkPhysicalChannel.push_back(*iterXTalk);
          }else{
            debug()<<"xtalk hit test  molto dopo "<<
              (*iter)<<" "<<*iterXTalk<<" "<<pFound<<endreq;
            if(pFound==(*iter))err()<<"alessia gran casino"<<endreq;
            pFound->addToPC(*iterXTalk);
            delete *iterXTalk;
          }
        }
        //end Xtalk
        // empty the Xtalked PC container		
        while(!channelsDueToXTalk.empty()){
          channelsDueToXTalk.pop_back();
        }
      }
      // add the xtalk hit to the main container
      
      for(iterOnSTD=XTalkPhysicalChannel.begin();
			    iterOnSTD<XTalkPhysicalChannel.end();iterOnSTD++){
        pFound=0;
        bool already=PhysicalChannel.
          isObjectIn(i,*iterOnSTD,pFound, ComparePC);
        if(already){
          pFound->addToPC(*iterOnSTD); 
          delete *iterOnSTD;
          //						 if(i==0)cout<<" giammai already found "<<endl;
        }
        else{
          StatusCode asc=PhysicalChannel.addMuonObject(i,*iterOnSTD);
          if(asc.isFailure())debug()<<" not able to add xt "<<endreq;
          
          //						 if(i==0)cout<<" real new  "<<endl;
          std::vector<MuonHitTraceBack>::iterator hji ;
          if(i==0){
            for(hji=(*iterOnSTD)->hitsTraceBack().begin();
                hji<(*iterOnSTD)->hitsTraceBack().end();hji++){
            }
          }
        }        
      } 
      // start deadtime
      //log<<MSG::INFO<<" deadtime "<<endreq;
      for(iter=PhysicalChannel.getPartition(i)->begin();
			    iter<PhysicalChannel.getPartition(i)->end();iter++){
        // sort in time the hit of each pc
        std::vector<MuonHitTraceBack> hits=(*iter)->hitsTraceBack();
        std::vector<MuonHitTraceBack>::iterator iterTest;
        (*iter)->sortInTimeHits();		
        // printout for testing
        hits=(*iter)->hitsTraceBack(); 
        if(m_verboseDebug){
          for (iterTest=hits.begin();iterTest<hits.end();iterTest++){
          }
        }			  
        //log<<MSG::INFO<<" deadtime 2"<<endreq;
        
        //apply time adjustment	
        if(m_applyTimeAdjustment)(*iter)->applyTimeAdjustment();	
        //apply deadtime
        if(m_applyDeadtime)(*iter)-> applyDeadtime(m_numberOfEventsNeed) ;
        //log<<MSG::INFO<<" deadtime 3"<<endreq;
			}	
      //end deadtime			 			 			  									 			
	  }				
  }
  
  //log<<MSG::INFO<<" end of all "<<endreq;
  
  return StatusCode::SUCCESS ;	
}






StatusCode MuonDigitization::
createLogicalChannel(MuonDigitizationData<MuonPhysicalChannelOutput>&
                     PhyChaOutput, LHCb::MCMuonDigits& mcDigitContainer){
	MsgStream log(msgSvc(), name()); 
  //  int flag=0;
  int countDigits=0;
  for(int i=0; i<m_partition; i++){
    MuonPhysicalChannelOutputs::const_iterator iter =
      PhyChaOutput.getPartition(i)->begin();
    LHCb::MCMuonDigits::iterator iterDigit;        
    for(iter=PhyChaOutput.getPartition(i)->begin();iter<
          PhyChaOutput.getPartition(i)->end();iter++){	
      if(m_verboseDebug){
        debug()<<"FE ID "<<(*iter)->phChID()->getID()<<endreq;}
      LHCb::MuonTileID phChTileID[2];
      int numberOfTileID;  
      if(m_verboseDebug)info()<<"FE ID "<<
                          (*iter)->phChID()->getID()<<endreq;
      (*iter)->calculateTileID(numberOfTileID,phChTileID,m_muonDetector);
      if( m_verboseDebug)info()<<" after tile calculation " 
                            << numberOfTileID<<" "<<endreq;
      if( m_verboseDebug)info()<<" tile  " << 
                                phChTileID[0]<< phChTileID[1]<<" "<<endreq;
      
      //
      // loop over possible phchtileID (1 or 2 if double mapping)
      //
      for(int iTile=0;iTile<numberOfTileID;iTile++){
        bool found=false; 
        //
        // loop over already created Digits
        //           
        if(m_verboseDebug){
          debug()<<" Loop on mappings "<<
            iTile<<" "<<numberOfTileID<<endreq;}             
        for(iterDigit=mcDigitContainer.begin(); 
            iterDigit<mcDigitContainer.end()&&!found; iterDigit++){
          LHCb::MuonTileID tile=(*iterDigit)->key();
          //
          // tile is the key of the existing Digit, phChTileID[] 
          // is the just created ID of the ph.ch.
          //                
               
          if(tile==phChTileID[iTile]){
            if( m_verboseDebug)info()<<
                                 " Loop on mappings found already "
                                  <<tile<<" "<<endreq;
            if( m_verboseDebug) info()<<"  "<<
                                  (*iterDigit)->DigitInfo().isAlive()
                                        <<" "<<(*iter)->phChInfo().isAlive()
                                   <<endreq;
            found=true;
                 // Digit already exists, update bits and links
            std::vector<MuonHitTraceBack>::iterator iterOnHits;
            std::vector<LHCb::MCMuonHitHistory>::iterator iterOnHitsInDigit;
            if((*iterDigit)->DigitInfo().isAlive()&&
               (*iter)->phChInfo().isAlive()){
              //both fired
              if((*iterDigit)->firingTime()<(*iter)->firingTime()){
                for(iterOnHits=(*iter)->hitsTraceBack().begin();
                    iterOnHits<(*iter)->hitsTraceBack().end();
                         iterOnHits++){
                  (*iterOnHits).getMCMuonHistory().setFiredFlag(0);			 
                }
              }else{
                (*iterDigit)->setFiringTime((*iter)->firingTime());
                (*iterDigit)->DigitInfo().setNatureHit((*iter)
                                                       ->phChInfo().
                                                       natureOfHit());
                (*iterDigit)->DigitInfo().setBXIDFlagHit((*iter)
                                                              ->phChInfo().
                                                         BX());
                for(iterOnHitsInDigit=(*iterDigit)->HitsHistory().begin();
                    iterOnHitsInDigit<(*iterDigit)->HitsHistory().end();
                    iterOnHitsInDigit++){
                       (*iterOnHitsInDigit).setFiredFlag(0);						 
                }
              }
            }
            
            if((*iterDigit)->DigitInfo().isAlive()&&!((*iter)
                                                           ->phChInfo().
                                                      isAlive())){
              // only one is fired	
              
            }
            if(!((*iterDigit)->DigitInfo().isAlive())&&
               ((*iter)->phChInfo().isAlive())){
              // only one is fired	
              (*iterDigit)->setFiringTime((*iter)->firingTime());
              (*iterDigit)->DigitInfo().setNatureHit((*iter)
                                                     ->phChInfo().
                                                     natureOfHit());
              (*iterDigit)->DigitInfo().setBXIDFlagHit((*iter)
                                                       ->phChInfo().
                                                       BX());  
              (*iterDigit)->DigitInfo().setSecondPart(0);   
              (*iterDigit)->DigitInfo().setAliveDigit(1);
                   
              if( m_verboseDebug)info()<<" importante "<<
                                   (*iterDigit)->DigitInfo().isAlive()  
                                    <<endreq;
            }
            if(!((*iterDigit)->DigitInfo().isAlive())&&
               !((*iter)->phChInfo().isAlive())){
              if( m_verboseDebug)info()<<
                                   " molto importante "<<
                                   (*iterDigit)->DigitInfo().isAlive()  
                                    <<endreq;                   
              // both not fired
              if((*iterDigit)->DigitInfo().isInDeadTime()||	
                 (*iter)->phChInfo().isInDeadTime()){
                (*iterDigit)->DigitInfo().setDeadtimeDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadForChamberInefficiency()
                 ||	(*iter)->phChInfo().isDeadForChamberInefficiency()){
                (*iterDigit)->DigitInfo().setChamberInefficiencyDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadByTimeJitter() ||	
                 (*iter)->phChInfo().isDeadByTimeJitter() ){
                (*iterDigit)->DigitInfo().setTimeJitteredDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadByTimeAdjustment() 
                 ||	(*iter)->phChInfo().isDeadByTimeAdjustment() ){
                (*iterDigit)->DigitInfo().setTimeAdjDigit(1) ;
              }					
              if((*iterDigit)->DigitInfo().isAliveByTimeAdjustment() 
                 ||	(*iter)->phChInfo().isAliveByTimeAdjustment() ){
                (*iterDigit)->DigitInfo().setAliveTimeAdjDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadByGeometry() 
                 ||	(*iter)->phChInfo().isDeadByGeometry() ){
                (*iterDigit)->DigitInfo().setGeometryInefficiency(1);
              }		
            }
            // add links to the hits 										
            for(iterOnHits=(*iter)->hitsTraceBack().begin();
                     iterOnHits<(*iter)->hitsTraceBack().end();iterOnHits++){
              (*iterDigit)->HitsHistory().push_back((*iterOnHits).
                                                    getMCMuonHistory());
                   (*iterDigit)->addToMCHits((*iterOnHits).getMCHit());
            }                 
          }
        }
        if(!found){
          if( m_verboseDebug)info()<<
                               " create new Digit with tile "<<
                               phChTileID[iTile]<<" "<<iTile<<endreq;
          std::vector<MuonHitTraceBack>::iterator iterOnHits;
          LHCb::MCMuonDigit* newMCDigit=new 
            LHCb::MCMuonDigit(phChTileID[iTile]);
          for(iterOnHits=(*iter)->hitsTraceBack().begin();
              iterOnHits<(*iter)->hitsTraceBack().end();iterOnHits++){
              (newMCDigit)->HitsHistory().push_back((*iterOnHits).
                                                    getMCMuonHistory());
            
            newMCDigit->addToMCHits((*iterOnHits).getMCHit());
          }
          newMCDigit->setDigitInfo((*iter)->phChInfo());
          newMCDigit->setFiringTime((*iter)->firingTime());
          
          mcDigitContainer.insert(newMCDigit); 
          ++countDigits;
        }
      }
    }
  }
  debug()<<" MC Digits created "<<countDigits<<endreq; 
  return StatusCode::SUCCESS;
}

StatusCode MuonDigitization::
createLogicalChannel(MuonDigitizationData<MuonCardiacChannelOutput>&
                     PhyChaOutput, LHCb::MCMuonDigits& mcDigitContainer){
	MsgStream log(msgSvc(), name()); 
  //  int flag=0;
  int countDigits=0;
  for(int i=0; i<m_partition; i++){
    MuonCardiacChannelOutputs::const_iterator iter =
      PhyChaOutput.getPartition(i)->begin();
    LHCb::MCMuonDigits::iterator iterDigit;        
    for(iter=PhyChaOutput.getPartition(i)->begin();iter<
          PhyChaOutput.getPartition(i)->end();iter++){	
     //  if(m_verboseDebug){
//         debug()<<"FE ID "<<(*iter)->phChID()->getID()<<endreq;}
//if((*iter)->
//info()<<" "<<(*iter)->
      LHCb::MuonTileID phChTileID[2];
      int numberOfTileID;  
      if(m_verboseDebug) info()<<"FE ID "<<
        (*iter)->chID().getID()<<endreq;
      if(m_verboseDebug)info()<<"FE ID "<<
                          (*iter)->chID().getID()<<endreq; 
       if(m_verboseDebug)info()<<" prima "<< (**iter).chID()<<endreq;
      (*iter)->calculateTileID(numberOfTileID,phChTileID,m_muonDetector);
       if(m_verboseDebug)info()<< (**iter).chID()<<endreq;
      
      if( m_verboseDebug)info()<<" after tile calculation " 
                               << numberOfTileID<<" "<<endreq;
      if( m_verboseDebug)info()<<" tile  " << 
                                phChTileID[0]<< phChTileID[1]<<" "<<endreq;
      
      //
      // loop over possible phchtileID (1 or 2 if double mapping)
      //
    
    
      for(int iTile=0;iTile<numberOfTileID;iTile++){
        bool found=false; 
        //
        // loop over already created Digits
        //         

                if(m_verboseDebug){info()<<" deg "<<  phChTileID[iTile].layout()<<" "<<
          phChTileID[iTile].station()<<" "<< phChTileID[iTile].region()<<" "<<
          phChTileID[iTile].quarter()<<" "<< phChTileID[iTile].nX()<<" "<<
                                     phChTileID[iTile].nY()<<endreq;
                }
                
        if(m_verboseDebug){
          debug()<<" Loop on mappings "<<
            iTile<<" "<<numberOfTileID<<endreq;}      
        if(m_verboseDebug) debug()<<" Loop on mappings "<<
            iTile<<" "<<numberOfTileID<<endreq;       
        for(iterDigit=mcDigitContainer.begin(); 
            iterDigit<mcDigitContainer.end()&&!found; iterDigit++){
          LHCb::MuonTileID tile=(*iterDigit)->key();
          //
          // tile is the key of the existing Digit, phChTileID[] 
          // is the just created ID of the ph.ch.
          //                
               
          if(tile==phChTileID[iTile]){
            if( m_verboseDebug)info()<<
                                 " Loop on mappings found already "
                                     <<tile<<" "<<endreq;
                      if( m_verboseDebug) 
            info()<<"  "<<
              (*iterDigit)->DigitInfo().isAlive()
                  <<" "<<(*iter)->chInfo().isAlive()
                  <<endreq;
            found=true;
            // Digit already exists, update bits and links
            std::vector<MuonHitTraceBack>::iterator iterOnHits;
            std::vector<MuonHitTraceBack*>::iterator iterOnHitsCardiac;
            std::vector<LHCb::MCMuonHitHistory>::iterator iterOnHitsInDigit;
            if((*iterDigit)->DigitInfo().isAlive()&&
               (*iter)->chInfo().isAlive()){
              //both fired
              if((*iterDigit)->firingTime()>(*iter)->firingTime()){
                (*iter)->setNotFiringDigit();
                
              }else{
                (*iterDigit)->setFiringTime((*iter)->firingTime());
                (*iterDigit)->DigitInfo().setNatureHit((*iter)
                                                       ->chInfo().
                                                       natureOfHit());
                (*iterDigit)->DigitInfo().setBXIDFlagHit((*iter)
                                                              ->chInfo().
                                                         BX());
                for(iterOnHitsInDigit=(*iterDigit)->HitsHistory().begin();
                    iterOnHitsInDigit<(*iterDigit)->HitsHistory().end();
                    iterOnHitsInDigit++){
                       (*iterOnHitsInDigit).setFiredFlag(0);						 
                }
              }
            }
            
            if((*iterDigit)->DigitInfo().isAlive()&&!((*iter)
                                                           ->chInfo().
                                                      isAlive())){
              // only one is fired	
              
            }
            if(!((*iterDigit)->DigitInfo().isAlive())&&
               ((*iter)->chInfo().isAlive())){
              // only one is fired	
             //  info()<<" importante "<<
//                 (*iterDigit)->DigitInfo().isAlive()  
//                     <<endreq;
              (*iterDigit)->setFiringTime((*iter)->firingTime());
              (*iterDigit)->DigitInfo().setNatureHit((*iter)
                                                     ->chInfo().
                                                     natureOfHit());
              (*iterDigit)->DigitInfo().setBXIDFlagHit((*iter)
                                                       ->chInfo().
                                                       BX());  
              (*iterDigit)->DigitInfo().setSecondPart(0);   
              (*iterDigit)->DigitInfo().setAliveDigit(1);
                   
              if( m_verboseDebug)info()<<" importante "<<
                                   (*iterDigit)->DigitInfo().isAlive()  
                                    <<endreq;
            }
            if(!((*iterDigit)->DigitInfo().isAlive())&&
               !((*iter)->chInfo().isAlive())){
              if( m_verboseDebug)info()<<
                                   " molto importante "<<
                                   (*iterDigit)->DigitInfo().isAlive()  
                                    <<endreq;                   
              // both not fired
              if((*iterDigit)->DigitInfo().isInDeadTime()||	
                 (*iter)->chInfo().isInDeadTime()){
                (*iterDigit)->DigitInfo().setDeadtimeDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadForChamberInefficiency()
                 ||	(*iter)->chInfo().isDeadForChamberInefficiency()){
                (*iterDigit)->DigitInfo().setChamberInefficiencyDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadByTimeJitter() ||	
                 (*iter)->chInfo().isDeadByTimeJitter() ){
                (*iterDigit)->DigitInfo().setTimeJitteredDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadByTimeAdjustment() 
                 ||	(*iter)->chInfo().isDeadByTimeAdjustment() ){
                (*iterDigit)->DigitInfo().setTimeAdjDigit(1) ;
              }					
              if((*iterDigit)->DigitInfo().isAliveByTimeAdjustment() 
                 ||	(*iter)->chInfo().isAliveByTimeAdjustment() ){
                (*iterDigit)->DigitInfo().setAliveTimeAdjDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadByGeometry() 
                 ||	(*iter)->chInfo().isDeadByGeometry() ){
                (*iterDigit)->DigitInfo().setGeometryInefficiency(1);
              }		
            }
            // add links to the hits 		
            
            std::vector<MuonHitTraceBack*>::iterator iterstart
              =(*iter)->hitsTraceBack().begin();		
            std::vector<MuonHitTraceBack*>::iterator iterstop=
              (*iter)->hitsTraceBack().end();									
            for(iterOnHitsCardiac=iterstart;
                     iterOnHitsCardiac!=iterstop;
                iterOnHitsCardiac++){
              // info()<<" add hits "<<(*iter)->hitsTraceBack().size()<<endreq;
              // info()<<" add hits "<< iterOnHitsCardiac<<endreq;
              
              //if((*iter)->hitsTraceBack().end()==iterOnHitsCardiac)
                //  info()<<" add hits at the end"<<endreq;
              
              (*iterDigit)->HitsHistory().push_back((*iterOnHitsCardiac)->
                                                    getMCMuonHistory());
                   (*iterDigit)->addToMCHits((*iterOnHitsCardiac)->getMCHit());
            }                 
          }
        }
        
        if(!found){
          //          if( m_verboseDebug)
        //   info()<<
//             " create new Digit with tile "<<
//             phChTileID[iTile]<<" "<<iTile<<endreq;
          std::vector<MuonHitTraceBack*>::iterator iterOnHitsCardiac;
          LHCb::MCMuonDigit* newMCDigit=new 
            LHCb::MCMuonDigit(phChTileID[iTile]);
          std::vector<MuonHitTraceBack*> pippo;
          pippo=(*iter)->hitsTraceBack();
          
          for(iterOnHitsCardiac=pippo.begin();
              iterOnHitsCardiac!=pippo.end();
              iterOnHitsCardiac++){ 
            // info()<<"a new  hit to add "<<(*iter)->hitsTraceBack().size()<<endreq;
            (newMCDigit)->HitsHistory().push_back((*iterOnHitsCardiac)->
                                                  getMCMuonHistory());
            // info()<<"a new  hit to add "<<endreq;
            
            newMCDigit->addToMCHits((*iterOnHitsCardiac)->getMCHit());
            // info()<<" hit added "<<endreq;
            //          (*iter)->chInfo()  
          }
          
          newMCDigit->setDigitInfo((*iter)->chInfo());
          newMCDigit->setFiringTime((*iter)->firingTime());
          //info()<<(*iter)->firingTime()<<" qui "
          //     <<(*iter)->chInfo().isAlive()<<endreq;
          mcDigitContainer.insert(newMCDigit); 
          ++countDigits;
        }
        
      }
                
    }
  }
  debug()<<" MC Digits created "<<countDigits<<endreq; 
  return StatusCode::SUCCESS;
}
  


StatusCode MuonDigitization::
createRAWFormat(LHCb::MCMuonDigits& mcDigitContainer, 
                LHCb::MuonDigits& digitContainer){
	MsgStream log(msgSvc(), name()); 
 	LHCb::MCMuonDigits::iterator iterMCDigit; 
 	for( iterMCDigit = mcDigitContainer.begin();iterMCDigit < 
         mcDigitContainer.end() ; iterMCDigit ++){



    LHCb::MCMuonDigitInfo digiinfo=(*iterMCDigit)->DigitInfo();
    
	  if(digiinfo.isAlive()){		  
      LHCb::MuonDigit* muonDigit= new LHCb::MuonDigit((*iterMCDigit)->key());
			unsigned int time=(unsigned	int)(((*iterMCDigit)->firingTime())
                                       /(m_timeBin));
      //			if(time>7)time=7;
			muonDigit->setTimeStamp(time);
			digitContainer.insert(muonDigit);
      LHCb::MuonTileID gg=(*iterMCDigit)->key();
			debug()<<"new daq word "<<
        gg.layout()<<" "<<gg.station()<<" "<<gg.region()<< 
        " "<<gg.quarter()<<" "<<gg.nX()<<" "<<gg.nY()<<" "<<" "<<time<<endreq;
      debug()<<gg<<endreq;
      
      //        " "<<time<<endreq;
 		}
	}
  return StatusCode::SUCCESS;	 
}

StatusCode MuonDigitization::
addElectronicNoise(MuonDigitizationData
                   <MuonPhysicalChannel>& PhysicalChannel){
  
	MsgStream log(msgSvc(), name()); 
	MuonPhysicalChannel* pFound;
  for(int ispill=1;ispill<=m_numberOfEvents;ispill++){
    int chamberTillNow=0;
    double shiftOfTOF=-m_BXTime*(ispill-1);
    for(int i=0;i<m_stationNumber;i++){
      for(int k=0;k<m_regionNumber;k++){
        int partitionNumber=i*m_regionNumber+k;
        int chamberInRegion=m_muonDetector->chamberInRegion(i,k);
        //					 int gapFE=m_pGetInfo->getGapPerFE(partitionNumber);
        for(int chamber=0;chamber<chamberInRegion;chamber++){		
          for(int frontEnd=0;frontEnd<(int)(m_muonDetector->gapsInRegion(i,k)/
                                            m_muonDetector->gapsPerFE(i,k));
              frontEnd++){
            for(int readout=0;readout<m_muonDetector->readoutInRegion(i,k);
                readout++){
              int phChInX=m_muonDetector->getPhChannelNX( readout,i,k);   
              int phChInY=m_muonDetector->getPhChannelNY( readout,i,k);
              //              info()<<"pre noiseChannels"<<partitionNumber<<" "<<readout<<endreq; 
              int noiseChannels=(detectorResponse.getResponse
                (partitionNumber,readout))->
                electronicNoise();
              //info()<<"noiseChannels "<<noiseChannels<<" "<<partitionNumber<<endreq;
              unsigned int readoutType=
                m_muonDetector->getReadoutType(readout,i,k);
              for(int hitNoise=0;hitNoise<noiseChannels;hitNoise++){
                int chX=(int)(m_flatDist()*phChInX);
                int chY=(int)(m_flatDist()*phChInY);
                if(chX==phChInX)chX=phChInX-1;
                if(chY==phChInY)chY=phChInY-1;
               double time=m_flatDist()*m_BXTime+shiftOfTOF ;
                MuonPhChID ID;
                ID.setStation(i);
                ID.setRegion(k);
                //                ID.setQuadrant(m_pGetInfo->getQuadrantChamber
                //               (chamber+chamberTillNow));
                ID.setChamber(chamber);
                ID.setPhChIDX(chX);
                ID.setPhChIDY(chY);
                ID.setFrontEnd(frontEnd);
                ID.setReadout(readoutType);					 
                MuonPhysicalChannel* newPhysicalChannel=new 
                  MuonPhysicalChannel(ID.getFETile(),m_gate,m_BXTime);
                MuonHitTraceBack* pointerToHit=new MuonHitTraceBack;
                pointerToHit->setHitArrivalTime(time);							
                pointerToHit->getMCMuonHitOrigin().
                  setHitNature(LHCb::MuonOriginFlag::ELECTRONICNOISE);
                pointerToHit->getMCMuonHitOrigin().setBX(ispill-1);	
                pointerToHit->getMCMuonHistory().setBXOfHit(ispill-1);
                pointerToHit->getMCMuonHistory().
                  setNatureOfHit(LHCb::MuonOriginFlag::ELECTRONICNOISE);
                newPhysicalChannel->hitsTraceBack().push_back(*pointerToHit);
                delete pointerToHit;				
                pFound=0;						
                bool already=PhysicalChannel.
                  isObjectIn(partitionNumber,newPhysicalChannel,
                             pFound, ComparePC);
                if(already){											   
                  pFound->addToPC(newPhysicalChannel);
                  delete newPhysicalChannel;
                }
                else{
                  newPhysicalChannel->setResponse
                    (detectorResponse.getResponse
                     (*(newPhysicalChannel->phChID())));
                  StatusCode asc=PhysicalChannel.addMuonObject(partitionNumber,
                                                newPhysicalChannel);
                  if(asc.isFailure())debug()<<" unable to add hit "<<endreq;
                  
                }
              }	 
            }	 
          }
        }
        chamberTillNow=chamberTillNow+chamberInRegion;	
      }									 							 		
    }			 
	}		
  //info()<<"esco "<<endreq;
  return StatusCode::SUCCESS;	 
  
}
