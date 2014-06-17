#include "StepAction.hh"


StepAction::StepAction()
{
	moutFile.open("test.txt");
}


StepAction::~StepAction()
{
	moutFile.close();
}
////////////////////////////////////////////////////////////////////////////////////////////////
//USERRRRRR STEPPPPPPPPPPPING ACTIONNNNNNNNNNNNNNNNNNNNNNNNNN
///////////////////////////////////////////////////////////////////////////////////////////////

void StepAction::UserSteppingAction(const G4Step* step)
 {
 	G4double pi = 4.0*atan(1.0);
 
   G4Track* track = step -> GetTrack();
 
   G4double deltaEnergy = step -> GetDeltaEnergy() / MeV;
   G4double energyDeposited = step -> GetTotalEnergyDeposit() / MeV;
   G4int stepNum = track->GetCurrentStepNumber();   

   G4double globalTime = track -> GetGlobalTime() / ns;
   G4String particleName = track -> GetDefinition() -> GetParticleName();
   G4VPhysicalVolume* volume = track -> GetVolume();

   G4String volumeName = volume -> GetName();
   G4int copyNum = volume -> GetCopyNo();
   G4double KE = track->GetKineticEnergy();  
   
   G4ThreeVector ParticlePosition =  track->GetPosition();
   G4double Theta = ParticlePosition.getTheta();
   G4double Phi = ParticlePosition.getPhi();

   //G4TrackVector* Secondary = step->GetfSecondary();
   //G4int parentID = Secondary[0]->GetParentID();
   //G4String Process = track->GetCreatorProcess()->GetProcessName();
   //G4cout << SecName << " ParentID   " << G4endl;

//Prestep process'
   G4StepPoint* preStep = step->GetPreStepPoint();
   G4ThreeVector prePdirection = preStep->GetMomentumDirection();
   G4ThreeVector prePosition = preStep->GetPosition();
   G4StepStatus PreStepStatus = preStep->GetStepStatus();   

//Poststep process'
   G4StepPoint* postStep = step->GetPostStepPoint();
   G4ThreeVector postPdirection = postStep->GetMomentumDirection();
   G4ThreeVector postPosition = postStep->GetPosition();
   G4StepStatus PostStepStatus = postStep->GetStepStatus();

   G4TouchableHandle theTouchable = preStep->GetTouchableHandle();

   G4double GlobalTime = track->GetGlobalTime() / ns;
   
	//if( ( !( strncmp("DiCD",volumeName,4 )) || !( strncmp("DoCD",volumeName,4 )) ) && !( strncmp("He6",particleName,3 )) )
	{
   	//moutFile << "ParticleName: " << particleName << " volumeName " << volumeName << " EDep " << energyDeposited 
      //  	<< " stepNum " << stepNum << " PostStepStatus " << PostStepStatus << " PreStepStatus " << PreStepStatus 
      //   << " theta: " << Theta*180/pi << " phi " << Phi*180/pi << G4endl;
   }
 	//strcpy(mParticleName,particleName);
 	//strcpy(mVolumeName,volumeName); 
}
