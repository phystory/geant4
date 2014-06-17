// --------------------------------------------------------------
//      GEANT 4 - practice01, main file = mydetsim.cc
// --------------------------------------------------------------

#ifndef MyDetectorConstruction_H
#define MyDetectorConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;

#include "G4VUserDetectorConstruction.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    MyDetectorConstruction();
    ~MyDetectorConstruction();

    G4VPhysicalVolume* Construct();

  private:
    
    // Logical volumes
    //
    G4LogicalVolume* experimentalHall_log;
    G4LogicalVolume* tracker_log;

    // Physical volumes
    //
    G4VPhysicalVolume* experimentalHall_phys;
    G4VPhysicalVolume* tracker_phys;
};

#endif

