#ifndef G4ProtonBuilder_h
#define G4ProtonBuilder_h 1

#include<vector>
#include "globals.hh"

#include "G4HadronElasticProcess.hh"
#include "G4ProtonInelasticProcess.hh"
#include "G4VProtonBuilder.hh"

class G4ProtonBuilder
{
  public: 
    G4ProtonBuilder();
    virtual ~G4ProtonBuilder();

  public: 
    void Build();
    void RegisterMe(G4VProtonBuilder * aB) {theModelCollections.push_back(aB);}

  private:
    G4HadronElasticProcess theProtonElasticProcess;
    G4ProtonInelasticProcess  theProtonInelastic;
    
    std::vector<G4VProtonBuilder *> theModelCollections;

};

// 2002 by J.P. Wellisch

#endif

