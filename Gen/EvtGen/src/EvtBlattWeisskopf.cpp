/*****************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtBlattWeisskopf.cpp,v 1.1 2003-10-02 17:34:48 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *****************************************************************************/

#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <iostream>
#include <assert.h>
#include <math.h>
#include "EvtGenBase/EvtBlattWeisskopf.hh"
#include "EvtGenBase/EvtReport.hh"

EvtBlattWeisskopf::EvtBlattWeisskopf(int LL, double R, double p0)
  : _LL(LL), _R(R), _p0(p0)
{
  if(R < 0) {

    report(INFO,"EvtGen") << "Radius " << R << " negative" << std::endl;
    assert(0);
  }

  _R = R;

  // compute formula for nominal momentum

  _F0 = compute(_p0);
  if(_F0 <= 0) {
    
    report(INFO,"EvtGen") << "Invalid nominal form factor computed " 
                          << _F0 << std::endl;
    assert(0);
  } 
}

EvtBlattWeisskopf::EvtBlattWeisskopf(const EvtBlattWeisskopf& other)
  : _LL(other._LL), _R(other._R), _p0(other._p0), _F0(other._F0)
{}

EvtBlattWeisskopf::~EvtBlattWeisskopf()
{}

double EvtBlattWeisskopf::operator()(double p) const
{
  double ret = compute(p)/_F0;
  //  report(INFO,"EvtGen") << p << " " << _p0 << " " << _F0 << " " 
  //<< _LL << " " << _R << " " << ret << std::endl;
  return ret;
}

// Blatt-Weisskopf form factors
// see e.g. hep-ex/0011065
// Dalitz Analysis of the Decay D0->K-pi+pi0 (CLEO)
//
// p   - momentum of either daugher in the meson rest frame,
//       the mass of the meson is used
// pAB - momentum of either daughter in the candidate rest frame
//       the mass of the candidate is used
// R - meson radial parameter
// 
// In the CLEO paper R=5 GeV-1 for D0, R=1.5 for intermediate resonances

double EvtBlattWeisskopf::compute(double p) const
{
  if(p < 0) {
    
    report(INFO,"EvtGen") << "Momentum " << p 
                          << " negative in form factor calculation" 
                          << std::endl;
    assert(0);
  }
  else {
    
    double x = p*p*_R*_R;
    
    if(0 == _LL) return 1.;
    else
      if(1 == _LL) return sqrt(1.0/(1.0+x));
      else
	if(2 == _LL) return sqrt(1.0/(1.0+x/3.0+x*x/9.0));
	else {
	  report(INFO,"EvtGen") << "Angular momentum " << _LL 
                          << " not implemented" << std::endl;
	  assert(0);
	}
  }
}

