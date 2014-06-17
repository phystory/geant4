#ifndef treeClass_He612C4He14C_H
#define treeClass_He612C4He14C_H 1

//--- create a structure to pass data from the primary generator to the eventaction function.
struct DataPass
{
	public:
		G4int		Reaction;
		G4double	LightEnergy;
		G4double LightTheta;
		G4double LightPhi;
		G4double LightX;
		G4double LightY;
		G4double LightZ;
};

#endif
