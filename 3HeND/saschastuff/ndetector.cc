//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: exampleN01.cc,v 1.6 2006/06/29 17:47:10 gunter Exp $
// GEANT4 tag $Name: geant4-08-01-patch-01 $
//
// 




#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4VisExecutive.hh"

#include "Randomize.hh"

#include "HeTube01DetectorConstruction.hh"
#include "HeTube01PrimaryGeneratorAction.hh"
#include "HeTube01SteppingAction.hh"

#include "HeTube01RunAction.hh"
#include "HeTube01EventAction.hh"
#include "HeTube01SteppingAction.hh"
#include "HeTube01TrackingAction.hh"

#include "QGSP_BIC_HP.hh"

#include "MyStat.hh"
#include "MyConfig.hh"

MyConfig *ConfigDet = new MyConfig();
MyStat *MyStatistic = new MyStat(ConfigDet->TotalNumberofTube());

int main()
{
  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  // set mandatory initialization classes
  G4VUserDetectorConstruction* detector = new HeTube01DetectorConstruction;
  runManager->SetUserInitialization(detector);
  //
  G4VUserPhysicsList* physics = new QGSP_BIC_HP;
  runManager->SetUserInitialization(physics);

  G4VUserPrimaryGeneratorAction* gen_action = new HeTube01PrimaryGeneratorAction;
  runManager->SetUserAction(gen_action);

  runManager->SetUserAction(new HeTube01RunAction);
  runManager->SetUserAction(new HeTube01EventAction);
  runManager->SetUserAction(new HeTube01TrackingAction);
  runManager->SetUserAction(new HeTube01SteppingAction);
  // Initialize G4 kernel
  runManager->Initialize();

  G4UImanager * UI = G4UImanager::GetUIpointer();


      UI->ApplyCommand("/control/execute run.mac");
  

  delete runManager;
  MyStatistic->PrintScreen();
  MyStatistic->SaveTree();
  return 0;

}


