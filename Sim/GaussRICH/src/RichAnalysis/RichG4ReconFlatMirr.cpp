// $Id: RichG4ReconFlatMirr.cpp,v 1.8 2006-11-01 09:41:55 seaso Exp $
// Include files


#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidBoolean.h"

//#include <CLHEP/Geometry/Point3D.h>
//#include <CLHEP/Geometry/Vector3D.h>
//#include <CLHEP/Geometry/Transform3D.h>
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Transform3DTypes.h"
#include "Kernel/SystemOfUnits.h"
#include "Kernel/Plane3DTypes.h"
                                                                                                                    

// local
#include "RichG4SvcLocator.h"
#include "RichG4GaussPathNames.h"

#include "RichG4ReconFlatMirr.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4ReconFlatMirr
//
// 2003-09-11 : Sajan EASO
//-----------------------------------------------------------------------------
//

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4ReconFlatMirr::RichG4ReconFlatMirr(  )
  :    m_RichNumFlatMirrorOrientationParam(4),
       m_RichNumFlatMirrorLocationTypes(4),
       m_RichFlatMirrorNominalOrientation(m_RichNumFlatMirrorLocationTypes,
                                          std::vector<double>(m_RichNumFlatMirrorOrientationParam)),
       m_RichNumSecMirrorCoCParam(4),
       m_Rich1NumSecMirror(16),
       m_Rich2NumSecMirror(40),
       m_RichSecMirrCoCRad( m_Rich1NumSecMirror+m_Rich2NumSecMirror,
                             std::vector<double> (m_RichNumSecMirrorCoCParam)),
       m_Rich1Mirror2CommonPath(Rich1DeStructurePathName+  "/Rich1Mirror2Q"),
       m_Rich2Mirror2CommonPath(Rich2DeStructurePathName + "/Rich2SecMirror:"),
       m_Rich1Mirror2IndPathString(std::vector<std::string> (m_Rich1NumSecMirror)),
       m_Rich2Mirror2IndPathString(std::vector<std::string> (m_Rich2NumSecMirror))
{

  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");
  //  RichG4ReconFlatMirrlog << MSG::INFO <<
  //  "Now creating RichG4ReconFlatMirr "<<endreq;
  // Now for the orientation of the flat mirror.
  // for the array   m_RichFlatMirrorNominalOrientation
  // the first element 0->3 correspond to the flat mirrors
  // at Rich1Top, Rich1Bottom, Rich2Left, Rich2Right respectively.
  // The second element 0->3 is the three direction cosines
  // of the plane of a flatmirror and the distance of the
  // flat mirror plane from the origin so that one can
  // write the equation of the plane as ax+by+cz+d=0.
  // for the m_RichSecMirrCoCRad the first parameter is the mirror number
  // in Rich1+Rich2; The second parameter is the xyz of Coc and radius. 
  // m_RichNumSecMirrorCoCParam = 4 for the xyz+rad for each segment.
  // the mirror number 0-15 for the rich1 and 16-45 for rich2 in this array.  
  

  SmartDataPtr<DetectorElement> Rich1DE(detSvc, Rich1DeStructurePathName);
  SmartDataPtr<DetectorElement> Rich2DE(detSvc, Rich2DeStructurePathName);
  std::string r2mNum [] ={"00:0","01:1","02:2","03:3","34:4","35:5",
                          "36:6","37:7","18:8","19:9","110:10","111:11",
                          "212:12","213:13","214:14","215:15"};
  for(int ii=0; ii<m_Rich1NumSecMirror; ++ii) {
    m_Rich1Mirror2IndPathString[ii]= m_Rich1Mirror2CommonPath+ r2mNum[ii];
    
  }


    std::string mnum []= {"0","1","2","3","4","5","6","7","8","9","10",
                         "11","12","13","14","15","16","17","18","19","20",
                          "21","22","23","24","25","26","27","28","29","30",
                          "31","32","33","34","35","36","37","38","39","40"};
    
    for(int ij=0; ij<m_Rich2NumSecMirror;++ij) {
      m_Rich2Mirror2IndPathString[ij]= m_Rich2Mirror2CommonPath+mnum[ij];
      
    }
    
 
  if(Rich1DE) {
    setRich1FlatMirrorParam();

  }
  if(Rich2DE){

    setRich2FlatMirrorParam();

  }


}
RichG4ReconFlatMirr::~RichG4ReconFlatMirr(  ) {
}



void RichG4ReconFlatMirr::setRich1FlatMirrorParam( )
{

  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");

  // modif in the following to be compatible with recent Detdesc. 
  // SE 16-6-2005.

  SmartDataPtr<DetectorElement> Rich1DE(detSvc, Rich1DeStructurePathName );
  // modif in the following to be compatible with the xml userparameter name.
  // SE 10-5-2005.
  if( Rich1DE) {

    //    std::vector<double> r1m2Nor = Rich1DE->param<std::vector<double> >("Rich1NominalFlatMirrorPlane");
    std::vector<double> r1m2Nor = Rich1DE->param<std::vector<double> >("NominalSecMirrorPlane");
    double r1m2A = r1m2Nor[0];
    double r1m2B = r1m2Nor[1];
    double r1m2C = r1m2Nor[2];
    double r1m2D = r1m2Nor[3];

    m_RichFlatMirrorNominalOrientation[0][0]= r1m2A;
    m_RichFlatMirrorNominalOrientation[0][1]= r1m2B;
    m_RichFlatMirrorNominalOrientation[0][2]= r1m2C;
    m_RichFlatMirrorNominalOrientation[0][3]= r1m2D;

    m_RichFlatMirrorNominalOrientation[1][0]= r1m2A;
    m_RichFlatMirrorNominalOrientation[1][1]= -1.0*r1m2B;
    m_RichFlatMirrorNominalOrientation[1][2]= r1m2C;
    m_RichFlatMirrorNominalOrientation[1][3]= r1m2D;

    

    //       RichG4ReconFlatMirrlog << MSG::INFO
    //           << "Flat Mirr param in rich1  " 
    //                         << r1m2A<<"  "<<r1m2B
    //                         <<"   "<<r1m2C
    //                         <<"   "<<r1m2D<< endreq;
    //
    //    double r0c0X =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbXR0C0");
    // double r0c0Y =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbYR0C0");
    // double r0c0Z =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbZR0C0");
    
    // double r2c2X =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbXR2C2");
    //  double r2c2Y =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbYR2C2");
    //  double r2c2Z =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbZR2C2");

    //    double r2delX =   Rich1DE->param<double>("Rich1Mirror2CoCNominalDeltaX");
    // double r2delY =   Rich1DE->param<double>("Rich1Mirror2CoCNominalDeltaY");
    // double r2delZ =   Rich1DE->param<double>("Rich1Mirror2CoCNominalDeltaZ");
	  //    double r2rad=     Rich1DE->param<double>("Rich1Mirror2NominalRadiusC");
	  //   RichG4ReconFlatMirrlog << MSG::INFO<<"Rich1 Mirror2 nominal radius "<< r2rad<<endreq;
   
    
    for(int im=0; im< m_Rich1NumSecMirror; ++im) { 
      //      std::string apath = m_Rich1Mirror2IndPathString[ii];
      //     SmartDataPtr<DetectorElement> Rich1M2(detSvc, apath);
      DetectorElement* Rich1M2=getMirrorDetElem (0, im );
      
     if(!Rich1M2 ) {

       RichG4ReconFlatMirrlog << MSG::ERROR<<       
       "Rich1 mirror2 detelem does not exist . Mirror num "<<im << endreq;
       
     }else {
       
     
      Gaudi::XYZPoint zero(0.0,0.0,0.0);
      Gaudi::XYZPoint mcoc = Rich1M2 ->geometry() ->toGlobal(zero);
      const SolidSphere* aSphereSolid = getCurMirrorSolid (0,im );
          
      double r2rad = aSphereSolid->insideRadius();
      // RichG4ReconFlatMirrlog << MSG::INFO<<"Rich1 Mirror2 nominal radius "<< r2rad<<endreq;
      
      m_RichSecMirrCoCRad[im] [0] = mcoc.x();
      m_RichSecMirrCoCRad[im] [1] = mcoc.y();
      m_RichSecMirrCoCRad[im] [2] = mcoc.z();
      m_RichSecMirrCoCRad[im] [3] =  r2rad;    

      //  RichG4ReconFlatMirrlog << MSG::INFO<< "Rich1 Mirror2 num CoCxyz rad "<<
      //                        im<<"  "<< mcoc.x()<<"  "
      //			     << mcoc.y()<<"  "<< mcoc.z()<<"  "<<r2rad <<endreq;
     
      

      
      
     }
     
    }    
  }
}



void RichG4ReconFlatMirr::setRich2FlatMirrorParam( )
{
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");


  SmartDataPtr<DetectorElement> Rich2DE(detSvc, Rich2DeStructurePathName );

  if(Rich2DE) {

    std::vector<double> r2SecNormPlane =  Rich2DE->param<std::vector<double> >("NominalSecMirrorPlane");
    double r2m2A = r2SecNormPlane[0];
    double r2m2B = r2SecNormPlane[1];
    double r2m2C = r2SecNormPlane[2];
    double r2m2D = r2SecNormPlane[3];


    m_RichFlatMirrorNominalOrientation[2][0]= r2m2A;
    m_RichFlatMirrorNominalOrientation[2][1]= r2m2B;
    m_RichFlatMirrorNominalOrientation[2][2]= r2m2C;
    m_RichFlatMirrorNominalOrientation[2][3]= r2m2D;

    m_RichFlatMirrorNominalOrientation[3][0]= -1.0*r2m2A;
    m_RichFlatMirrorNominalOrientation[3][1]= r2m2B;
    m_RichFlatMirrorNominalOrientation[3][2]= r2m2C;
    m_RichFlatMirrorNominalOrientation[3][3]= r2m2D;

    // RichG4ReconFlatMirrlog << MSG::INFO
    //                       << "Flat Mirr nominal param in rich2  "
    //                       << r2m2A<<"  "<<r2m2B
    //                       <<"   "<<r2m2C
    //                       <<"   "<<r2m2D<< endreq;
    //
    
        
    for(int im=0; im< m_Rich2NumSecMirror; ++im) {
      DetectorElement* Rich2M2=getMirrorDetElem (1, im );

      //      std::string apath = aa+mnum[im];
      // SmartDataPtr<DetectorElement> Rich2M2(detSvc, apath);
    if(!Rich2M2) {
       RichG4ReconFlatMirrlog << MSG::ERROR<<       
      "Rich1 mirror2 detelem does not exist . Mirror num "<<im<< endreq;
       
    }else {
      
      Gaudi::XYZPoint zero(0.0,0.0,0.0);
      Gaudi::XYZPoint mcoc = Rich2M2 ->geometry() ->toGlobal(zero);

      const SolidSphere* aSphereSolid = getCurMirrorSolid (1,im );
      
     
      double r2rad = aSphereSolid->insideRadius();
             
      m_RichSecMirrCoCRad[im+m_Rich1NumSecMirror] [0] = mcoc.x();
      m_RichSecMirrCoCRad[im+m_Rich1NumSecMirror] [1] = mcoc.y();
      m_RichSecMirrCoCRad[im+m_Rich1NumSecMirror] [2] = mcoc.z();
      m_RichSecMirrCoCRad[im+m_Rich1NumSecMirror] [3] =  r2rad;    

      //  RichG4ReconFlatMirrlog << MSG::INFO<< "Rich2 Mirror2 num CoCxyz rad "<<
      //                        im<<"  "<< mcoc.x()<<"  "
      //                       << mcoc.y()<<"  "<< mcoc.z()<<"   "<<r2rad <<endreq; 
      //
    }    
      
    }    
  }
  

  
}

Gaudi::XYZPoint  RichG4ReconFlatMirr::FlatMirrorReflect(const Gaudi::XYZPoint & HitCoordQw ,
                                                   int FlatMirrorType)
{

  Gaudi::Plane3D aMPlane(   
                 m_RichFlatMirrorNominalOrientation[FlatMirrorType][0],
                 m_RichFlatMirrorNominalOrientation[FlatMirrorType][1],
                 m_RichFlatMirrorNominalOrientation[FlatMirrorType][2],
                 m_RichFlatMirrorNominalOrientation[FlatMirrorType][3]);
  double adist =  aMPlane.Distance(HitCoordQw);
  Gaudi::XYZPoint virtDetPoint =
         HitCoordQw - 2.0*adist * (aMPlane.Normal() );

  return virtDetPoint;
  
}
Gaudi::XYZPoint RichG4ReconFlatMirr::ConvertToLocal(const Gaudi::XYZPoint & aGlobalPoint, 
     int aRichDetNum, int aMirrorNum ) 
{
  
 DetectorElement* aMDet = getMirrorDetElem(aRichDetNum,aMirrorNum);
  
  Gaudi::XYZPoint aLocp = aMDet->geometry()->toLocal( aGlobalPoint);
  
  return aLocp;
  
}
Gaudi::XYZPoint RichG4ReconFlatMirr::ConvertToGlobal(const Gaudi::XYZPoint & aLocalPoint,
     int aRichDetNum, int aMirrorNum )   
{
 
  DetectorElement* aMDet = getMirrorDetElem(aRichDetNum,aMirrorNum);
  Gaudi::XYZPoint aGlop = aMDet->geometry()->toGlobal( aLocalPoint);
  return aGlop;
  
}

Gaudi::XYZPoint RichG4ReconFlatMirr::FlatMirrorIntersection( const Gaudi::XYZPoint & aGlobalPoint1,
                     const Gaudi::XYZPoint & aGlobalPoint2,int aRichDetNum,int aFlatMirrorNum) 
{


  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");
 
  const Gaudi::XYZPoint & aLocalPoint1 =  ConvertToLocal( aGlobalPoint1,aRichDetNum,aFlatMirrorNum);
  const Gaudi::XYZPoint & aLocalPoint2 =  ConvertToLocal( aGlobalPoint2,aRichDetNum,aFlatMirrorNum);
  
  const Gaudi::XYZVector aVect = (aLocalPoint2-aLocalPoint1).unit();
  ISolid::Ticks sphTicks;


  const SolidSphere* aSphereSolid= getCurMirrorSolid (aRichDetNum,aFlatMirrorNum);
  const unsigned int sphTicksSize = aSphereSolid->
          intersectionTicks(aLocalPoint1, aVect, sphTicks);     
  
  const Gaudi::XYZPoint & aLocalIntersectionPoint = aLocalPoint1+ sphTicks[0] * aVect;
  
  const Gaudi::XYZPoint & aGlobalIntersectionPoint = ConvertToGlobal(aLocalIntersectionPoint,
                                                                     aRichDetNum,aFlatMirrorNum );
  return aGlobalIntersectionPoint;
  
}

const SolidSphere* RichG4ReconFlatMirr::getCurMirrorSolid (int aRichDetNum, int aFlatMirrorNum ) 
{

  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");

   const SolidSphere* sphereSolid = 0;
  DetectorElement* aMDet = getMirrorDetElem(aRichDetNum,aFlatMirrorNum);
  if(!aMDet ) {
       RichG4ReconFlatMirrlog << MSG::ERROR<<       
         "Rich mirror detelem does not exist . richdet Mirror num "<< aRichDetNum
                              << aFlatMirrorNum << endreq;
      
  }else {
    
  
     const ISolid* aSolid = aMDet ->geometry() ->lvolume()->solid();
     const std::string type = aSolid->typeName();
     if( type == "SolidSphere" ){
       sphereSolid = dynamic_cast<const SolidSphere*>(aSolid);
     }else {
       // assume that the sphere segment boolean is always made with sphere as the
       // staring vol.
       const SolidBoolean* compSolid = dynamic_cast<const SolidBoolean*>(aSolid);
       sphereSolid =  dynamic_cast<const SolidSphere*>(compSolid->first());   
     }
  }
  
return  sphereSolid ;
     
}

DetectorElement* RichG4ReconFlatMirr::getMirrorDetElem (int aRichDetNum, int aMirrorNum ) 
{
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");


  std::string apath = "NULL";
  
  
  
  if( aRichDetNum == 0 ) {
    if( aMirrorNum < (int) m_Rich1Mirror2IndPathString.size() ) {
    
      apath = m_Rich1Mirror2IndPathString[aMirrorNum];
    }else {

      RichG4ReconFlatMirrlog << MSG::ERROR<<" Unknown sec mirror num in Rich1  "
       << aMirrorNum<<endreq;
      
    }
    
    
    
  } else if (aRichDetNum == 1 ) {

    if( aMirrorNum < (int) m_Rich2Mirror2IndPathString.size() ) {


      apath =  m_Rich2Mirror2IndPathString[aMirrorNum];

    }else {
     
      RichG4ReconFlatMirrlog << MSG::ERROR<<" Unknown sec mirror num in Rich2  "
       << aMirrorNum<<endreq;
    }
    
  } else {
    
      RichG4ReconFlatMirrlog << MSG::ERROR<<" Unknown Rich det num   "
       << aRichDetNum <<endreq;
    
  }
  
  
  
  
       SmartDataPtr<DetectorElement> RichM2(detSvc, apath);
  
       return RichM2;
       
  
}
Gaudi::XYZPoint RichG4ReconFlatMirr::FlatMirrorCoC( int  aRichDetNum, int aMirrorNum ) {

  Gaudi::XYZPoint aPoint(0,0,0);

  if( aRichDetNum == 0 ) {
    aPoint = Gaudi::XYZPoint (m_RichSecMirrCoCRad [aMirrorNum] [0],
                              m_RichSecMirrCoCRad [aMirrorNum] [1],
			      m_RichSecMirrCoCRad[aMirrorNum] [2]);

  }else if ( aRichDetNum == 1 ) {

    aPoint = Gaudi::XYZPoint (m_RichSecMirrCoCRad [aMirrorNum+m_Rich1NumSecMirror] [0],
                              m_RichSecMirrCoCRad [aMirrorNum+m_Rich1NumSecMirror] [1],
			      m_RichSecMirrCoCRad[aMirrorNum+m_Rich1NumSecMirror] [2]);

  }

  return aPoint;
}
//=============================================================================
