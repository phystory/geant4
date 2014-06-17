#ifndef treeClass_CSM_H
#define treeClass_CSM_H 1
   const G4int        MaxArraySize = 1000;
using namespace std;
class treeClass
{

   public:
   G4int	      Hits;
   G4int              NumofDets;
   G4int              EventID;
   G4double           KineticEner;
   G4int              DminNum;
   G4double           DetLength; 
   G4int              DMinNumDist;
   G4int              NumOfParticles;
	char					 ParticleName[128];
	char					 VolumeName[128];	

   G4int              DetID;
   G4double           TimeofFlight;
   G4double           Neutx;
   G4double           Neuty;
   G4double           Neutz;
   G4double           Edeposited;
   G4double           Eloss;

   G4double           DetX;
   G4double           DetY;
   G4double           DetZ;
};

#endif
