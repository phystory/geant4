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
// $Id: ExN01DetectorConstruction.cc,v 1.9 2006/06/29 17:47:19 gunter Exp $
// GEANT4 tag $Name: geant4-08-01-patch-01 $
//
// Detector Construction File for n-Detector at p,gamma-Line at Notre Dame
// by Sascha Falahat (sfalahat@nd.edu), 13.04.2007
//
//
//
//
//


#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4UserLimits.hh"
#include "G4PVParameterised.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"

#include "HeTube01DetectorConstruction.hh"
#include "HeTube01Parametrisation.hh"
#include "AlTubeParametrisation.hh"

#include "MyConfig.hh"

extern MyConfig *ConfigDet;

HeTube01DetectorConstruction::HeTube01DetectorConstruction()
 :  TargetHall_log(0), HeTube_log(0), AlTube_log(0), ParaffinBlock_log(0), ParaffinBlock_with_holes_log(0), BeamPipeOuter_log(0), BeamPipeInner_log(0)
{;}

HeTube01DetectorConstruction::~HeTube01DetectorConstruction()
{
}

G4VPhysicalVolume* HeTube01DetectorConstruction::Construct()
{
#include "NeutronDetectorParameterDef.icc"
G4int numberTubes=ConfigDet->TotalNumberofTube();
  

  //------------------------materials that will be used for the setup

  	// material parameters

  	//G4double a;  // atomic mass
  	//G4double z;  // atomic number
  	

 	//environment
 	G4NistManager* man = G4NistManager::Instance();

 	// Vacuum for (Inner) Beam Pipe

	G4Material* Vac    = man->FindOrBuildMaterial("G4_Galactic");

	G4Material* Air  = man->FindOrBuildMaterial("G4_AIR");

	// Aluminum for the 3He-Tubes and Beam Pipe
  	G4Material* Al = man->FindOrBuildMaterial("G4_Al"); 

	//thermalization of neutrons requires Paraffin
 	//G4Material* Par = man->FindOrBuildMaterial("G4_PARAFFIN");  


       //thermalization of neutrons requires Polyethylene
        G4Material* Poly = man->FindOrBuildMaterial("G4_POLYETHYLENE");

 
	//this is the 3He at 10atm in the 3He-Tubes


        G4int protons=2, neutrons=1, nucleons=protons+neutrons;
        G4double atomicMass = 3.016*g/mole;

        G4Isotope* He3 = new G4Isotope("HE3", protons, nucleons, atomicMass);
        G4Element* HeThree  = new G4Element("Helium 3","He", 1);
        HeThree->AddIsotope(He3,  100.*perCent);

	G4double pressure = 10.1325*bar;
        G4double temperature = 294.15*kelvin;
        G4double molar_constant = Avogadro*k_Boltzmann;
        G4double density = (atomicMass*pressure)/(temperature*molar_constant);
     
        G4Material* He = new G4Material("HeliumThree", density, 1, kStateGas, temperature, pressure);
        He->AddElement(HeThree, 100*perCent);


/*
//old definition of 3He

  	G4Isotope* He3 = new G4Isotope("HE3", 2, 3, 3.016*g/mole, gas, pressure = 10.0*atm);
  	G4Element* HeThree  = new G4Element("Helium 3","He",1);
  	HeThree->AddIsotope(He3,  100.*perCent);
  	G4Material* He = new G4Material("HeliumThree", 3.016*g/mole, 1);
  	He->AddElement(HeThree, 1);
*/
  //------------------------detector components/volumes------------------------
  	//------------------------------ experimental hall (world volume)
  	//------------------------------ beam line along x axis

	G4Box* TargetHall_box
    		= new G4Box("TargetHall_box",TargetHall_x,TargetHall_y,TargetHall_z);

  	TargetHall_log = new G4LogicalVolume(TargetHall_box,
                                             Air,"TargetHall_log",0,0,0);

  	TargetHall_phys = new G4PVPlacement(0,G4ThreeVector(),
                                      TargetHall_log,"TargetHall",0,false,0);

		//------------------------------ VisAttributes for Target Hall

		G4VisAttributes* TargetHall_logVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
   			TargetHall_logVisAtt->SetForceWireframe(true);
 			TargetHall_log->SetVisAttributes(TargetHall_logVisAtt);


	//------------------------------ Paraffin Block (without Tubes in it)
  	
 	G4VSolid* ParaffinBlock_box
    		= new G4Box("ParaffinBlock_box",ParaffinBlock_x,ParaffinBlock_y,ParaffinBlock_z);

  	ParaffinBlock_log = new G4LogicalVolume(ParaffinBlock_box,
                                             Poly,"ParaffinBlock_log",0,0,0);

	ParaffinBlock_phys = new G4PVPlacement(0,G4ThreeVector(),
                                    ParaffinBlock_log,"ParaffinBlock",TargetHall_log,false,0);

		//------------------------------ VisAttributes for Paraffin Block

		G4VisAttributes* ParaffinBlock_logVisAtt = new G4VisAttributes(G4Colour(1.0,1.,0.0));
			ParaffinBlock_logVisAtt->SetForceWireframe(true);
   			ParaffinBlock_log->SetVisAttributes(ParaffinBlock_logVisAtt);


	//------------------------------ Outer Beam Pipe in center of Paraffin Block

 	G4VSolid* BeamPipeOuter_Tube
    		= new G4Tubs("BeamPipeOuter_Tubes",innerRadiusOfBeamPipeOuter,
                                    outerRadiusOfBeamPipeOuter,heightOfBeamPipeOuter,
                                    startAngleOfBeamPipeOuter,spanningAngleOfBeamPipeOuter);

  	BeamPipeOuter_log = new G4LogicalVolume(BeamPipeOuter_Tube,
                                             Al,"BeamPipeOuter_log",0,0,0);

	BeamPipeOuter_phys = new G4PVPlacement(0,G4ThreeVector(0,0,-1*ParaffinBlock_z/2),
                                    BeamPipeOuter_log,"BeamPipeOuter",TargetHall_log,false,0);

		//------------------------------ VisAttributes for Paraffin Block

		G4VisAttributes* BeamPipeOuter_logVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
			BeamPipeOuter_logVisAtt->SetForceWireframe(true);
   			BeamPipeOuter_log->SetVisAttributes(BeamPipeOuter_logVisAtt);

	//------------------------------ Inner Beam Pipe in center of Paraffin Block

	G4VSolid* BeamPipeInner_Tube
    		= new G4Tubs("BeamPipeInner_Tubes",innerRadiusOfBeamPipeInner,
                                    outerRadiusOfBeamPipeInner,heightOfBeamPipeInner,
                                    startAngleOfBeamPipeInner,spanningAngleOfBeamPipeInner);
  	BeamPipeInner_log = new G4LogicalVolume(BeamPipeInner_Tube,
                                             Vac,"BeamPipeInner_log",0,0,0);

	BeamPipeInner_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),
                                    BeamPipeInner_log,"BeamPipeInner",BeamPipeOuter_log,false,0);

		//------------------------------ VisAttributes for Paraffin Block

		G4VisAttributes* BeamPipeInner_logVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
			BeamPipeInner_logVisAtt->SetForceWireframe(true);
   			BeamPipeInner_log->SetVisAttributes(BeamPipeOuter_logVisAtt);

	
/* from the User Guide, section 4.1.4.2 : Currently for many cases it is not possible to add daughter volumes to a parameterised volume. Only parameterised volumes all of whose solids have the same size are allowed to contain daughter volumes. When the size or type of solid varies, adding daughters is not supported.

So the full power of parameterised volumes can be used only for "leaf" volumes, which contain no other volumes. 

That's why we can't use the Al-Tubes right now.
nrametrisation*/


//------------------------------ (Parametrised) Al-Tubes surrounding the 3He-Tubes
/*
	G4VSolid * AlTubes_Param_tubs
    		= new G4Tubs("AlTubes_Param_tubs",innerRadiusOfAlTube,
                                    outerRadiusOfAlTube,heightOfAlTube,
                                    startAngleOfAlTube,spanningAngleOfAlTube);

  	G4LogicalVolume * AlTubes_Param_log
    		= new G4LogicalVolume(AlTubes_Param_tubs,Al,"AlTube",0,0,0);

  	G4VPVParameterisation * AlTubes_Param
    		= new AlTubeParametrisation;
      			new G4PVParameterised("AlTubes_Param_phys",AlTubes_Param_log,ParaffinBlock_log,
			   kZAxis, numberTubes, AlTubes_Param);

		//------------------------------ VisAttributes for 3He-Tubes

	G4VisAttributes* AlTubes_Param_logVisAtt
    		= new G4VisAttributes(G4Colour(0.5,0.0,1.0));
  		AlTubes_Param_logVisAtt->SetForceWireframe(true);
  		AlTubes_Param_log->SetVisAttributes(AlTubes_Param_logVisAtt);


*/



//------------------------------ (Parametrised) 3He-Tubes

	G4VSolid * HeTubes_Param_tubs
    		= new G4Tubs("HeTubes_Param_tubs",innerRadiusOfHeTube,
                                    outerRadiusOfHeTube,heightOfHeTube,
                                    startAngleOfHeTube,spanningAngleOfHeTube);

  	G4LogicalVolume * HeTubes_Param_log
    		= new G4LogicalVolume(HeTubes_Param_tubs,He,"3HeTube",0,0,0);

  	G4VPVParameterisation * HeTubes_Param
    		= new HeTube01Parametrisation;
  			// dummy value : kXAxis -- modified by parameterised volume
  			// G4VPhysicalVolume *trackerLayer_phys =
      			new G4PVParameterised("HeTubes_Param_phys",HeTubes_Param_log,ParaffinBlock_log,
			   kZAxis, numberTubes, HeTubes_Param);
      			//new G4PVParameterised("HeTubes_Param_phys",HeTubes_Param_log,AlTubes_Param_log,
			//   kZAxis, numberTubes, HeTubes_Param);

		//------------------------------ VisAttributes for 3He-Tubes

	G4VisAttributes* HeTubes_Param_logVisAtt
    		= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  		HeTubes_Param_logVisAtt->SetForceWireframe(true);
  		HeTubes_Param_log->SetVisAttributes(HeTubes_Param_logVisAtt);




	//------------------------------ In order to create a Paraffin Block with 3He-Tubes in it, we need to //------------------------------ substract both components
  	
	/*G4SubtractionSolid* subtraction =
    		new G4SubtractionSolid("ParaffinBlock_with_holes", ParaffinBlock_box, HeTubes_Param_tubs);


	G4LogicalVolume * ParaffinBlock_with_holes_log
    		= new G4LogicalVolume(subtraction,Par,"ParaffinBlock_with_holes",0,0,0);

	ParaffinBlock_with_holes_phys = new G4PVPlacement(0,G4ThreeVector(),
                                   ParaffinBlock_with_holes_log,"ParaffinBlock_subtraction",TargetHall_log,false,0);


 
		//------------------------------ VisAttributes for Paraffin-Block with 3He-Tubes

	G4VisAttributes* ParaffinBlock_with_holes_logVisAtt = new G4VisAttributes(G4Colour(1.0,0.5,0.0));
   		ParaffinBlock_with_holes_logVisAtt->SetForceWireframe(true);
   		ParaffinBlock_with_holes_log->SetVisAttributes(ParaffinBlock_with_holes_logVisAtt);

	//------------------------------ Setting of the VisAttributes for each component

  	*/

 /*

  G4VisAttributes* AlTube_logVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
   AlTube_logVisAtt->SetForceWireframe(true);
   AlTube_log->SetVisAttributes(AlTube_logVisAtt);

*/




  return TargetHall_phys;
}

