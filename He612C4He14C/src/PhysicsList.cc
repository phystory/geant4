#include "PhysicsList.hh"

PhysicsList::PhysicsList():G4VUserPhysicsList()
 {
   theCerenkovProcess           = 0;
   theScintillation             = 0;
   theAbsorptionProcess         = 0;
   theRayleighScatteringProcess = 0;
   theBoundaryProcess           = 0;
  
   defaultCutValue = 1.0 * mm;
   SetVerboseLevel(1);
	OpVerbLevel = 0;  // from VANDLE code   

 }


PhysicsList::~PhysicsList()
 {}

//---- ConstructIons
void PhysicsList::ConstructIons(void)
{

}

void PhysicsList::ConstructParticle()
 {
	// Ions 
	G4IonConstructor iConstructor;	
	iConstructor.ConstructParticle();
	
	G4Deuteron::DeuteronDefinition();
	G4Triton::TritonDefinition();
	G4He3::He3Definition();
	G4Alpha::AlphaDefinition();
	G4GenericIon::GenericIonDefinition();
 
// Baryons
   G4BaryonConstructor baryons;
   baryons.ConstructParticle();

// Bosons (including geantinos)
   G4BosonConstructor bosons;
   bosons.ConstructParticle();
   
   G4OpticalPhoton::OpticalPhotonDefinition();

// Leptons
   G4LeptonConstructor leptons;
   leptons.ConstructParticle();

// Construct all mesons
   G4MesonConstructor mesons;
   mesons.ConstructParticle();

// Resonances and quarks
   G4ShortLivedConstructor shortLiveds;
   shortLiveds.ConstructParticle();
 }


void PhysicsList::ConstructProcess()
{
  AddTransportation();
  ConstructEM();
  ConstructHad();
  ConstructGeneral();
  ConstructOp();
}


void PhysicsList::ConstructEM()
 {

   theParticleIterator -> reset();
  
   while((*theParticleIterator)())
    {

      G4ParticleDefinition* particle = theParticleIterator -> value();
      G4ProcessManager* pmanager = particle -> GetProcessManager();
      G4String particleName = particle -> GetParticleName();

      if (particleName == "gamma")
       {

         pmanager -> AddDiscreteProcess(new G4GammaConversion());
         pmanager -> AddDiscreteProcess(new G4ComptonScattering());      
         pmanager -> AddDiscreteProcess(new G4PhotoElectricEffect());

       }
      else if (particleName == "e-")
       {

         pmanager -> AddProcess(new G4eMultipleScattering, -1, 1, 1);
         pmanager -> AddProcess(new G4eIonisation,        -1, 2, 2);
         pmanager -> AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
       }
      else if (particleName == "e+")
       {

         pmanager -> AddProcess(new G4eMultipleScattering, -1, 1, 1);
         pmanager -> AddProcess(new G4eIonisation,        -1, 2, 2);
         pmanager -> AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
         pmanager -> AddProcess(new G4eplusAnnihilation,   0,-1, 4);

       }
      else if ( particleName == "GenericIon")
      {
         pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
   	   pmanager->AddProcess(new G4ionIonisation,      -1, 2, 2);
	   }       
		else if( particleName == "alpha" || particleName == "GenericIon" ) { 
      	pmanager->AddProcess(new G4hMultipleScattering,-1, 1,1);
      	pmanager->AddProcess(new G4ionIonisation,     -1, 2,2);

     }	   
      else if(particleName == "mu+" || particleName == "mu-")
       {

         pmanager -> AddProcess(new G4MuMultipleScattering, -1, 1, 1);
         pmanager -> AddProcess(new G4MuIonisation,       -1, 2, 2);
         pmanager -> AddProcess(new G4MuBremsstrahlung,   -1, 3, 3);
         pmanager -> AddProcess(new G4MuPairProduction,   -1, 4, 4);

       }

      else if ((!particle -> IsShortLived()) && (particle -> GetPDGCharge() != 0.0) && (particle -> GetParticleName() != "chargedgeantino"))
       {

         pmanager -> AddProcess(new G4hMultipleScattering, -1, 1, 1);
         pmanager -> AddProcess(new G4hIonisation,        -1, 2, 2);

       }

    }

 }

void PhysicsList::ConstructHad() 
{
	G4HadronElasticProcess*			elasticProcess = new G4HadronElasticProcess();
	G4LElastic*							elasticModel = new G4LElastic();

   // Binary Cascade
   G4ParticleDefinition* particle = 0;
   G4ProcessManager* pmanager = 0;
   G4BinaryLightIonReaction* theBC = new G4BinaryLightIonReaction();
   theBC->SetMinEnergy(80*MeV);
   theBC->SetMaxEnergy(20*GeV);

   G4TripathiCrossSection * TripathiCrossSection= new G4TripathiCrossSection;
   G4IonsShenCrossSection * aShen = new G4IonsShenCrossSection;

	elasticProcess->RegisterMe(elasticModel);

   theParticleIterator->reset();
   while ((*theParticleIterator)()) 
   {
   particle = theParticleIterator->value();
   pmanager = particle->GetProcessManager();
   G4String                particleName = particle->GetParticleName();

   if ( pmanager == NULL)
	{
		particle->SetProcessManager(new G4ProcessManager(particle));
		pmanager = particle->GetProcessManager();
	}

	//elastic
	if ( particle->GetPDGMass() > 110.*MeV && elasticProcess->IsApplicable(*particle) && !(particle->IsShortLived()) )
	{
		pmanager->AddDiscreteProcess(elasticProcess);
	}

	if( particleName == "deuteron" )
	{
   // deuteron
   particle = G4Deuteron::Deuteron();
   pmanager = particle->GetProcessManager();
   G4LEDeuteronInelastic* theDIModel = new G4LEDeuteronInelastic;
   theDIModel->SetMaxEnergy(100*MeV);
   theIPdeuteron.AddDataSet(TripathiCrossSection);
   theIPdeuteron.AddDataSet(aShen);
   theIPdeuteron.RegisterMe(theDIModel);
   theIPdeuteron.RegisterMe(theBC);
   pmanager->AddDiscreteProcess(&theIPdeuteron);
	}
	
	if( particleName == "triton" )	
	{
   // triton
   particle = G4Triton::Triton();
   pmanager = particle->GetProcessManager();
   G4LETritonInelastic* theTIModel = new G4LETritonInelastic;
   theTIModel->SetMaxEnergy(100*MeV);
   theIPtriton.AddDataSet(TripathiCrossSection);
   theIPtriton.AddDataSet(aShen);
   theIPtriton.RegisterMe(theTIModel);
   theIPtriton.RegisterMe(theBC);
   pmanager->AddDiscreteProcess(&theIPtriton);
	}
	
	if( particleName == "alpha" )	
	{
   // alpha
   particle = G4Alpha::Alpha();
   pmanager = particle->GetProcessManager();
   G4LEAlphaInelastic* theAIModel = new G4LEAlphaInelastic;
   theAIModel->SetMaxEnergy(100*MeV);
   theIPalpha.AddDataSet(TripathiCrossSection);
   theIPalpha.AddDataSet(aShen);
   theIPalpha.RegisterMe(theAIModel);
   theIPalpha.RegisterMe(theBC);
   pmanager->AddDiscreteProcess(&theIPalpha);
	}
	
	if( particleName == "GenericIon" )	
	{
   // GenericIon
   particle = G4GenericIon::GenericIon();
   pmanager = particle->GetProcessManager();
   G4HadronInelasticProcess* theIPGenericIon =
   new G4HadronInelasticProcess("IonInelastic",particle);
   theIPGenericIon->AddDataSet(TripathiCrossSection);
   theIPGenericIon->AddDataSet(aShen);
   G4BinaryLightIonReaction * theGenIonBC= new G4BinaryLightIonReaction;
   theGenIonBC->SetMinEnergy(0*MeV);
   theGenIonBC->SetMaxEnergy(10*GeV);
   theIPGenericIon->RegisterMe(theGenIonBC);
   pmanager->AddDiscreteProcess(theIPGenericIon);
	}
	
   }
}


void PhysicsList::ConstructOp()
{ 
//////////////////////////////////////////////////////////////////////////////////////////////////
/// This is stuff from the VANDLE code. I don't know if this will work or not. 
///////  There are more examples of this in /N06
///////////////////////////////////////////////////////////////////////////////////////////////// 
  // optical processes
  G4Scintillation* theScintillation = new G4Scintillation("Scintillation");
  G4Cerenkov*  theCerenkovProcess = new G4Cerenkov("Cerenkov");
  G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();
  G4OpRayleigh* theRayleighScatteringProcess = new G4OpRayleigh();
  G4OpBoundaryProcess* theBoundaryProcess = new G4OpBoundaryProcess();

  theAbsorptionProcess->SetVerboseLevel(OpVerbLevel);
  theRayleighScatteringProcess->SetVerboseLevel(OpVerbLevel);
  theBoundaryProcess->SetVerboseLevel(OpVerbLevel);
  theCerenkovProcess->SetVerboseLevel(OpVerbLevel);
  theScintillation->SetVerboseLevel(OpVerbLevel);
  
  theCerenkovProcess->SetMaxNumPhotonsPerStep(20);
  theCerenkovProcess->SetMaxBetaChangePerStep(10.0);
  theCerenkovProcess->SetTrackSecondariesFirst(true);
  
  theScintillation->SetScintillationYieldFactor(1.0);
  //theScintillation->SetTrackSecondariesFirst(true);

  G4OpticalSurfaceModel themodel = unified;
  //theBoundaryProcess->SetModel(themodel);

  theParticleIterator->reset();
  
  while( (*theParticleIterator)() )
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (theCerenkovProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(theCerenkovProcess);
      pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
    }
    if (theScintillation->IsApplicable(*particle)){
    pmanager->AddProcess(theScintillation);
    pmanager->SetProcessOrderingToLast(theScintillation,idxAtRest);
    pmanager->SetProcessOrderingToLast(theScintillation,idxPostStep);
    
    }
    
    if (particleName == "opticalphoton")
	{
		pmanager->AddDiscreteProcess(theAbsorptionProcess);
		pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
		pmanager->AddDiscreteProcess(theBoundaryProcess);
    }
  }	
}  



void PhysicsList::ConstructGeneral()
 {

// Add Decay Process
   G4Decay* theDecayProcess = new G4Decay();

   theParticleIterator -> reset();

   while((*theParticleIterator)())
    {

      G4ParticleDefinition* particle = theParticleIterator -> value();
      G4ProcessManager* pmanager = particle -> GetProcessManager();
      if (theDecayProcess -> IsApplicable(*particle))
       {

         pmanager -> AddProcess(theDecayProcess);

      // Set ordering for PostStepDoIt and AtRestDoIt
         pmanager -> SetProcessOrdering(theDecayProcess, idxPostStep);
         pmanager -> SetProcessOrdering(theDecayProcess, idxAtRest);

       }    

    }

 }


void PhysicsList::SetCuts()   /// See example N06 for how to set all default values
 {

   if (verboseLevel > 0)
    {

      G4cout << "PhysicsList::SetCuts:CutLength : " << G4BestUnit(defaultCutValue, "Length") << G4endl;
      DumpCutValuesTable();

    }
   
   SetCutValue(defaultCutValue, "gamma");
   SetCutValue(defaultCutValue, "e-");
   SetCutValue(defaultCutValue, "e+");
   SetCutValue(defaultCutValue, "neutron");
   SetCutValue(defaultCutValue, "proton");
   SetCutValue(defaultCutValue, "opticalphoton");
  
 }



