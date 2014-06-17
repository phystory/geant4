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
// $Id: ExN02SteppingAction.cc,v 1.9 2006/06/29 17:48:18 gunter Exp $
// GEANT4 tag $Name: geant4-08-01-patch-01 $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//file date : 08-08-2007



#include "HeTube01SteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include "G4Step.hh"
#include "MyStat.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

extern MyStat *MyStatistic;

HeTube01SteppingAction::HeTube01SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HeTube01SteppingAction::UserSteppingAction(const G4Step* aStep)
{ 
 G4Track * theTrack = aStep->GetTrack();
 G4String partname = theTrack->GetDefinition()->GetParticleName();
 G4StepPoint * thePrePoint = aStep->GetPreStepPoint();

 //This part comes from Manoel Couder, mcouder@nd.edu
 
//  Things to do if proton in tube with reaction ....
 if ((partname=="proton") && 
     (MyStatistic->have_reacted_DoIt_proton==theTrack->GetTrackID()) && 
     (MyStatistic->have_reacted_Done_proton==0))
  {
     MyStatistic->nb_proton++;
   //Here come the
     const G4ThreeVector vec = thePrePoint->GetPosition();
     MyStatistic->tube_number_reaction=MyStatistic->FindTube(vec.x(),vec.y(),vec.z());
     MyStatistic->nb_reaction_tube[MyStatistic->tube_number_reaction]++;
     MyStatistic->have_reacted_Done_proton=1;
  }

//  Things to do if proton in tube with reaction ....
  if ((partname=="triton") &&
      (MyStatistic->have_reacted_DoIt_triton==theTrack->GetTrackID()) && 
      (MyStatistic->have_reacted_Done_triton==0))
   {
    MyStatistic->nb_triton++;
    MyStatistic->have_reacted_Done_triton=1;
   }

 // check if it is alive
  if(theTrack->GetTrackStatus()!=fAlive)return;
  if (partname!="neutron") return;

//Not all of it is needed but we need to know if newtron enter in a tube
  if(theTrack->GetTrackStatus()==fAlive){
   G4StepPoint * thePostPoint = aStep->GetPostStepPoint();
   G4VPhysicalVolume * thePrePV = thePrePoint->GetPhysicalVolume();
   G4VPhysicalVolume * thePostPV = thePostPoint->GetPhysicalVolume();
   G4String thePrePVname = thePrePV->GetName();
   G4String thePostPVname = thePostPV->GetName();

  if (partname=="neutron") if (thePrePVname!=thePostPVname)
  if ((thePrePVname!=thePostPVname)&&
      (thePostPVname=="HeTube_Param_phys") &&
      (!(MyStatistic->in_tube)) &&
      (partname=="neutron"))
   {
     MyStatistic->in_tube=1;
     //Here come the
     const G4ThreeVector vec = thePrePoint->GetPosition();
     MyStatistic->tube_number_neutron=MyStatistic->FindTube(vec.x(),vec.y(),vec.z());
     MyStatistic->nb_neutron_tube[MyStatistic->tube_number_neutron]++;
   }
}

}
