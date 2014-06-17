// --------------------------------------------------------------
//      GEANT 4 - practice01, main file = mydetsim.cc
// --------------------------------------------------------------

#include "MyPhysicsList.hh"
#include "G4VProcess.hh"
#include "G4ParticleTypes.hh"
#include "G4Transportation.hh"
#include "G4ProcessManager.hh"
#include "G4hMultipleScattering.hh"


MyPhysicsList::MyPhysicsList():  G4VUserPhysicsList()
{
  defaultCutValue = 1.0*mm;
  SetVerboseLevel(1);
}

MyPhysicsList::~MyPhysicsList()
{;}

void MyPhysicsList::ConstructParticle()
{
/*
  G4Proton::ProtonDefinition();
*/
}

void MyPhysicsList::ConstructProcess()
{
  AddTransportation();
  //ConstructMultiScattering();
}

void MyPhysicsList::ConstructMultiScattering()
{
/*
  G4ParticleDefinition* proton = G4Proton::ProtonDefinition();
  G4ProcessManager* pman=proton->GetProcessManager();
  //G4MultipleScattering,G4eIonisation,G4eBremsstrahlung
  G4hMultipleScattering* theMultipleScattering = new G4hMultipleScattering;
  pman->AddDiscreteProcess((G4VProcess *)theMultipleScattering);
*/
}

void MyPhysicsList::SetCuts()
{
/*
  G4int temp = GetVerboseLevel();
  SetVerboseLevel(0);                                                           
  SetCutsWithDefault();   
  SetVerboseLevel(temp);  
*/
}

