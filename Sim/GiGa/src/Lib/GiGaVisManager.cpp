// ============================================================================
// VS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/01/22 18:20:53  ibelyaev
//  Vanya: update for newer versions of Gaudi and Geant4
//
// Revision 1.5  2001/08/12 15:42:50  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.4  2001/07/23 13:12:13  ibelyaev
// the package restructurisation(II)
//
// Revision 1.3  2001/07/15 20:54:27  ibelyaev
// package restructurisation
//
// ============================================================================
#define GIGA_GIGAVISMANAGER_CPP 1 
// ============================================================================
#ifndef G4VIS_NONE
// GiGa 
#include "GiGaVisManager.h"
// Supported drivers...
// Not needing external packages or libraries...
#include "G4ASCIITree.hh"
#include "G4DAWNFILE.hh"
#include "G4GAGTree.hh"
#include "G4HepRepFile.hh"
#include "G4RayTracer.hh"
#include "G4VRML1File.hh"
#include "G4VRML2File.hh"

// Needing external packages or libraries...

#ifdef G4VIS_USE_DAWN
#include "G4FukuiRenderer.hh"
#endif

#ifdef G4VIS_USE_OPACS
#include "G4Wo.hh"
#include "G4Xo.hh"
#endif

#ifdef G4VIS_USE_OPENGLX
#include "G4OpenGLImmediateX.hh"
#include "G4OpenGLStoredX.hh"
#endif

#ifdef G4VIS_USE_OPENGLWIN32
#include "G4OpenGLImmediateWin32.hh"
#include "G4OpenGLStoredWin32.hh"
#endif

#ifdef G4VIS_USE_OPENGLXM
#include "G4OpenGLImmediateXm.hh"
#include "G4OpenGLStoredXm.hh"
#endif

#ifdef G4VIS_USE_OIX
#include "G4OpenInventorX.hh"
#endif

#ifdef G4VIS_USE_OIWIN32
#include "G4OpenInventorWin32.hh"
#endif

#ifdef G4VIS_USE_VRML
#include "G4VRML1.hh"
#include "G4VRML2.hh"
#endif

/** @file 
 *  Implementation fiel for class GiGaVisManager 
 *  @author Vanya Belyaev 
 *  @date xx/xxx/xxxxx
 */

/// constructor
GiGaVisManager::GiGaVisManager () {}

void GiGaVisManager::RegisterGraphicsSystems () 
{

  // Graphics Systems not needing external packages or libraries...
  RegisterGraphicsSystem (new G4ASCIITree);
  RegisterGraphicsSystem (new G4GAGTree);
  RegisterGraphicsSystem (new G4DAWNFILE);
  RegisterGraphicsSystem (new G4HepRepFile);
  RegisterGraphicsSystem (new G4RayTracer);
  RegisterGraphicsSystem (new G4VRML1File);
  RegisterGraphicsSystem (new G4VRML2File);

  // Graphics systems needing external packages or libraries...

#ifdef G4VIS_USE_DAWN
  RegisterGraphicsSystem (new G4FukuiRenderer);
#endif

#ifdef G4VIS_USE_OPACS
  RegisterGraphicsSystem (new G4Wo);
  RegisterGraphicsSystem (new G4Xo);
#endif

#ifdef G4VIS_USE_OPENGLX
  RegisterGraphicsSystem (new G4OpenGLImmediateX);
  RegisterGraphicsSystem (new G4OpenGLStoredX);
#endif

#ifdef G4VIS_USE_OPENGLWIN32
  RegisterGraphicsSystem (new G4OpenGLImmediateWin32);
  RegisterGraphicsSystem (new G4OpenGLStoredWin32);
#endif

#ifdef G4VIS_USE_OPENGLXM
  RegisterGraphicsSystem (new G4OpenGLImmediateXm);
  RegisterGraphicsSystem (new G4OpenGLStoredXm);
#endif

#ifdef G4VIS_USE_OIX
  RegisterGraphicsSystem (new G4OpenInventorX);
#endif

#ifdef G4VIS_USE_OIWIN32
  RegisterGraphicsSystem (new G4OpenInventorWin32);
#endif

#ifdef G4VIS_USE_VRML
  RegisterGraphicsSystem (new G4VRML1);
  RegisterGraphicsSystem (new G4VRML2);
#endif

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
#endif   // G4VIS_NONE 

// ============================================================================
// Th END 
// ============================================================================
