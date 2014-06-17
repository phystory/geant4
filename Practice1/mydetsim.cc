// --------------------------------------------------------------
//      GEANT 4 - practice01, main file = mydetsim.cc
// --------------------------------------------------------------

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "MyDetectorConstruction.hh"
#include "MyPhysicsList.hh"
#include "MyPrimaryGeneratorAction.hh"

int main(int argc, char** argv)
{
	G4RunManager *rm=new G4RunManager();

	G4VUserDetectorConstruction* mydetector = new MyDetectorConstruction;
	rm->SetUserInitialization(mydetector);

	G4VUserPhysicsList* myphysics = new MyPhysicsList;
	rm->SetUserInitialization(myphysics);

	G4VUserPrimaryGeneratorAction* mygenaction = new MyPrimaryGeneratorAction;
	rm->SetUserAction(mygenaction);

/*
	G4VUserRunAction* myrunaction = new RunAction;
	rm->SetUserAction(myrunaction);

	G4VUserEventAction* myeventaction = new EventAction;
	rm->SetUserAction(myeventaction);

	G4VUserSteppingAction* mysteppingaction = new SteppingAction;
	rm->SetUserAction(mysteppingaction);
*/

	rm->Initialize();

	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
	G4UImanager* UI = G4UImanager::GetUIpointer();

	if (argc!=1)   // batch mode
	{
		UI->ApplyCommand("/run/verbose 1");
		UI->ApplyCommand("/event/verbose 1");
		UI->ApplyCommand("/tracking/verbose 1");

		G4int numberOfEvent = 3;
		rm->BeamOn(numberOfEvent);
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UI->ApplyCommand(command+fileName);
	}
	else
	{  // interactive mode : define UI session
		G4UIExecutive* uie = new G4UIExecutive(argc, argv);
		UI->ApplyCommand("/control/execute vis.mac"); 
		if (uie->IsGUI())
		UI->ApplyCommand("/control/execute gui.mac");
		uie->SessionStart();
		delete uie;
	}

	delete visManager;
	delete rm;

	return 0;
}
