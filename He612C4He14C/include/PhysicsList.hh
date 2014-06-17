#ifndef PhysicsList_H
#define PhysicsList_H 1

#include "globals.hh"
#include "G4Decay.hh"
#include "G4eIonisation.hh"
#include "G4hIonisation.hh"
#include "G4MuIonisation.hh"
#include "G4ParticleTypes.hh"
#include "G4IonConstructor.hh"
#include "G4ProcessManager.hh"
#include "G4eBremsstrahlung.hh"
#include "G4GammaConversion.hh"
#include "G4BosonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4eplusAnnihilation.hh"
#include "G4BaryonConstructor.hh"
#include "G4ComptonScattering.hh"
#include "G4LeptonConstructor.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4ShortLivedConstructor.hh"
#include <G4BinaryLightIonReaction.hh>
#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#include "G4ionIonisation.hh"       
#include <G4TripathiCrossSection.hh>
#include <G4IonsShenCrossSection.hh>
//Me Cleaning up the neutron process
#include "G4HadronElasticProcess.hh"
#include "G4HadronCaptureProcess.hh"

// Hadronic processes ////////////////////////////////////////////////////////

// Elastic processes:
#include "G4HadronElasticProcess.hh"

// Inelastic processes:
#include "G4ProtonInelasticProcess.hh"
#include "G4AntiProtonInelasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4AntiNeutronInelasticProcess.hh"
#include "G4DeuteronInelasticProcess.hh"
#include "G4TritonInelasticProcess.hh"
#include "G4AlphaInelasticProcess.hh"

// Low-energy Models: < 20GeV
#include "G4LElastic.hh"
#include "G4LEProtonInelastic.hh"
#include "G4LEAntiProtonInelastic.hh"
#include "G4LENeutronInelastic.hh"
#include "G4LEAntiNeutronInelastic.hh"
#include "G4LEDeuteronInelastic.hh"
#include "G4LETritonInelastic.hh"
#include "G4LEAlphaInelastic.hh"

// High-energy Models: >20 GeV
#include "G4HEProtonInelastic.hh"
#include "G4HEAntiProtonInelastic.hh"
#include "G4HENeutronInelastic.hh"
#include "G4HEAntiNeutronInelastic.hh"

// Neutron high-precision models: <20 MeV
#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPCapture.hh"
#include "G4NeutronHPCaptureData.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronHPInelasticData.hh"

#include "G4LCapture.hh"

#include "G4NeutronInelasticCrossSection.hh"
#include "G4HadronFissionProcess.hh"
#include "G4PionPlusInelasticProcess.hh"
#include "G4LEPionPlusInelastic.hh"
#include "G4HEPionPlusInelastic.hh"
#include "G4PionMinusInelasticProcess.hh"
#include "G4LEPionMinusInelastic.hh"
#include "G4HEPionMinusInelastic.hh"
#include "G4ProtonInelasticCrossSection.hh"

// Stopping processes
#include "G4AntiProtonAnnihilationAtRest.hh"
#include "G4AntiNeutronAnnihilationAtRest.hh"


//Optical Physics
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"

#include <iostream>
//#include "GLG4Scint.hh"


class PhysicsList:public G4VUserPhysicsList
 {

   public:
      PhysicsList();
      virtual ~PhysicsList();
      
      G4int OpVerbLevel;  // from VANDLE code

   protected:
      void ConstructParticle();
      void ConstructProcess();
		virtual void    ConstructIons(void);      
      void SetCuts();

   private:
      void ConstructGeneral();
      void ConstructEM();
      virtual void ConstructHad();
      virtual void ConstructOp();
     
      
     G4Cerenkov*          theCerenkovProcess;
     G4Scintillation*     theScintillation;
     G4OpAbsorption*      theAbsorptionProcess;
     G4OpRayleigh*        theRayleighScatteringProcess;
     G4OpBoundaryProcess* theBoundaryProcess;
     
    G4DeuteronInelasticProcess      theIPdeuteron;
    G4TritonInelasticProcess        theIPtriton;
    G4AlphaInelasticProcess         theIPalpha;
    G4HadronInelasticProcess*       theIPGenericIon;     

 };

#endif



