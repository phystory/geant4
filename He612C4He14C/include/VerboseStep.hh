#ifndef VerboseStep_H
#define VerboseStep_H 1

#include "G4UnitsTable.hh"
#include "G4SteppingManager.hh"
#include "G4SteppingVerbose.hh"


class VerboseStep:public G4SteppingVerbose 
 {

   public:
      VerboseStep();
      ~VerboseStep();
      void StepInfo();
      void TrackingStarted();

 };

#endif



