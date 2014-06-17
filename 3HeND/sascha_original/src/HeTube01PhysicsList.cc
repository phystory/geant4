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
// $Id: ExN01PhysicsList.cc,v 1.6 2006/06/29 17:47:21 gunter Exp $
// GEANT4 tag $Name: geant4-08-01-patch-01 $
//
// 

#include "HeTube01PhysicsList.hh"
#include "G4ParticleTypes.hh"
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"
#include "G4ParticleTable.hh"


// set of header files for neutron physics according to http://////www.slac.stanford.edu/comp/physics/geant4/slac_physics_lists/micro/physlistdoc.html#neutron

// neutron physics lists

// processes
#include "G4ProcessManager.hh"
#include "G4HadronElasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4HadronFissionProcess.hh"
#include "G4HadronCaptureProcess.hh"

// cross sections
#include "G4NeutronInelasticCrossSection.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPInelasticData.hh"
#include "G4NeutronHPFissionData.hh"
#include "G4NeutronHPCaptureData.hh"

// models
#include "G4LElastic.hh"
#include "G4CascadeInterface.hh"
#include "G4LENeutronInelastic.hh"
#include "G4LFission.hh"
#include "G4LCapture.hh"
#include "G4TheoFSGenerator.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4ExcitationHandler.hh"
#include "G4PreCompoundModel.hh"
#include "G4QGSMFragmentation.hh"
#include "G4ExcitedStringDecay.hh"
#include "G4QGSParticipants.hh"
#include "G4QGSModel.hh"
#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronHPFission.hh"
#include "G4NeutronHPCapture.hh"


//gamma and geantino physics lists

// processes
#include "G4GammaConversion.hh"
#include "G4ComptonScattering.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4PhotoNuclearProcess.hh"

// models
#include "G4GammaNuclearReaction.hh"
#include "G4TheoFSGenerator.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4QGSMFragmentation.hh"
#include "G4ExcitedStringDecay.hh"
#include "G4GammaParticipants.hh"
#include "G4QGSModel.hh"

#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4Gamma.hh"



#include "G4MultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4hLowEnergyIonisation.hh"
#include "G4hIonisation.hh"

#include "LHEP_PRECO_HP.hh" 


HeTube01PhysicsList::HeTube01PhysicsList()
                          :G4VUserPhysicsList()
{;}

HeTube01PhysicsList::~HeTube01PhysicsList()
{;}


//void HeTube01PhysicsList::ConstructParticle()
//{
//  G4Gamma::GammaDefinition();
//  G4Proton::ProtonDefinition();
  
//  G4IonConstructor ion;
//  ion.ConstructParticle();

  //}


void HeTube01PhysicsList::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  ConstructBosons();
  ConstructLeptons();
  ConstructMesons();
  ConstructBaryons();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HeTube01PhysicsList::ConstructBosons()
{
  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();

  // gamma
  G4Gamma::GammaDefinition();

  // optical photon
  G4OpticalPhoton::OpticalPhotonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HeTube01PhysicsList::ConstructLeptons()
{
  // leptons
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();

  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HeTube01PhysicsList::ConstructMesons()
{
 //  mesons
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4PionZero::PionZeroDefinition();
  G4Eta::EtaDefinition();
  G4EtaPrime::EtaPrimeDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();
  G4KaonZero::KaonZeroDefinition();
  G4AntiKaonZero::AntiKaonZeroDefinition();
  G4KaonZeroLong::KaonZeroLongDefinition();
  G4KaonZeroShort::KaonZeroShortDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HeTube01PhysicsList::ConstructBaryons()
{
//  barions
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();
}




void HeTube01PhysicsList::ConstructProcess()
{

AddTransportation();
ConstructEM();
ConstructNeutron();
ConstructGeneral();

}


void HeTube01PhysicsList::ConstructGeneral()
{

// general processes

//AddTransportation();

}

void HeTube01PhysicsList::ConstructNeutron()
{

// neutron processes

  G4bool HPN;
  if (getenv("NeutronHPCrossSections")) {
    G4cout << "physics list: Constructing high precision neutron models" << G4endl;
    G4cout << "              using cross sections in "
           << getenv("NeutronHPCrossSections") << G4endl;
    HPN = true;
  } else {
    G4cout << "physics list: Constructing non-high-precision neutron models" << G4endl;
    HPN = false;
  }

  G4ProcessManager* pManager = G4Neutron::Neutron()->GetProcessManager();

  G4double HPNLimit = 19.9*MeV;

  

  // Neutron capture process, models and cross sections

  G4HadronCaptureProcess* neutronCapture = new G4HadronCaptureProcess();
  G4LCapture* theLCaptureModel = new G4LCapture();
  theLCaptureModel->SetMaxEnergy(20.*TeV);
  if (HPN) {
    theLCaptureModel->SetMinEnergy(HPNLimit);
    G4NeutronHPCapture* theHPCaptureModel = new G4NeutronHPCapture();
   G4NeutronHPCaptureData* theHPCaptureData = new G4NeutronHPCaptureData();
    neutronCapture->RegisterMe(theHPCaptureModel);
    neutronCapture->AddDataSet(theHPCaptureData);
  }
  neutronCapture->RegisterMe(theLCaptureModel);
  pManager->AddDiscreteProcess(neutronCapture);

}

// 

void HeTube01PhysicsList::ConstructEM()

{

  theParticleIterator->reset();

  while( (*theParticleIterator)() ){

    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {

      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
      pmanager->AddDiscreteProcess(new G4ComptonScattering);
      pmanager->AddDiscreteProcess(new G4GammaConversion);

//   low energy part
//G4GammaNuclearReaction* lowEGammaModel = new G4GammaNuclearReaction();
//lowEGammaModel->SetMaxEnergy(25*MeV);
//G4PhotoNuclearProcess* thePhotoNuclearProcess = new G4PhotoNuclearProcess();
//thePhotoNuclearProcess->RegisterMe(lowEGammaModel);

       
     
    } 
/*
else if (particleName == "alpha") {
      
      pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
//      pmanager->AddProcess(new G4hIonisation,        -1, 2, 2);
      pmanager->AddProcess(new G4hLowEnergyIonisation(),-1, 2, 2);
    }
*/

else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {
      //all others charged particles except geantino
      pmanager->AddProcess(new G4MultipleScattering,-1, 1,1);
      pmanager->AddProcess(new G4hLowEnergyIonisation(),-1, 2, 2);
    }

  }

}







//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......








//void HeTube01PhysicsList::ConstructProcess()
//{
  // Define transportation process

//  AddTransportation();

//}

void HeTube01PhysicsList::SetCuts()
{
  //G4VUserPhysicsList::SetCutsWithDefault method sets 
  //the default cut value for all particle types 
  SetCutsWithDefault();
     
  if (verboseLevel>0) DumpCutValuesTable();
}

