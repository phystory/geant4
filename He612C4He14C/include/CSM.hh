#ifndef CSM_H
#define CSM_H 1

#include "QGSP_BERT.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4UIterminal.hh"

#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>

#include "global.hh"
#include "RunAction.hh"
#include "StepAction.hh"
#include "EventAction.hh"
#include "PhysicsList.hh"
#include "VerboseStep.hh"
#include "PrimaryGenerator.hh"
#include "DetectorConstruction.hh"

#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"
#endif

#ifdef G4UI_USE_XM
#include "G4UIXm.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#endif



