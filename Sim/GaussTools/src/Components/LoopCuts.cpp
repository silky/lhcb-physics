// --------------------------------------------------------------

#include "LoopCuts.h"

#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"

LoopCuts::LoopCuts(const G4String& aName, int maxs, double minstep)
  : SpecialCuts(aName)
{ 

  m_maxstepnumber = maxs;
  m_minstep = minstep;
  
  if (verboseLevel>1) {
    G4cout << GetProcessName() << " is created "<< G4endl;
  }
  SetProcessType(fUserDefined);
}

LoopCuts::~LoopCuts()
{}

LoopCuts::LoopCuts(LoopCuts& right)
  :SpecialCuts(right)
{}

 
G4double LoopCuts::
PostStepGetPhysicalInteractionLength(const G4Track& track,
                                     G4double ,
                                     G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;
  // default proposed step
  G4double     proposedStep = DBL_MAX;

  double steplenght = track.GetStepLength();
  int stepn = track.GetCurrentStepNumber();
  
  if (stepn==1)
    {
      m_counter=0;
    }
  else if (steplenght < m_minstep) m_counter++;
  
  if ( m_counter > m_maxstepnumber ) 
    {          
      proposedStep = 0.;
    } 
  
  return proposedStep;    
}
  
