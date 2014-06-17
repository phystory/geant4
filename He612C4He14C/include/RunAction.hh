#ifndef RunAction_H
#define RunAction_H 1

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UserRunAction.hh"
#include "PrimaryGenerator.hh"


class G4Run;
class RunAction:public G4UserRunAction
{
   public:
      RunAction();
      ~RunAction();
      void BeginOfRunAction(const G4Run* run);
      void EndOfRunAction(const G4Run*);

   private:
      G4int runNumber;
};

#endif



