#ifndef G4NeutronBuilder_h
#define G4NeutronBuilder_h 1

#include<vector>
#include "globals.hh"

#include "G4HadronElasticProcess.hh"
#include "G4HadronFissionProcess.hh"
#include "G4HadronCaptureProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4VNeutronBuilder.hh"


class G4NeutronBuilder
{
  public: 
    G4NeutronBuilder();
    virtual ~G4NeutronBuilder();

  public: 
    void Build();
    void RegisterMe(G4VNeutronBuilder * aB) {theModelCollections.push_back(aB);}

  private:
    G4HadronElasticProcess theNeutronElasticProcess;
    G4NeutronInelasticProcess  theNeutronInelastic;
    G4HadronFissionProcess theNeutronFission;
    G4HadronCaptureProcess  theNeutronCapture;
    
    std::vector<G4VNeutronBuilder *> theModelCollections;

};

// 2002 by J.P. Wellisch

#endif

