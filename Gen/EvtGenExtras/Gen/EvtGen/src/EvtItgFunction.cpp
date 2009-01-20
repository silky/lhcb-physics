//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtItgFunction.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
//
// Description:
//	Class EvtItgFunction
//      Implementation for the EvtItgFunction class.  Defines the bounds
//      checked value() function and the non-bounds checked operator()
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Phil Strother           Originator
//
// Copyright Information: See EvtGen/COPYRIGHT
//	Copyright (C) 1998	LBNL
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "EvtGenModels/EvtItgFunction.hh"

//-------------
// C Headers --
//-------------
extern "C" {
}

//----------------
// Constructors --
//----------------
EvtItgFunction::EvtItgFunction( double (*theFunction)(double), double lowerRange, double upperRange):
  EvtItgAbsFunction(lowerRange, upperRange),
  _myFunction(theFunction)
{}


//--------------
// Destructor --
//--------------

EvtItgFunction::~EvtItgFunction( )
{}


double
EvtItgFunction::myFunction(double x) const{
  return _myFunction(x);
}

