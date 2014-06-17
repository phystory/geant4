#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

#include "G4Box.hh"
#include "G4Tet.hh"
#include "G4Tubs.hh"
#include "globals.hh"
#include "G4Colour.hh"
#include "G4Polyhedra.hh"
#include "G4UnionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVParameterised.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4CylindricalSurface.hh"
#include "G4SDManager.hh"

#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "Material.hh"


class G4Material;
class G4LogicalVolume;
class G4VPhysicalVolume;
class DetectorConstruction:public G4VUserDetectorConstruction
 {

   public:
      DetectorConstruction();
      virtual ~DetectorConstruction();
      virtual G4VPhysicalVolume* Construct();

   private:
      void ExperimentalHall();
      void Wheel(G4double edgelength);
      void Ring(G4double edgelength);
      void Block(G4double edgelength);
      void SiDet();

      G4double DToR(G4double angleInDegree);
      G4double RToD(G4double angleInRadian);

      G4int 	          mNumofDets;
      G4double            x;
      G4double            y;
      G4double            z;
      G4double            pi; 
     

      G4int materialOfDetector;

      G4LogicalVolume* expHallLogical;
      G4VPhysicalVolume* expHallPhysical;

		//--- The logical volumes for the CD's
		G4LogicalVolume* InBoardDiCDDetLogical;		
		G4LogicalVolume* OutBoardDiCDDetLogical;		
		G4LogicalVolume* SideBoardDiCDDetLogical;								
		G4LogicalVolume* DiCDDetLogical;
		G4LogicalVolume* DiCDDetDeadLogical;		
		G4LogicalVolume* DoCDDetLogical;		
		G4LogicalVolume* DoCDDetDeadLogical;				
		
		//--- The logical volumes for the Boxs
      G4LogicalVolume* DiBoxDetLogical;		
      G4LogicalVolume* DiBoxDetDeadLogical;
      G4LogicalVolume* DiBoxFPCBLogical;     
      G4LogicalVolume* DiBoxBPCBLogical;      
      G4LogicalVolume* DiBoxSPCBLogical;          
      G4LogicalVolume* DoBoxDetLogical;
      G4LogicalVolume* DoBoxDetDeadLogical;      
      G4LogicalVolume* UmBoxDetLogical;
      G4LogicalVolume* UmBoxDetDeadLogical;
      
		//--- The logical volumes for the UmCD's
		G4LogicalVolume* UmCDDetLogical;
		G4LogicalVolume* UmCDDetDeadLogical;	       
         
      G4LogicalVolume* TargetLogical;      

      Material* materials;

 };

#endif



