#ifndef StepAction_H
#define StepAction_H 1

#include <list>
#include <iomanip>
#include <cstring>
#include <fstream>
using namespace std;

#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>
#include <TBranch.h>

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4TrackVector.hh"
#include "G4ParticleTypes.hh"
#include "G4SteppingManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4UserSteppingAction.hh"


class StepAction:public G4UserSteppingAction
{
   public:
      StepAction();
      ~StepAction();
      void UserSteppingAction(const G4Step* step);

   private:

		ofstream				  moutFile;
};

#endif



