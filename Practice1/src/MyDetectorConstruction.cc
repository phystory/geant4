// --------------------------------------------------------------
//      GEANT 4 - practice01, main file = mydetsim.cc
// --------------------------------------------------------------

#include "MyDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

MyDetectorConstruction::MyDetectorConstruction()
 :  experimentalHall_log(0), tracker_log(0),
    experimentalHall_phys(0), tracker_phys(0)
{;}

MyDetectorConstruction::~MyDetectorConstruction()
{
}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{

  //------------------------------------------------------ materials

  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;

  G4Material* Ar = 
  new G4Material("ArgonGas", z= 18., a= 39.95*g/mole, density= 1.782*mg/cm3);

  G4Material* Al = 
  new G4Material("Aluminum", z= 13., a= 26.98*g/mole, density= 2.7*g/cm3);

  //G4Material* Pb = 
  //new G4Material("Lead", z= 82., a= 207.19*g/mole, density= 11.35*g/cm3);

  //------------------------------------------------------ volumes

  //------------------------------ experimental hall (world volume)
  //------------------------------ beam line along x axis

  G4double expHall_x = 3.0*m;
  G4double expHall_y = 1.0*m;
  G4double expHall_z = 1.0*m;
  G4Box* experimentalHall_box
    = new G4Box("expHall_box",expHall_x,expHall_y,expHall_z);
  experimentalHall_log = new G4LogicalVolume(experimentalHall_box,
                                             Ar,"expHall_log",0,0,0);
  experimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(),
                                      experimentalHall_log,"expHall",0,false,0);

  //------------------------------ a tracker tube

  G4double innerRadiusOfTheTube = 0.*cm;
  G4double outerRadiusOfTheTube = 60.*cm;
  G4double hightOfTheTube = 50.*cm;
  G4double startAngleOfTheTube = 0.*deg;
  G4double spanningAngleOfTheTube = 360.*deg;
  G4Tubs* tracker_tube = new G4Tubs("tracker_tube",innerRadiusOfTheTube,
                                    outerRadiusOfTheTube,hightOfTheTube,
                                    startAngleOfTheTube,spanningAngleOfTheTube);
  tracker_log = new G4LogicalVolume(tracker_tube,Al,"tracker_log",0,0,0);
  G4double trackerPos_x = -1.0*m;
  G4double trackerPos_y = 0.*m;
  G4double trackerPos_z = 0.*m;
  tracker_phys = new G4PVPlacement(0,
             G4ThreeVector(trackerPos_x,trackerPos_y,trackerPos_z),
             tracker_log,"tracker",experimentalHall_log,false,0);

  //------------------------------------------------------------------

  return experimentalHall_phys;
}

