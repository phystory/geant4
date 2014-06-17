#include "VerboseStep.hh"


VerboseStep::VerboseStep()
 {}


VerboseStep::~VerboseStep()
 {}


void VerboseStep::StepInfo()
 {

   CopyState();
   G4VPhysicalVolume* volume = fTrack -> GetVolume();
   G4int prec = G4cout.precision(3);

   if (verboseLevel >= 1)
    {

      if (verboseLevel >= 4)
       {

         VerboseTrack();

       }

      if (verboseLevel >= 3)
       {

         G4cout << G4endl << std::setw(6) << "Step #" << std::setw(10) << "X (cm)" << std::setw(10) << "Y (cm)" << std::setw(10) << "Z (cm)";
         G4cout << std::setw(15) << "KinE (MeV)" << std::setw(15) << "Copy Num" << std::setw(20) << "Step Length (cm)";
         G4cout << std::setw(20) << "Track Length (cm)" << std::setw(20) << "Volume" << std::setw(20) << "Process" << G4endl;

       }

      G4cout << std::setw(6) << fTrack -> GetCurrentStepNumber() << std::setw(10) << fTrack -> GetPosition().x() / cm;
      G4cout << std::setw(10) << fTrack -> GetPosition().y() / cm << std::setw(10) << fTrack -> GetPosition().z() / cm;
      G4cout << std::setw(15) << fTrack -> GetKineticEnergy() / MeV << std::setw(15) << fTrack->GetLocalTime() / ns;
      G4cout << std::setw(20) << fStep -> GetDeltaEnergy() / MeV << std::setw(20) << fTrack -> GetVolume() -> GetCopyNo();

      if (fTrack -> GetNextVolume() != 0)
       {

         G4cout << std::setw(20) << fTrack->GetVolume()->GetName();

       }
      else
       {

         G4cout << std::setw(20) << "Out Of World";

       }

      if (fStep -> GetPostStepPoint() -> GetProcessDefinedStep() != NULL)
       {

         G4cout << std::setw(20) << fStep -> GetPostStepPoint() -> GetProcessDefinedStep() ->GetProcessName();

       }
      else
       {

         G4cout << std::setw(20) << "User Limit";

       }

      G4cout << G4endl;

      if (verboseLevel == 2)
       {

         G4int tN2ndariesTot = fN2ndariesAtRestDoIt + fN2ndariesAlongStepDoIt + fN2ndariesPostStepDoIt;

         if (tN2ndariesTot > 0)
          {

            G4cout << G4endl << std::setw(58) << ":------ List of Secondaries ------ # Spawn In Step =" << std::setw(2) << tN2ndariesTot;
            G4cout << std::setw(8) << "(Rest =" << std::setw(2) << fN2ndariesAtRestDoIt << ", Along =" << std::setw(2) << fN2ndariesAlongStepDoIt;
            G4cout << ", Post =" << std::setw(2) << fN2ndariesPostStepDoIt << "), # Spawn Total =" << std::setw(2) << (*fSecondary).size();
            G4cout << std::setw(7) << "------" << G4endl;

            for (size_t lp1 = (*fSecondary).size() - tN2ndariesTot; lp1 < (*fSecondary).size(); lp1 ++)
             {

               G4cout << std::setw(7) << ":" << std::setw(9) << (*fSecondary)[lp1] -> GetPosition().x() / cm;
               G4cout << std::setw(10) << (*fSecondary)[lp1] -> GetPosition().y() / cm << std::setw(10) << (*fSecondary)[lp1] -> GetPosition().z() / cm;
               G4cout << std::setw(15) << (*fSecondary)[lp1] -> GetKineticEnergy() / MeV;
               G4cout << std::setw(20) << (*fSecondary)[lp1] -> GetDefinition() -> GetParticleName() << G4endl;

             }

            G4cout << std::setw(118) << ":------------------------------------------- End Of Secondaries Info -------------------------------------------";
            G4cout << G4endl << G4endl;

          }

       }

    }

   G4cout.precision(prec);

 }


void VerboseStep::TrackingStarted()
 {

   CopyState();

   G4int prec = G4cout.precision(3);

   if (verboseLevel > 0)
    {

      G4cout << std::setw(6) << "Step #" << std::setw(10) << "X (cm)" << std::setw(10) << "Y (cm)" << std::setw(10) << "Z (cm)";
      G4cout << std::setw(15) << "KinE (MeV)" << std::setw(15) << "GlobalTime(ns)" << std::setw(20) << "DeltaE" << std::setw(20) << "CopyNum";
      G4cout << std::setw(20) << "Volume" << std::setw(20) << "Process" << G4endl;

      G4cout << std::setw(6) << fTrack -> GetCurrentStepNumber() << std::setw(10) << fTrack -> GetPosition()[0] / cm;
      G4cout << std::setw(10) << fTrack->GetPosition()[1] / cm << std::setw(10) << fTrack -> GetPosition().z() / cm;
      G4cout << std::setw(15) << fTrack -> GetKineticEnergy() / MeV << std::setw(15) << fStep -> GetTotalEnergyDeposit() / MeV;
      G4cout << std::setw(20) << fStep -> GetDeltaEnergy() / cm << std::setw(20) << fTrack -> GetVolume() -> GetCopyNo();

      if (fTrack -> GetNextVolume())
       {

         G4cout << std::setw(20) << fTrack -> GetVolume() -> GetName();

       }
      else
       {

         G4cout << std::setw(20) << "OutOfWorld";

       }

      G4cout << std::setw(20) << "initStep" << G4endl;

    }

   G4cout.precision(prec);

 }



