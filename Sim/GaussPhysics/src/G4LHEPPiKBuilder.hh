#ifndef G4LHEPPiKBuilder_h
#define G4LHEPPiKBuilder_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4LElastic.hh"
#include "G4VPiKBuilder.hh"

#include "G4LEPionPlusInelastic.hh"
#include "G4LEPionMinusInelastic.hh"
#include "G4LEKaonPlusInelastic.hh"
#include "G4LEKaonZeroSInelastic.hh"
#include "G4LEKaonZeroLInelastic.hh"
#include "G4LEKaonMinusInelastic.hh"

#include "G4HEPionPlusInelastic.hh"
#include "G4HEPionMinusInelastic.hh"
#include "G4HEKaonPlusInelastic.hh"
#include "G4HEKaonZeroInelastic.hh"
#include "G4HEKaonZeroInelastic.hh"
#include "G4HEKaonMinusInelastic.hh"

class G4LHEPPiKBuilder : public G4VPiKBuilder
{
  public: 
    G4LHEPPiKBuilder();
    virtual ~G4LHEPPiKBuilder();
    
  public: 
    virtual void Build(G4HadronElasticProcess & aP);
    virtual void Build(G4PionPlusInelasticProcess & aP);
    virtual void Build(G4PionMinusInelasticProcess & aP);
    virtual void Build(G4KaonPlusInelasticProcess & aP);
    virtual void Build(G4KaonMinusInelasticProcess & aP);
    virtual void Build(G4KaonZeroLInelasticProcess & aP);
    virtual void Build(G4KaonZeroSInelasticProcess & aP);
    
    void SetMinEnergy(G4double aM) 
    {
      theM=aM;
      theMinPion=theM;
    }
    void SetMinPionEnergy(G4double aM)
    {
      theMinPion = aM;
    }
    
  private:
    G4double theM;
    G4double theMinPion;
    
    G4LElastic * theElasticModel;
    
    // Pi + 
    G4LEPionPlusInelastic* theLEPionPlusModel;
    G4HEPionPlusInelastic* theHEPionPlusModel;

    // Pi -
    G4LEPionMinusInelastic* theLEPionMinusModel;
    G4HEPionMinusInelastic* theHEPionMinusModel;

    // K + 
    G4LEKaonPlusInelastic* theLEKaonPlusModel;
    G4HEKaonPlusInelastic* theHEKaonPlusModel;

    // K -
    G4LEKaonMinusInelastic* theLEKaonMinusModel;
    G4HEKaonMinusInelastic* theHEKaonMinusModel;

    // K0L
    G4LEKaonZeroLInelastic* theLEKaonZeroLModel;
    G4HEKaonZeroInelastic* theHEKaonZeroLModel;

    // K0S
    G4LEKaonZeroSInelastic* theLEKaonZeroSModel;
    G4HEKaonZeroInelastic* theHEKaonZeroSModel;

};
// 2002 by J.P. Wellisch

#endif
