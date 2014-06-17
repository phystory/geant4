#include "DetectorConstruction.hh"
#include "SiliconSD.hh"

using namespace std;
#include <math.h>
#include <vector>
#include <cstdio>

DetectorConstruction::DetectorConstruction()
    :expHallLogical(0),expHallPhysical(0)
{
   pi = 4*atan(1);

   materials = new Material();
   
   materialOfDetector = 1;
}


DetectorConstruction::~DetectorConstruction()
{}


G4VPhysicalVolume* DetectorConstruction::Construct()
{

   G4double edgelength = 6.4*cm;
   materials -> DefineMaterials();

   ExperimentalHall();

   if (0)
    {

      Wheel(edgelength);

    }
   else if (0)
    {

      Ring(edgelength);

    }
   else if (0)
    {

      Block(edgelength);

    }
   else
    {

      SiDet();

    }

   return expHallPhysical;  

}

void DetectorConstruction::ExperimentalHall()
{

   G4double expHallSide = 2.0*m;
   G4ThreeVector ExpHallPos;
   ExpHallPos.set(0.0,0.0,0.0);

   G4Box* expHallSolid = new G4Box("Experimental Hall Solid", expHallSide, expHallSide, expHallSide);
   expHallLogical = new G4LogicalVolume(expHallSolid, materials -> Vacuum, "Experimental Hall Logical");
   expHallPhysical = new G4PVPlacement(0, ExpHallPos, expHallLogical, "Experimental Hall", 0, false, 0);

   expHallLogical -> SetVisAttributes(G4VisAttributes::Invisible);

}

void DetectorConstruction::SiDet()
{
	G4SDManager *SDman = G4SDManager::GetSDMpointer();
	G4String    SiliconSDname = "/SiDet/Silicon"; 
	SiliconSD *siliconSD = new SiliconSD( SiliconSDname );
	SDman->AddNewDetector( siliconSD );
	
   G4ThreeVector   position;
	
   G4VisAttributes* DetAttributes = new G4VisAttributes(G4Colour::White());
   DetAttributes->SetForceSolid(true);	
   
   G4VisAttributes* BoardAttributes = new G4VisAttributes(G4Colour::Green());
   BoardAttributes->SetForceSolid(true);	    
   
  	G4VisAttributes* DeadDetAttributes = new G4VisAttributes(G4Colour::White());
   DeadDetAttributes->SetForceSolid(true);   
	
	//--------------- DiCD detector
	for(int i = 0; i < 4; i++)
	{
		//--- the cylinder parameters for the CDs, thickness is the total thickness while the input paramter takes half z-thickness
		G4double DiCD_InnerRadius = 9.0*mm;
		G4double DiCD_OuterRadius = 41.0*mm;
		G4double DiCD_DeadThick = 0.7*um;
		G4double DiCD_Thickness = 80*um - 2*DiCD_DeadThick;
		//--- the second CD Detector is 40um instead of 80um
		if( i == 1 )
			DiCD_Thickness = 0.040*mm - 2*DiCD_DeadThick;
			
		//--- the parameters for the board 
		G4double InnerBoard_IR = 7.5*mm;
		G4double InnerBoard_OR = 9.0*mm;
		G4double OuterBoard_IR = 41.00*mm;
		G4double OuterBoard_OR = 50.0*mm;	
		G4double SideBoard_IR = 7.5*mm;
		G4double SideBoard_OR = 50*mm;	
		G4double BoardThick = 2.*mm;
		
		G4double DiCD_StartAngle = 6.4*pi/180. + i*(pi/2.);
		G4double DiCD_SpanAngle = 3.4*24.*pi/180.;
		
		G4double Board_StartAngle = ( 6.4*pi/180. + (i)*(pi/2.) + DiCD_SpanAngle ) - pi/2;
		G4double Board_SpanAngle = pi/2. - DiCD_SpanAngle;
		
		//--- Declare the cylinder for the detectors and give them a logical volume
		char buffer1[64];
		sprintf( buffer1, "Si_Di_CD%d", i+1);
		G4Tubs* DiCDDet = new G4Tubs(buffer1, DiCD_InnerRadius, DiCD_OuterRadius, DiCD_Thickness/2, DiCD_StartAngle, DiCD_SpanAngle);
		DiCDDetLogical = new G4LogicalVolume( DiCDDet, materials -> Silicon, buffer1 );		
		
		DiCDDetLogical->SetSensitiveDetector( siliconSD );
		
		//--- Decalre the boards for the detectors and give them logical		
		G4Tubs* InBoardDiCDDet = new G4Tubs("Board", InnerBoard_IR, InnerBoard_OR, BoardThick/2, DiCD_StartAngle, DiCD_SpanAngle);
		InBoardDiCDDetLogical = new G4LogicalVolume( InBoardDiCDDet, materials -> Board, "Board" );	
		G4Tubs* OutBoardDiCDDet = new G4Tubs("Board", OuterBoard_IR, OuterBoard_OR, BoardThick/2, DiCD_StartAngle, DiCD_SpanAngle);
		OutBoardDiCDDetLogical = new G4LogicalVolume( OutBoardDiCDDet, materials -> Board, "Board" );			
		G4Tubs* SideBoardDiCDDet = new G4Tubs("Board", SideBoard_IR, SideBoard_OR, BoardThick/2, Board_StartAngle, Board_SpanAngle);
		SideBoardDiCDDetLogical = new G4LogicalVolume( SideBoardDiCDDet, materials -> Board, "Board" );					
				
		//--- Set the dead layer logical volume
		char buffer2[64];
		sprintf( buffer2, "Si_DiDead1_CD%d", i+1);
		G4Tubs* DiCDDetDead = new G4Tubs(buffer2, DiCD_InnerRadius, DiCD_OuterRadius, DiCD_DeadThick/2, DiCD_StartAngle, DiCD_SpanAngle);
		DiCDDetDeadLogical = new G4LogicalVolume( DiCDDetDead, materials -> Silicon, buffer1 );			
		
		G4double DiCDPlaceX = 0.0*mm;
		G4double DiCDPlaceY = 0.0*mm;
		G4double DiCDPlaceZ = 78.2*mm + DiCD_Thickness/2.0;	
		G4ThreeVector *DiCDDetPos = new G4ThreeVector();	
		DiCDDetPos->set( DiCDPlaceX, DiCDPlaceY, DiCDPlaceZ );
		
		//--- Place the first dead layer in front of the detector
		G4double DiCDDead1PlaceX = 0.0*mm;
		G4double DiCDDead1PlaceY = 0.0*mm;
		G4double DiCDDead1PlaceZ = 78.2*mm - DiCD_Thickness/2.0 - DiCD_DeadThick/2.0;	
		G4ThreeVector *DiCDDetPosDead1 = new G4ThreeVector();	
		DiCDDetPosDead1->set( DiCDDead1PlaceX, DiCDDead1PlaceY, DiCDDead1PlaceZ );
		
		//--- Place the second dead layer behind the detector
		G4double DiCDDead2PlaceX = 0.0*mm;
		G4double DiCDDead2PlaceY = 0.0*mm;
		G4double DiCDDead2PlaceZ = 78.2*mm + DiCD_Thickness + DiCD_DeadThick/2.0;	
		G4ThreeVector *DiCDDetPosDead2 = new G4ThreeVector();	
		DiCDDetPosDead2->set( DiCDDead2PlaceX, DiCDDead2PlaceY, DiCDDead2PlaceZ );			
		
		G4PVPlacement *DiCDDetectors;
		char buffer3[64];
		sprintf( buffer3, "DiCD%d", i+1 );
		DiCDDetectors = new G4PVPlacement( 0, *DiCDDetPos, DiCDDetLogical, buffer3, expHallLogical, false, i );	
	
	   DiCDDetLogical->SetVisAttributes(DetAttributes);			
	   
	   //--- Place the board in the volume
		G4PVPlacement *InBoardDiCDDetectors;
		InBoardDiCDDetectors = new G4PVPlacement( 0, *DiCDDetPos, InBoardDiCDDetLogical, "board", expHallLogical, false, i );	
	
	   InBoardDiCDDetLogical->SetVisAttributes(BoardAttributes);		  
	    
		G4PVPlacement *OutBoardDiCDDetectors;
		OutBoardDiCDDetectors = new G4PVPlacement( 0, *DiCDDetPos, OutBoardDiCDDetLogical, "board", expHallLogical, false, i );	
	
	   OutBoardDiCDDetLogical->SetVisAttributes(BoardAttributes);		   
	   
		G4PVPlacement *SideBoardDiCDDetectors;
		SideBoardDiCDDetectors = new G4PVPlacement( 0, *DiCDDetPos, SideBoardDiCDDetLogical, "board", expHallLogical, false, i );	
	
	   SideBoardDiCDDetLogical->SetVisAttributes(BoardAttributes);		   
	   
	   G4int setCopyNum1 = 0;
	   G4int setCopyNum2 = 1;
	   
	   if( i == 1)
	   {
	   	setCopyNum1 = 2;
	   	setCopyNum2 = 3;	   	
	   }	
	   if( i == 2)
	   {
	   	setCopyNum1 = 4;
	   	setCopyNum2 = 5;	   	
	   }		      
	   if( i == 3)
	   {
	   	setCopyNum1 = 6;
	   	setCopyNum2 = 7;	   	
	   }		   
	   
		G4PVPlacement *DiCDDetectorsDead1, *DiCDDetectorsDead2;
		char buffer4[64];
		sprintf( buffer4, "DiDead1CD%d", i+1 );
		DiCDDetectorsDead1 = new G4PVPlacement( 0, *DiCDDetPosDead1, DiCDDetDeadLogical, buffer4, expHallLogical, false, setCopyNum1 );		   
	
		char buffer5[64];
		sprintf( buffer5, "DiDead2CD%d", i+1 );
		DiCDDetectorsDead2 = new G4PVPlacement( 0, *DiCDDetPosDead2, DiCDDetDeadLogical, buffer5, expHallLogical, false, setCopyNum2 );	
		
		DiCDDetDeadLogical->SetVisAttributes(DeadDetAttributes);
	}	
	//--------------- End DiCD detectors
	
	//--------------- DoCD detector	
	for(int i = 0; i < 4; i++)
	{
		//--- the cylinder parameters for the CDs, thickness is the total thickness while the input paramter takes half z-thickness	
		G4double DoCD_InnerRadius = 9.0*mm;
		G4double DoCD_OuterRadius = 50.0*mm;
		G4double DoCD_DeadThick = 0.7*um;		
		G4double DoCD_Thickness = 1.0*mm - 2*DoCD_DeadThick;
		G4double DoCD_StartAngle = 6.4*pi/180. + i*(pi/2.);
		G4double DoCD_EndAngle = 3.4*24.*pi/180.;		
		
		G4Tubs* DoCDDet = new G4Tubs("Si_Do_CD", DoCD_InnerRadius, DoCD_OuterRadius, DoCD_Thickness/2, DoCD_StartAngle, DoCD_EndAngle);
		DoCDDetLogical = new G4LogicalVolume( DoCDDet, materials -> Silicon, "Si_Do_CD" );		
		
		DoCDDetLogical->SetSensitiveDetector( siliconSD );		
		
		//--- Set the dead layer logical volume
		char buffer1[64];
		sprintf( buffer1, "Si_DoDead1_CD%d", i+1);
		G4Tubs* DoCDDetDead = new G4Tubs(buffer1, DoCD_InnerRadius, DoCD_OuterRadius, DoCD_DeadThick/2, DoCD_StartAngle, DoCD_EndAngle);
		DoCDDetDeadLogical = new G4LogicalVolume( DoCDDetDead, materials -> Silicon, buffer1 );			
		
		G4double DoCDPlaceX = 0.0*mm;
		G4double DoCDPlaceY = 0.0*mm;
		G4double DoCDPlaceZ = 80.5*mm + DoCD_Thickness/2.0;	
		G4ThreeVector *DoCDDetPos = new G4ThreeVector();
		DoCDDetPos->set( DoCDPlaceX, DoCDPlaceY, DoCDPlaceZ );
		
		G4double DoCDDeadPlaceX = 0.0*mm;
		G4double DoCDDeadPlaceY = 0.0*mm;
		G4double DoCDDeadPlaceZ = 80.5*mm - DoCD_Thickness/2.0 - DoCD_DeadThick/2.0;	
		G4ThreeVector *DoCDDetPosDead = new G4ThreeVector();
		DoCDDetPosDead->set( DoCDDeadPlaceX, DoCDDeadPlaceY, DoCDDeadPlaceZ );		
		
		G4PVPlacement *DoCDDetectors;
		char buffer2[64];
		sprintf( buffer2, "DoCD%d", i+1 );
		DoCDDetectors = new G4PVPlacement( 0, *DoCDDetPos, DoCDDetLogical, buffer2, expHallLogical, false, i );	
	   DoCDDetLogical->SetVisAttributes(DetAttributes);		
	   
		G4PVPlacement *DoCDDeadDetectors;
		char buffer3[64];
		sprintf( buffer3, "DoCD%dDead", i+1 );
		DoCDDeadDetectors = new G4PVPlacement( 0, *DoCDDetPosDead, DoCDDetDeadLogical, buffer3, expHallLogical, false, i );	
	   DoCDDetDeadLogical->SetVisAttributes(DeadDetAttributes);		   	
	}
	//--------------- End DoCD detectors	
	
	//--------------- DiBx detectors
	for(int i =0; i < 4; i++)
	{
		//--- refer to drawing number 10330-24-A2NUC
		//--- The extent of the box is from the origin of the world volume to +/- DiBx1_i down the ith axis
		G4double DiBx_X = ( 72.0 )*mm;
		G4double DiBx_Y = ( 48.0 )*mm;
		G4double DiBx_Z = ( 139.8 )*um;	
		G4double DiBx_Z_Dead = ( 0.100 )*um;
		
		//--- the PCB closest to 90 deg detection
		G4double DiBx_FrontPCB_X = 104.*mm;
		G4double DiBx_FrontPCB_Y = 6.89*mm;
		G4double DiBx_FrontPCB_Z = 2.*mm;
		
		//--- PCB closest to 0 deg or the DCD
		G4double DiBx_BackPCB_X = 104.*mm;
		G4double DiBx_BackPCB_Y = 1.05*mm;
		G4double DiBx_BackPCB_Z = 2.*mm;
		
		//--- PCB on the right side if considering DiBx1
		G4double DiBx_SideRPCB_X = 104.;
		G4double DiBx_SideRPCB_Y = 48.*mm;
		G4double DiBx_SideRPCB_Z = 2.*mm;							
		
		char buffer1[64], buffer2[64], buffer3[64];
		sprintf( buffer1, "Si_Di_Bx%d", i+1 );
		sprintf( buffer2, "Bx%dDead", i+1 );
		sprintf( buffer3, "Bx%dPCB",i+1 );
		
		//--- Declare the shape of the volume
   	G4Box* BoxDet = new G4Box(buffer1, DiBx_X/2. , DiBx_Y/2. , DiBx_Z/2. );
   	G4Box* BoxDetDead = new G4Box(buffer2, DiBx_X/2., DiBx_Y/2. , DiBx_Z_Dead/2. );
   	
   	//--- Declare the shape of the PCBs to be put into their logical volumes
  		G4Box* BoxDetPCB_Front = new G4Box( buffer3, DiBx_FrontPCB_X/2., DiBx_FrontPCB_Y/2., DiBx_FrontPCB_Z/2. );
  		G4Box* BoxDetPCB_Back = new G4Box( buffer3, DiBx_BackPCB_X/2., DiBx_BackPCB_Y/2., DiBx_BackPCB_Z/2. );
  		G4Box* BoxDetPCB_Side = new G4Box( buffer3, DiBx_SideRPCB_X/2., DiBx_SideRPCB_Y/2., DiBx_SideRPCB_Z/2. );  		  		
		//--- declare the logical volume with the shape 
	   DiBoxDetLogical = new G4LogicalVolume(BoxDet, materials ->  Silicon, buffer1);
	   DiBoxDetDeadLogical = new G4LogicalVolume(BoxDetDead, materials ->  Silicon, buffer2);
	   
	   //--- logical volume of the PCB
	   DiBoxFPCBLogical = new G4LogicalVolume( BoxDetPCB_Front, materials -> Board, buffer3 );
	   DiBoxBPCBLogical = new G4LogicalVolume( BoxDetPCB_Back, materials -> Board, buffer3 );	   
	   DiBoxSPCBLogical = new G4LogicalVolume( BoxDetPCB_Side, materials -> Board, buffer3 );	   	   
	   
	   //--- Place the Silicon part of the detector as a sensitive detector, or allow event detection of paricles in this area only
		DiBoxDetLogical->SetSensitiveDetector( siliconSD );		   
	   
		//--- place the logical volume in the world, the center of the logical volume is placed in the G4ThreeVector location
		G4double DiBoxPlaceX, DiBoxPlaceY, DiBoxPlaceZ; 
		G4double DiBoxDeadX1, DiBoxDeadY1, DiBoxDeadZ1;
		G4double DiBoxDeadX2, DiBoxDeadY2, DiBoxDeadZ2;
		
		//--- F-Front, B- Back, S-Side pcb geometric placement
		G4double DiBoxPCBFX, DiBoxPCBFY, DiBoxPCBFZ;		
		G4double DiBoxPCBBX, DiBoxPCBBY, DiBoxPCBBZ;				
		G4double DiBoxPCBSX, DiBoxPCBSY, DiBoxPCBSZ;				
		
		G4double DiBxXOffSet = 40.8*mm;
		G4double DiBxYoffSet = 40.8*mm;		
		G4double DiBxZoffSet = 15.9*mm;
		//--- This is the offset from the center axis
		//--- for example in Bx1 the active area starts at x = 40.8mm but goes to x = 40.8-72 = -31.2mm where 72 is the length of the active area of the front, 24*3mm
		//--- thus there is a slight asymmetry of the detectors the offset is given by 40.8-72/2
		G4double DBxExtraOffSet = 4.8*mm;
		
		if( i == 0 )
		{
			DiBoxPlaceX = DBxExtraOffSet;
			DiBoxPlaceY = DiBxYoffSet + DiBx_Z/2.0;
			DiBoxPlaceZ = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxDeadX1 = DBxExtraOffSet;
			DiBoxDeadY1 = DiBxYoffSet - DiBx_Z_Dead/2.0;
			DiBoxDeadZ1 = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxDeadX2 = DBxExtraOffSet;
			DiBoxDeadY2 = DiBxYoffSet + DiBx_Z + DiBx_Z_Dead/2.0;				
			DiBoxDeadZ2 = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxPCBFX = -DBxExtraOffSet*2;
			DiBoxPCBFY = DiBxYoffSet + DiBx_FrontPCB_Z/2.0;
			DiBoxPCBFZ = DiBxZoffSet - DiBx_FrontPCB_Y/2.;			
		}
		if( i == 1 )
		{
			DiBoxPlaceX = -DiBxXOffSet - DiBx_Z/2.0;
			DiBoxPlaceY = DBxExtraOffSet;
			DiBoxPlaceZ = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxDeadX1 = -DiBxXOffSet + DiBx_Z_Dead/2.0;
			DiBoxDeadY1 = DBxExtraOffSet;
			DiBoxDeadZ1 = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxDeadX2 = -DiBxXOffSet - DiBx_Z - DiBx_Z_Dead/2.0;
			DiBoxDeadY2 = DBxExtraOffSet;				
			DiBoxDeadZ2 = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxPCBFX = -DiBxXOffSet - DiBx_FrontPCB_Z/2.0;
			DiBoxPCBFY = -DBxExtraOffSet*2;
			DiBoxPCBFZ = DiBxZoffSet - DiBx_FrontPCB_Y/2.;
		}		
		if( i == 2 )
		{
			DiBoxPlaceX = -DBxExtraOffSet;
			DiBoxPlaceY = -DiBxYoffSet - DiBx_Z/2.0;
			DiBoxPlaceZ = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxDeadX1 = -DBxExtraOffSet;
			DiBoxDeadY1 = -DiBxYoffSet + DiBx_Z_Dead/2.0;
			DiBoxDeadZ1 = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxDeadX2 = -DBxExtraOffSet;
			DiBoxDeadY2 = -DiBxYoffSet - DiBx_Z - DiBx_Z_Dead/2.0;				
			DiBoxDeadZ2 = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxPCBFX = DBxExtraOffSet*2;
			DiBoxPCBFY = -DiBxYoffSet - DiBx_FrontPCB_Z/2.0;
			DiBoxPCBFZ = DiBxZoffSet - DiBx_FrontPCB_Y/2.;
		}
		if( i == 3 )
		{
			DiBoxPlaceX = DiBxXOffSet + DiBx_Z/2.0;
			DiBoxPlaceY = -DBxExtraOffSet;
			DiBoxPlaceZ = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxDeadX1 = DiBxXOffSet - DiBx_Z_Dead/2.0;
			DiBoxDeadY1 = -DBxExtraOffSet;
			DiBoxDeadZ1 = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxDeadX2 = DiBxXOffSet + DiBx_Z + DiBx_Z_Dead/2.0;
			DiBoxDeadY2 = -DBxExtraOffSet;				
			DiBoxDeadZ2 = DiBxZoffSet + DiBx_Y/2.0;
			
			DiBoxPCBFX = DiBxXOffSet + DiBx_FrontPCB_Z/2.0;
			DiBoxPCBFY = DBxExtraOffSet*2;
			DiBoxPCBFZ = DiBxZoffSet - DiBx_FrontPCB_Y/2.;
		}		

		//--- place the SD detectors
		G4ThreeVector *DiBoxDetPos = new G4ThreeVector();
		DiBoxDetPos->set( DiBoxPlaceX, DiBoxPlaceY, DiBoxPlaceZ );
		//--- place the dead layers
		G4ThreeVector *DiBoxDetPos_Dead1 = new G4ThreeVector();
		DiBoxDetPos_Dead1->set( DiBoxDeadX1, DiBoxDeadY1, DiBoxDeadZ1 );		
		G4ThreeVector *DiBoxDetPos_Dead2 = new G4ThreeVector();
		DiBoxDetPos_Dead2->set( DiBoxDeadX2, DiBoxDeadY2, DiBoxDeadZ2 );
		//--- place the PCB
		G4ThreeVector *DiBoxDetPos_FPCB = new G4ThreeVector();
		DiBoxDetPos_FPCB->set( DiBoxPCBFX, DiBoxPCBFY, DiBoxPCBFZ );
		//DiBoxDetPos_FPCB->set( DiBoxPlaceX, DiBoxPlaceX, DiBoxPlaceX );		
		
		G4RotationMatrix DiBxRM;
		if( i == 0 || i ==2 )
		{
			DiBxRM.rotateX(pi/2);		
		}
		if( i == 1 || i ==3 )
		{
			DiBxRM.rotateY(pi/2);	
			DiBxRM.rotateX(pi/2);
		}			
		
		char buffer4[64];
		sprintf( buffer4, "DiBx%d", i+1);
		
		G4PVPlacement *DiBoxDetectors;
		DiBoxDetectors = new G4PVPlacement( G4Transform3D(DiBxRM,*DiBoxDetPos) , DiBoxDetLogical, buffer4, expHallLogical, false,i );	  
	   DiBoxDetLogical->SetVisAttributes(DetAttributes);
	   
	   G4int setCopyNum1 = 0;
	   G4int setCopyNum2 = 1;
	   
	   if( i == 1)
	   {
	   	setCopyNum1 = 2;
	   	setCopyNum2 = 3;	   	
	   }	
	   if( i == 2)
	   {
	   	setCopyNum1 = 4;
	   	setCopyNum2 = 5;	   	
	   }		      
	   if( i == 3)
	   {
	   	setCopyNum1 = 6;
	   	setCopyNum2 = 7;	   	
	   }		   
	   
	   char buffer5[64];
	   sprintf( buffer5, "Dead1DiBx%d", i+1 );	   
	   
		G4PVPlacement *DiBoxDetectorsDead1;	   
		DiBoxDetectorsDead1 = new G4PVPlacement( G4Transform3D(DiBxRM,*DiBoxDetPos_Dead1) , DiBoxDetDeadLogical, buffer5, expHallLogical, 
		                                         false,setCopyNum1 );
		DiBoxDetDeadLogical->SetVisAttributes(DeadDetAttributes);
	   
	   char buffer6[64];
	   sprintf( buffer6, "Dead2DiBx%d", i+1 );
	   
		G4PVPlacement *DiBoxDetectorsDead2;	   
		DiBoxDetectorsDead2 = new G4PVPlacement( G4Transform3D(DiBxRM,*DiBoxDetPos_Dead2) , DiBoxDetDeadLogical, buffer6, expHallLogical, false,setCopyNum2 );
		DiBoxDetDeadLogical->SetVisAttributes(DetAttributes);	
		
	   char buffer7[64];
	   sprintf( buffer7, "PCBFDiBx%d", i+1 ); 		
	   
		G4PVPlacement *DiBoxPCBFront;	   
		DiBoxPCBFront = new G4PVPlacement( G4Transform3D(DiBxRM,*DiBoxDetPos_FPCB) , DiBoxFPCBLogical, buffer7, expHallLogical, false, i );
		DiBoxFPCBLogical->SetVisAttributes(BoardAttributes);		   
	}
	//--------------- End DiBx Box detectors
	
	//--------------- DoBx detectors
	for(int i =0; i < 4; i++)
	{
		//--- The extent of the box is from the origin of the world volume to +/- DoBx_i down the ith axis
		G4double DoBx_X = ( 72.0 )*mm;
		G4double DoBx_Y = ( 48.0 )*mm;
		G4double DoBx_Z_Dead = ( 1.0 )*um;		
		G4double DoBx_Z = ( 1.0 )*mm - DoBx_Z_Dead;
		
		char buffer1[64];
		sprintf( buffer1, "Si_Do_Bx%d", i+1 );
		char buffer2[64];
		sprintf( buffer2, "Do_BxDead%d", i+1 );
		
		//--- Declare the shape of the volume
   	G4Box* BoxDet = new G4Box(buffer1, DoBx_X/2.0, DoBx_Y/2.0, DoBx_Z/2.0);
   	G4Box* BoxDetDead = new G4Box(buffer2, DoBx_X/2.0, DoBx_Y/2.0 , DoBx_Z_Dead/2.0 );   	
		//--- declare the logical volume with the shape 
	   DoBoxDetLogical = new G4LogicalVolume(BoxDet, materials ->  Silicon, buffer1);
	   DoBoxDetDeadLogical = new G4LogicalVolume(BoxDetDead, materials ->  Silicon, buffer2);	   
	   
	   //--- set only the part active volume of the detector
		DoBoxDetLogical->SetSensitiveDetector( siliconSD );		   	   
	   
		//--- place the logical volume in the world, the center of the logical volume is placed in the G4ThreeVector location
		G4double DoBoxPlaceX, DoBoxPlaceY, DoBoxPlaceZ;
		G4double DoBoxDeadX1, DoBoxDeadY1, DoBoxDeadZ1;		
		
		G4double DoBxXOffSet = 42.5*mm;
		G4double DoBxYoffSet = 42.5*mm;	
		G4double DoBxZoffSet = 15.9*mm;
		G4double DBxExtraOffSet = 4.8*mm;		
				
		if( i == 0 )
		{
			DoBoxPlaceX = DBxExtraOffSet;
			DoBoxPlaceY = DoBxYoffSet + DoBx_Z/2.0;
			DoBoxPlaceZ = DoBxZoffSet + DoBx_Y/2.0;	
				
			DoBoxDeadX1 = DBxExtraOffSet;
			DoBoxDeadY1 = DoBxYoffSet - DoBx_Z_Dead/2.0;
			DoBoxDeadZ1 = DoBxZoffSet + DoBx_Y/2.0;
		}
		if( i == 1 )
		{
			DoBoxPlaceX = -DoBxXOffSet - DoBx_Z/2.0;
			DoBoxPlaceY = DBxExtraOffSet;
			DoBoxPlaceZ = DoBxZoffSet + DoBx_Y/2.0;
			
			DoBoxDeadX1 = -DoBxXOffSet + DoBx_Z_Dead/2.0;
			DoBoxDeadY1 = DBxExtraOffSet;
			DoBoxDeadZ1 = DoBxZoffSet + DoBx_Y/2.0;
		}		
		if( i == 2 )
		{
			DoBoxPlaceX = -DBxExtraOffSet;
			DoBoxPlaceY = -DoBxYoffSet - DoBx_Z/2.0;
			DoBoxPlaceZ = DoBxZoffSet + DoBx_Y/2.0;
			
			DoBoxDeadX1 = -DBxExtraOffSet;
			DoBoxDeadY1 = -DoBxYoffSet + DoBx_Z_Dead/2.0;
			DoBoxDeadZ1 = DoBxZoffSet + DoBx_Y/2.0;
		}
		if( i == 3 )
		{
			DoBoxPlaceX = DoBxXOffSet + DoBx_Z/2.0;
			DoBoxPlaceY = -DBxExtraOffSet;
			DoBoxPlaceZ = DoBxZoffSet + DoBx_Y/2.0;
			
			DoBoxDeadX1 = DoBxXOffSet - DoBx_Z_Dead/2.0;
			DoBoxDeadY1 = -DBxExtraOffSet;
			DoBoxDeadZ1 = DoBxZoffSet + DoBx_Y/2.0;
		}	
		
		G4ThreeVector *DoBoxDetPos = new G4ThreeVector();
		DoBoxDetPos->set( DoBoxPlaceX, DoBoxPlaceY, DoBoxPlaceZ );
		
		//--- place the dead layers
		G4ThreeVector *DoBoxDetPos_Dead1 = new G4ThreeVector();
		DoBoxDetPos_Dead1->set( DoBoxDeadX1, DoBoxDeadY1, DoBoxDeadZ1 );		
		
		G4RotationMatrix DoBxRM;
		if( i == 0)
		{
			DoBxRM.rotateX(pi/2);		
		}
		if( i == 1)
		{
			DoBxRM.rotateY(pi/2);	
			DoBxRM.rotateX(pi/2);
		}
		if( i == 2)
		{
			DoBxRM.rotateX(pi/2);		
		}
		if( i == 3)
		{
			DoBxRM.rotateY(pi/2);	
			DoBxRM.rotateX(pi/2);
		}						
		
		char buffer3[64];
		sprintf( buffer3, "DoBx%d", i+1);
		
		G4PVPlacement *DoBoxDetectors;
		DoBoxDetectors = new G4PVPlacement( G4Transform3D(DoBxRM,*DoBoxDetPos) , DoBoxDetLogical, buffer3, expHallLogical, false,i );	  
	   DoBoxDetLogical->SetVisAttributes(DetAttributes);
	   
	   char buffer4[64];
	   sprintf( buffer4, "Dead1DoBx%d", i+1 );
	   
   	G4VisAttributes* DeadDetAttributes = new G4VisAttributes(G4Colour::White());
	   DeadDetAttributes->SetForceSolid(true);	   
	   
		G4PVPlacement *DoBoxDetectorsDead1;	   
		DoBoxDetectorsDead1 = new G4PVPlacement( G4Transform3D(DoBxRM,*DoBoxDetPos_Dead1) , DoBoxDetDeadLogical, buffer4, expHallLogical, false,i );
		DoBoxDetDeadLogical->SetVisAttributes(DeadDetAttributes);	   	 
	}
	//--------------- End DoBx Box detectors	
   
   //--------------- Start UmBx Detectors
	//--------------- UmBx detectors
	for(int i =0; i < 4; i++)
	{
		//--- The extent of the box is from the origin of the world volume to +/- DoBx_i down the ith axis
		G4double UmBx_X = ( 72.0 )*mm;
		G4double UmBx_Y = ( 48.0 )*mm;
		G4double UmBx_Z_Dead = ( 0.1 )*um;		
		G4double UmBx_Z = ( 1.0 )*mm - UmBx_Z_Dead;
		
		char buffer1[64];
		sprintf( buffer1, "Si_Um_Bx%d", i+1 );
		char buffer2[64];
		sprintf( buffer2, "Um_BxDead%d", i+1 );
		
		//--- Declare the shape of the volume
   	G4Box* BoxDet = new G4Box(buffer1, UmBx_X/2.0, UmBx_Y/2.0, UmBx_Z/2.0);
   	G4Box* BoxDetDead = new G4Box(buffer2, UmBx_X/2.0, UmBx_Y/2.0 , UmBx_Z_Dead/2.0 );   	
		//--- declare the logical volume with the shape 
	   UmBoxDetLogical = new G4LogicalVolume(BoxDet, materials ->  Silicon, buffer1);
	   UmBoxDetDeadLogical = new G4LogicalVolume(BoxDetDead, materials ->  Silicon, buffer2);	   
	   
	   //--- set only the part active volume of the detector
		UmBoxDetLogical->SetSensitiveDetector( siliconSD );		   	   
	   
		//--- place the logical volume in the world, the center of the logical volume is placed in the G4ThreeVector location
		G4double UmBoxPlaceX, UmBoxPlaceY, UmBoxPlaceZ;
		G4double UmBoxDeadX1, UmBoxDeadY1, UmBoxDeadZ1;		
		
		G4double DoBxXOffSet = 40.8*mm;
		G4double DoBxYoffSet = 40.8*mm;
		G4double DoBxZoffSet = -12.8*mm;
		G4double DBxExtraOffSet = 4.8*mm;
				
		if( i == 0 )
		{
			UmBoxPlaceX = -DBxExtraOffSet*mm;
			UmBoxPlaceY = DoBxYoffSet + UmBx_Z/2.0;
			UmBoxPlaceZ = DoBxZoffSet - UmBx_Y/2.0;	
				
			UmBoxDeadX1 = -DBxExtraOffSet*mm;
			UmBoxDeadY1 = DoBxYoffSet - UmBx_Z_Dead/2.0;
			UmBoxDeadZ1 = DoBxZoffSet - UmBx_Y/2.0;
		}
		if( i == 1 )
		{
			UmBoxPlaceX = -DoBxXOffSet - UmBx_Z/2.0;
			UmBoxPlaceY = -DBxExtraOffSet*mm;
			UmBoxPlaceZ = DoBxZoffSet - UmBx_Y/2.0;
			
			UmBoxDeadX1 = -DoBxXOffSet + UmBx_Z_Dead/2.0;
			UmBoxDeadY1 = -DBxExtraOffSet*mm;
			UmBoxDeadZ1 = DoBxZoffSet - UmBx_Y/2.0;
		}		
		if( i == 2 )
		{
			UmBoxPlaceX = DBxExtraOffSet*mm;
			UmBoxPlaceY = -DoBxYoffSet - UmBx_Z/2.0;
			UmBoxPlaceZ = DoBxZoffSet - UmBx_Y/2.0;
			
			UmBoxDeadX1 = DBxExtraOffSet*mm;
			UmBoxDeadY1 = -DoBxYoffSet + UmBx_Z_Dead/2.0;
			UmBoxDeadZ1 = DoBxZoffSet - UmBx_Y/2.0;
		}
		if( i == 3 )
		{
			UmBoxPlaceX = DoBxXOffSet + UmBx_Z/2.0;
			UmBoxPlaceY = DBxExtraOffSet*mm;
			UmBoxPlaceZ = DoBxZoffSet - UmBx_Y/2.0;
			
			UmBoxDeadX1 = DoBxXOffSet - UmBx_Z_Dead/2.0;
			UmBoxDeadY1 = DBxExtraOffSet*mm;
			UmBoxDeadZ1 = DoBxZoffSet - UmBx_Y/2.0;
		}	
		
		G4ThreeVector *UmBoxDetPos = new G4ThreeVector();
		UmBoxDetPos->set( UmBoxPlaceX, UmBoxPlaceY, UmBoxPlaceZ );
		
		//--- place the dead layers
		G4ThreeVector *UmBoxDetPos_Dead1 = new G4ThreeVector();
		UmBoxDetPos_Dead1->set( UmBoxDeadX1, UmBoxDeadY1, UmBoxDeadZ1 );		
		
		G4RotationMatrix UmBxRM;
		if( i == 0)
		{
			UmBxRM.rotateX(pi/2);		
		}
		if( i == 1)
		{
			UmBxRM.rotateY(pi/2);	
			UmBxRM.rotateX(pi/2);
		}
		if( i == 2)
		{
			UmBxRM.rotateX(pi/2);		
		}
		if( i == 3)
		{
			UmBxRM.rotateY(pi/2);	
			UmBxRM.rotateX(pi/2);
		}						
		
		char buffer3[64];
		sprintf( buffer3, "UmBx%d", i+1);
		
		G4PVPlacement *UmBoxDetectors;
		UmBoxDetectors = new G4PVPlacement( G4Transform3D(UmBxRM,*UmBoxDetPos) , UmBoxDetLogical, buffer3, expHallLogical, false,i );	  
	   UmBoxDetLogical->SetVisAttributes(DetAttributes);
	   
	   char buffer4[64];
	   sprintf( buffer4, "Dead1UmBx%d", i+1 );
	   
   	G4VisAttributes* DeadDetAttributes = new G4VisAttributes(G4Colour::White());
	   DeadDetAttributes->SetForceSolid(true);	   
	   
		G4PVPlacement *UmBoxDetectorsDead1;	   
		UmBoxDetectorsDead1 = new G4PVPlacement( G4Transform3D(UmBxRM,*UmBoxDetPos_Dead1) , UmBoxDetDeadLogical, buffer4, expHallLogical, false,i );
		UmBoxDetDeadLogical->SetVisAttributes(DeadDetAttributes);	   	 
	}   
   //--------------- End UmBx detectors
   
	//--------------- UmCD detector	
	for(int i = 0; i < 1; i++)
	{
		//--- the cylinder parameters for the CDs, thickness is the total thickness while the input paramter takes half z-thickness	
		G4double UmCD_InnerRadius = 9.0*mm;
		G4double UmCD_OuterRadius = 41.0*mm;
		G4double UmCD_DeadThick = 0.7*um;		
		G4double UmCD_Thickness = 1.0*mm - 2*UmCD_DeadThick;
		G4double UmCD_StartAngle = 0.0 + i*(pi/2);
		G4double UmCD_EndAngle = 1.4643312;	
		
		G4Tubs* UmCDDet = new G4Tubs("Si_Um_CD", UmCD_InnerRadius, UmCD_OuterRadius, UmCD_Thickness/2, UmCD_StartAngle, UmCD_EndAngle);
		UmCDDetLogical = new G4LogicalVolume( UmCDDet, materials -> Silicon, "Si_Um_CD" );		
		
		UmCDDetLogical->SetSensitiveDetector( siliconSD );		
		
		//--- Set the dead layer logical volume
		char buffer1[64];
		sprintf( buffer1, "Si_UmDead1_CD%d", i+1);
		G4Tubs* UmCDDetDead = new G4Tubs(buffer1, UmCD_InnerRadius, UmCD_OuterRadius, UmCD_DeadThick/2, UmCD_StartAngle, UmCD_EndAngle);
		UmCDDetDeadLogical = new G4LogicalVolume( UmCDDetDead, materials -> Silicon, buffer1 );			
		
		G4double UmCDPlaceX = 0.0*mm;
		G4double UmCDPlaceY = 0.0*mm;
		G4double UmCDPlaceZ = -66.5*mm - UmCD_Thickness/2.0;	
		G4ThreeVector *UmCDDetPos = new G4ThreeVector();
		UmCDDetPos->set( UmCDPlaceX, UmCDPlaceY, UmCDPlaceZ );
		
		G4double UmCDDeadPlaceX = 0.0*mm;
		G4double UmCDDeadPlaceY = 0.0*mm;
		G4double UmCDDeadPlaceZ = -66.5*mm + UmCD_Thickness/2.0 + UmCD_DeadThick/2.0;	
		G4ThreeVector *UmCDDetPosDead = new G4ThreeVector();
		UmCDDetPosDead->set( UmCDDeadPlaceX, UmCDDeadPlaceY, UmCDDeadPlaceZ );		
		
		G4PVPlacement *UmCDDetectors;
		char buffer2[64];
		sprintf( buffer2, "UmCD%d", i+1 );
		UmCDDetectors = new G4PVPlacement( 0, *UmCDDetPos, UmCDDetLogical, buffer2, expHallLogical, false, i );	
	   UmCDDetLogical->SetVisAttributes(DetAttributes);		
	   
		G4PVPlacement *UmCDDeadDetectors;
		char buffer3[64];
		sprintf( buffer3, "UmCD%dDead", i+1 );
		UmCDDeadDetectors = new G4PVPlacement( 0, *UmCDDetPosDead, UmCDDetDeadLogical, buffer3, expHallLogical, false, i );	
	   UmCDDetDeadLogical->SetVisAttributes(DeadDetAttributes);		   	
	}
	//--------------- End UmCD detectors	   
   
   //--------------- Target --- ?Arial density?
   G4double TargetArialThickness = 217;  //ug/cm^2
	G4double Target_X = (5)*mm;
	G4double Target_Y = (5)*mm;
	G4double Target_Z = ( (TargetArialThickness/(2.267/1e-6)) )*cm;   
   
   G4Box* Target = new G4Box("CTarget", Target_X/2, Target_Y/2, Target_Z/2 );
   
 	TargetLogical = new G4LogicalVolume(Target, materials->Target, "ReactionTarget");
 	
 	G4PVPlacement *TargetPlace;
 	
 	TargetPlace = new G4PVPlacement(0, G4ThreeVector( 0.0 , 0.0 , 0.0), TargetLogical, "target", expHallLogical, false, 0); 
   G4VisAttributes* targetAttributes = new G4VisAttributes(G4Colour::Red());
   targetAttributes -> SetForceSolid(true);
   TargetLogical -> SetVisAttributes(targetAttributes);	
}
