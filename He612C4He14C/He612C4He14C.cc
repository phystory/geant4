#ifndef He612C4He14C_H
#define He612C4He14C_H 1

#include "QGSP_BERT_HP.hh"
#include "QGSP_BERT.hh"
#include "HadronPhysicsQGSP_BERT.hh"
#include "QGSP_BERT_HP.hh"
#include "QGSP_BIC_HP.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4UIterminal.hh"

#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>

#include "RunAction.hh"
#include "PhysicsList.hh"
#include "StepAction.hh"
#include "EventAction.hh"
#include "VerboseStep.hh"
#include "PrimaryGenerator.hh"
#include "DetectorConstruction.hh"
#include "DataPass.hh"

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

#include <iostream>
using namespace std;

int main(int argc, char** argv)
 {
 
 	DataPass LightInfoPass; 
 
   G4VSteppingVerbose* verbosity = new VerboseStep;
   G4VSteppingVerbose::SetInstance(verbosity);

   G4RunManager* runManager = new G4RunManager;
   runManager -> SetUserInitialization(new DetectorConstruction());
   //runManager -> SetUserInitialization(new PhysicsList);
   //runManager -> SetUserInitialization(new QGSP_BERT_HP());
   runManager -> SetUserInitialization(new QGSP_BIC_HP());   
   runManager -> SetUserAction(new PrimaryGenerator( &LightInfoPass ) );
   runManager -> SetUserAction(new RunAction());
   runManager -> SetUserAction(new EventAction( &LightInfoPass ));
   runManager -> SetUserAction(new StepAction());

   runManager -> Initialize();

   G4UImanager* UI = G4UImanager::GetUIpointer();
   #if defined(G4VIS_USE)

   G4VisManager* visManager = new G4VisExecutive;
   visManager->Initialize();

   #endif

   if (argc == 1)
    {
 
      G4UIsession* session = 0;

      #if defined(G4UI_USE_TCSH)

         session = new G4UIterminal(new G4UItcsh);

      #else

         session = new G4UIterminal();

      #endif

      UI -> ApplyCommand("/control/execute macro/volumeHepRep.mac");
      session -> SessionStart();
      delete session;

    }
   else 
    {

      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UI -> ApplyCommand(command + fileName);

    }

   #if defined(G4VIS_USE)

      delete visManager;

   #endif

   //rootFile -> Write();
   //rootFile -> Close();

   delete runManager;
   delete verbosity;

   return 0; 
 }



