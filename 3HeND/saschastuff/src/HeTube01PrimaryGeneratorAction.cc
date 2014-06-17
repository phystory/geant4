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
// 
//file date : 09-10-2007


#include "HeTube01PrimaryGeneratorAction.hh"

#include "HeTube01DetectorConstruction.hh"
#include "Randomize.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "G4Neutron.hh"

#include "MyStat.hh"

extern MyStat *MyStatistic;

HeTube01PrimaryGeneratorAction::HeTube01PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);
}

void HeTube01PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    
    //set the energy and the direction of the primary neutrons
    
    G4double energy = 0.01*MeV + 6.*MeV * G4UniformRand();
    particleGun->SetParticleEnergy(energy);

    G4double phi = 2.*acos(-1.)* G4UniformRand();
    G4double cosTheta = -1. + 2. * G4UniformRand();
    G4double sinTheta = sqrt(1. - cosTheta * cosTheta);

    G4double ux= sinTheta * cos(phi);
    G4double uy= sinTheta * sin(phi);
    G4double uz =cosTheta;

    particleGun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz));

    particleGun->SetParticleDefinition(G4Neutron::NeutronDefinition());
    particleGun->SetParticlePosition(G4ThreeVector(0.*m, 0.*m, 0.*m));

    particleGun->GeneratePrimaryVertex(anEvent);
    MyStatistic->NewEvent(particleGun->GetParticleEnergy());
}


HeTube01PrimaryGeneratorAction::~HeTube01PrimaryGeneratorAction()
{
  delete particleGun;
  
}
