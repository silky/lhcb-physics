#ifndef G4PiKBuilder_h
#define G4PiKBuilder_h 1

#include<vector>

#include "globals.hh"

#include "G4HadronElasticProcess.hh"
#include "G4ProtonInelasticProcess.hh"
#include "G4VPiKBuilder.hh"

class G4PiKBuilder
{
  public: 
    G4PiKBuilder();
    virtual ~G4PiKBuilder();

  public: 
    void Build();
    void RegisterMe(G4VPiKBuilder * aB) {theModelCollections.push_back(aB);}

  private:
    G4HadronElasticProcess thePionPlusElasticProcess;
    G4HadronElasticProcess thePionMinusElasticProcess;
    G4HadronElasticProcess theKaonPlusElasticProcess;
    G4HadronElasticProcess theKaonMinusElasticProcess;
    G4HadronElasticProcess theKaonZeroLElasticProcess;
    G4HadronElasticProcess theKaonZeroSElasticProcess;

    G4PionPlusInelasticProcess  thePionPlusInelastic;
    G4PionMinusInelasticProcess thePionMinusInelastic;
    G4KaonPlusInelasticProcess  theKaonPlusInelastic;
    G4KaonMinusInelasticProcess theKaonMinusInelastic;
    G4KaonZeroLInelasticProcess theKaonZeroLInelastic;
    G4KaonZeroSInelasticProcess theKaonZeroSInelastic;
     
    std::vector<G4VPiKBuilder *> theModelCollections;

};

// 2002 by J.P. Wellisch

#endif

