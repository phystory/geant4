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

#include "HeTube01SteppingAction.hh"
#include "G4SteppingManager.hh"


//from example N03

#include "HeTube01DetectorConstruction.hh"
#include "HeTube01EventAction.hh"

#include "G4Step.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HeTube01SteppingAction::HeTube01SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HeTube01SteppingAction::UserSteppingAction(const G4Step* aStep)
{ 
 G4Track * theTrack = aStep->GetTrack();
 
 if (theTrack->GetCurrentStepNumber() > 100)
 {
    theTrack->SetTrackStatus(fStopAndKill);
    G4cout<<"trashed one infinite step loop!"<<G4endl;
 } 


 if(theTrack->GetTrackStatus()==fAlive){ 
  G4StepPoint * thePrePoint = aStep->GetPreStepPoint();
  G4VPhysicalVolume * thePrePV = thePrePoint->GetPhysicalVolume();
  G4String thePrePVname = thePrePV->GetName();
  G4StepPoint * thePostPoint = aStep->GetPostStepPoint();
  G4VPhysicalVolume * thePostPV = thePostPoint->GetPhysicalVolume();
  G4String thePostPVname = thePostPV->GetName();
  G4ThreeVector prevPos = thePrePoint->GetPosition();
  G4ThreeVector postPos = thePostPoint->GetPosition();
  G4ThreeVector postMom = thePostPoint->GetMomentumDirection();

//  G4cout <<"---------------------------------------"<<G4endl;
  G4cout << theTrack->GetDefinition()->GetParticleName() << G4endl;
//  G4cout << "thePrePVname = "<< thePrePVname << G4endl;
//  G4cout << "thePostPVname = "<< thePostPVname << G4endl;
//  G4cout << "prevPos = "<< prevPos << G4endl;
//  G4cout << "postPos = " <<postPos <<G4endl;
//  G4cout << "postMom = " << postMom << G4endl;
//  G4cout <<"---------------------------------------"<<G4endl;
 } 


//from example N03

/*G4VPhysicalVolume* volume 
  = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();


  
  G4double stepl = 0.;
  if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.)
    stepl = aStep->GetStepLength();

  G4cout <<"---------------------------------------"<<G4endl;
  G4cout << theTrack->GetDefinition()->GetParticleName() << G4endl;
 // G4cout << aStep->GetVolume()<< G4endl;<<G4endl;
*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

