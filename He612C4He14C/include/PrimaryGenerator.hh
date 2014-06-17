#ifndef PrimaryGenerator_H
#define PrimaryGenerator_H 1

#include "globals.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "TRandom3.h"
#include "DataPass.hh"

#include <iostream>
using namespace std;

class G4Event;
class G4ParticleGun;
class PrimaryGenerator:public G4VUserPrimaryGeneratorAction
 {

   public:
      PrimaryGenerator(struct DataPass *LightInfoPass);
      virtual ~PrimaryGenerator();
      void GeneratePrimaries(G4Event* event);
      static G4ParticleGun* Gun()
       {

         return particleGun;

       }

		//-- custom structure to pass the Alpha data from the generator to eventaction
		G4int		*mReaction;
		G4double *mLightEnergy;
		G4double *mLightTheta;
		G4double *mLightPhi;		

   private:
   	
      static G4ParticleGun* particleGun;
      G4int	count;

      G4int 	   ParticleNum;
		G4double		m1, m2, m3, m4;
		
		G4double		*mCalcLightProdKinematics();
		G4double		*mCalcLightProdKinematicsp6He4Het();		
 };

#endif



