// $Id: MuonPhysicalChannelOutput.cpp,v 1.14 2007-03-12 10:32:53 cattanem Exp $
#include <iostream>

#include "MuonDet/DeMuonDetector.h"

#include "MuonPhysicalChannelOutput.h"

// constructor from MuonPhysicalChannel
MuonPhysicalChannelOutput::MuonPhysicalChannelOutput
(MuonPhysicalChannel& orig){
  std::vector<MuonHitTraceBack>::iterator iterOrigin;
  
  for(iterOrigin=orig.hitsTraceBack().begin();
      iterOrigin<orig.hitsTraceBack().end();iterOrigin++){
	  m_Hits.push_back((*iterOrigin));
	}
  //  m_Hits = orig.hitsTraceBack();
  setPhChID(*(orig.phChID()));
  m_FiringTime = 0;
}


void MuonPhysicalChannelOutput::calculateTileID( int& numberTileOutput, 
                                                 LHCb::MuonTileID phChTileID[2],DeMuonDetector* muonDetector){  
  
  bool debug=false;
  
  unsigned int station=phChID()->getStation();
  unsigned int region=phChID()->getRegion();
  unsigned int chamber=phChID()->getChamber();
  unsigned int readout=phChID()->getReadout();
  unsigned int quadrant=phChID()->getQuadrant();
  unsigned int idX=phChID()->getPhChIDX();
  unsigned int idY=phChID()->getPhChIDY();
  unsigned int newidX,newidY;
  unsigned int idXGlobal,idYGlobal;
  unsigned int idLogX,idLogY;
  int numberOfTileCreated=0;
  unsigned int numberOfPCX, numberOfPCY;

  if(debug){
    std::cout<<station<<" "<<region<<" "<<chamber<<" "<<quadrant<<" "<<
      idX<<" "<<idY<<std::endl;
  } 

  newidX=0;
  newidY=0;
  numberOfPCX=0;
  numberOfPCY=0;
  numberTileOutput=0;
  // loop over FE channel readout
  //
  if(debug)std::cout<<muonDetector->readoutInRegion(station,region)<<std::endl;

  for (int readoutNumber=0;readoutNumber<(int)muonDetector->
         getLogMapInRegion(station,region);readoutNumber++){
    //
    // check if current readout coincides with one of the LogMap readouts
    //
    if(debug) std::cout<<"logmap type, readout "<<
                muonDetector->getLogMapRType(readoutNumber,station,region)<<" "<<
                readout<<" "<<readoutNumber<<
                std::endl;
    
    if(muonDetector->getLogMapRType(readoutNumber,station,region)==readout)
    {
      // define order of FE channel according to the MuonTileID 
      //conventions: from 0,0 left,bottom to radial coordinates
      //
      
      for(int countReadout=0; countReadout<muonDetector->
            readoutInRegion(station,region);countReadout++)
      {
        if(  muonDetector->getLogMapRType(readoutNumber,station,region)==
             muonDetector->getReadoutType(countReadout,station,region)){
          numberOfPCX=muonDetector->
            getPhChannelNX(countReadout,station,region);
          numberOfPCY=muonDetector->
            getPhChannelNY(countReadout,station,region);
          if(debug) std::cout<<"channels "<<countReadout<<" "<<station<<" "<<region<<" "
                             <<numberOfPCX<<" "<<
                      numberOfPCY<<std::endl;
          
          
        }
      }
      
      // 
      // FE ch address relative to the chamber transformed 
      //in quadrant reference system
      //
      if(quadrant==0){
        newidX=idX; 
        newidY=idY;
      }else if(quadrant==3){
        newidX=numberOfPCX-idX-1; 
        newidY=idY;                          
      }else if(quadrant==2){
        newidX=numberOfPCX-idX-1; 
        newidY=numberOfPCY-idY-1;
      }else if(quadrant==1){
        newidX=idX;
        newidY=numberOfPCY-idY-1;
      }
      // 
      // FE ch address in the whole quadrant
      //
      LHCb::MuonTileID chaTile;
      muonDetector-> Chamber2Tile(chamber, station,region,chaTile)   ;
      
      idXGlobal=newidX+chaTile.nX()*numberOfPCX;
      idYGlobal=newidY+chaTile.nY()*numberOfPCY;
      if(debug)std::cout<<"cha tile "<<chaTile.nX()<<" "<<chaTile.nY()<<std::endl;
      
      if(debug)std::cout<< idXGlobal<<" "<< idYGlobal<<std::endl;
      
      //
      //  compute Logical Channel address now
      //
      idLogX=idXGlobal/muonDetector->
        getLogMapMergex(readoutNumber,station,region);    
      idLogY=idYGlobal/muonDetector->
        getLogMapMergey(readoutNumber,station,region);   
      //
      // create the tile of the phys chan
      //  
      if(debug)std::cout<<  idLogX <<" "<< idLogY<<std::endl;
      if(debug)std::cout<< muonDetector->
                 getLogMapMergex(readoutNumber,station,region)<<" "<<
                 muonDetector->
                 getLogMapMergey(readoutNumber,station,region)<<std::endl;
      
      
      ++numberTileOutput;
      
      MuonLayout layout(muonDetector->getLayoutX(readoutNumber,station,region),
                        muonDetector->
                        getLayoutY(readoutNumber,station,region));   
      if(debug)std::cout<<"Layout "<<muonDetector->getLayoutX(readoutNumber,station,region)<<" "
                        <<muonDetector->getLayoutY(readoutNumber,station,region)<<std::endl;
      
      phChTileID[numberOfTileCreated].setLayout(layout);
      phChTileID[numberOfTileCreated].setStation(station);
      //        phChTileID[numberOfTileCreated].setReadout(usefullPointer->
      //                                                   getLogMapRType
      //                                                   (readoutNumber,part));
      phChTileID[numberOfTileCreated].setRegion(region);
      phChTileID[numberOfTileCreated].setQuarter(quadrant);
      phChTileID[numberOfTileCreated].setX(idLogX);
      phChTileID[numberOfTileCreated].setY(idLogY);        
      ++numberOfTileCreated;
    }else{
    }
  }
};


void MuonPhysicalChannelOutput::
calculateCardiacORID(MuonCardiacChID & cardiacID,                     
                     DeMuonDetector* muonDetector)
{
  unsigned int station=phChID()->getStation();
  unsigned int region=phChID()->getRegion();
  unsigned int chamber=phChID()->getChamber();
  unsigned int readout=phChID()->getReadout();
  unsigned int quadrant=phChID()->getQuadrant();
  unsigned int idX=phChID()->getPhChIDX();
  unsigned int idY=phChID()->getPhChIDY();
/*
std::cout<<muonDetector->getCardiacORX(station*4+region,readout)<<" dft "<<
	muonDetector->getCardiacORY(station*4+region,readout)<<std::endl;
std::cout<<" sta reg "<<readout<<" "<<station<<" "<<region<<std::endl;;
*/
  MuonCardiacChID cardiacOutput;
  cardiacOutput.setStation(station);
  cardiacOutput.setRegion(region);
  cardiacOutput.setChamber(chamber);
  cardiacOutput.setReadout(readout);
  cardiacOutput.setQuadrant(quadrant);
  unsigned int x=idX/muonDetector->getCardiacORX(station*4+region,readout);
  unsigned int y=idY/muonDetector->getCardiacORY(station*4+region,readout);
  cardiacOutput.setChIDX(x);
  cardiacOutput.setChIDY(y);
  cardiacID=cardiacOutput;
}

void MuonPhysicalChannelOutput::fillTimeList()
{
  std::vector<MuonHitTraceBack>::iterator iterInHits ;
  for(iterInHits=hitsTraceBack().begin();
      iterInHits<hitsTraceBack().end();iterInHits++){ 
    if((*iterInHits).getMCMuonHistory().isHitFiring()){
      // add time to the list
      m_timeList.push_back((float)((*iterInHits).hitArrivalTime()));
    }
  }
}
