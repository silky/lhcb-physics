/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
#define GIGA_GIGAVISMANAGER_CPP 1 
/// ===========================================================================
#ifdef G4VIS_USE
/// GiGa 
#include "GiGa/GiGaVisManager.h"
/// DAWN 
#ifdef G4VIS_USE_DAWN
#include "G4FukuiRenderer.hh"
#endif
/// DAWNFILE 
#ifdef G4VIS_USE_DAWNFILE
#include "G4DAWNFILE.hh"
#endif
/// OPACS 
#ifdef G4VIS_USE_OPACS
#include "G4Wo.hh"
#include "G4Xo.hh"
#endif
/// OPENGL 
#ifdef G4VIS_USE_OPENGLX
#include "G4OpenGLImmediateX.hh"
#include "G4OpenGLStoredX.hh"
#endif
/// OPENGLWIN32 
#ifdef G4VIS_USE_OPENGLWIN32
#include "G4OpenGLImmediateWin32.hh"
#include "G4OpenGLStoredWin32.hh"
#endif
/// OPENGLXM 
#ifdef G4VIS_USE_OPENGLXM
#include "G4OpenGLImmediateXm.hh"
#include "G4OpenGLStoredXm.hh"
#endif
/// OIX 
///#ifdef G4VIS_USE_OIX
///#include "G4OpenInventorX.hh"
///#endif
/// OIWIN32 
#ifdef G4VIS_USE_OIWIN32
#include "G4OpenInventorWin32.hh"
#endif
/// VRML 
#ifdef G4VIS_USE_VRML
#include "G4VRML1.hh"
#include "G4VRML2.hh"
#endif
/// VRMLFILE 
#ifdef G4VIS_USE_VRMLFILE
#include "G4VRML1File.hh"
#include "G4VRML2File.hh"
#endif
/// RAYTRACER 
#ifdef G4VIS_USE_RAYTRACER
#include "G4RayTracer.hh"
#endif

/// constructor
GiGaVisManager::GiGaVisManager () {}

void GiGaVisManager::RegisterGraphicsSystems () 
{
  //
#ifdef G4VIS_USE_DAWN
  RegisterGraphicsSystem (new G4FukuiRenderer);
#endif
  ///
#ifdef G4VIS_USE_DAWNFILE
  RegisterGraphicsSystem (new G4DAWNFILE);
#endif
  ///
#ifdef G4VIS_USE_OPACS
  RegisterGraphicsSystem (new G4Wo);
  RegisterGraphicsSystem (new G4Xo);
#endif
  ///
#ifdef G4VIS_USE_OPENGLX
  RegisterGraphicsSystem (new G4OpenGLImmediateX);
  RegisterGraphicsSystem (new G4OpenGLStoredX);
#endif
  ///
#ifdef G4VIS_USE_OPENGLWIN32
  RegisterGraphicsSystem (new G4OpenGLImmediateWin32);
  RegisterGraphicsSystem (new G4OpenGLStoredWin32);
#endif
  ///
#ifdef G4VIS_USE_OPENGLXM
  RegisterGraphicsSystem (new G4OpenGLImmediateXm);
  RegisterGraphicsSystem (new G4OpenGLStoredXm);
#endif
  ///
  ///#ifdef G4VIS_USE_OIX
  ///RegisterGraphicsSystem (new G4OpenInventorX);
  ///#endif
  ///
#ifdef G4VIS_USE_OIWIN32
  RegisterGraphicsSystem (new G4OpenInventorWin32);
#endif
  ///
#ifdef G4VIS_USE_VRML
  RegisterGraphicsSystem (new G4VRML1);
  RegisterGraphicsSystem (new G4VRML2);
#endif
  ///
#ifdef G4VIS_USE_VRMLFILE
  RegisterGraphicsSystem (new G4VRML1File);
  RegisterGraphicsSystem (new G4VRML2File);
#endif
  ///
#ifdef G4VIS_USE_RAYTRACER
  RegisterGraphicsSystem (new G4RayTracer);
#endif
  ///
  if (fVerbose > 0) 
    {
      G4cout <<
        "\nYou have successfully chosen to use the following graphics systems."
             << G4endl;
      PrintAvailableGraphicsSystems ();
    }
  ///
}
///
#endif
