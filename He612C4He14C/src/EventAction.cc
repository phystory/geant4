#include "EventAction.hh"

#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "Randomize.hh"

EventAction::EventAction(struct DataPass *LightInfoPass)
{
	LightData = (LightInfoPass);
	siliconCollID = -1;
	moutFile = new TFile("Analysis/SimRootFile/EventActionElastic_pt.root", "RECREATE");
	mActualDataTree = new TTree("actualData", "Event action tree of real values");	
	mReconDataTree = new TTree("reconData", "Event action tree of reconstructed values");
	
	mdEEnergy4He = 0;
	mdEEnergy12C = 0;
	mEEnergy4He = 0;
	mEEnergy12C = 0;	
	
	mTotalEDep12C = 0;
	mTotalEDep6He = 0;
	
  	mActualTheta6He = 0;
  	mActualTheta12C = 0;	
  	
  	for(int j =0; j < 4; j++)
  	{
  		DBoxMultiHit[j] = false;
  	}
  	
  	for(int i =0; i < 50; i++ )
  	{
  		mDetID[i] = -1;
  		mDetSec[i] = -1;	  		
  		mFrontStrip[i] = -1;
  		mDetectorTheta[i] = -1;  		
  		mDetectorPhi[i] = -1;  		
  		mDetectorX[i] = 0;  		
  		mDetectorY[i] = 0;  		
  		mDetectorZ[i] = 0;
  		mSi_dE_F[i] = 0;
  		mSi_dE_B[i] = 0;  		  		
  		mSi_E[i] = 0;
  		m6HeParticle[i] = -1;
	}
	mSetBranches();
	mAlphaTheta = -1.0;
	mAlphaEnergy = -1.0;	
		
	//--- Set the intrisic detector resolution, in units of MeV
	mDiCDResolution = 0.040;
	mDiCD2Resolution = 0.040;
	mDoCDResolution = 0.040;
	mDiBxResolution = 0.040;
	mDoBxResolution = 0.040;
	mUmBxResolution = 0.040;
	mUmCDResolution = 0.040;
	
	//--- Set a threshhold of 100 keV
	mThreshHold = 0.100;
}

EventAction::~EventAction()
{
	moutFile->Write();
	moutFile->Close();
}

void EventAction::BeginOfEventAction(const G4Event* event)
{
	mEventID = event -> GetEventID();
	//G4cout << G4endl << "*****  BEGIN Event Number " << event -> GetEventID() << "  *****" << G4endl;
	
	G4SDManager *SDman = G4SDManager::GetSDMpointer();
	if( siliconCollID < 0 )
	{
		G4String colName;
		siliconCollID = SDman->GetCollectionID( colName="siliconCollection" );
	}
}

void EventAction::EndOfEventAction(const G4Event* evt)
{
	if( mEventID % 100000 == 0)
		G4cout << G4endl << "*****  END Event Number " << mEventID << "  *****" << " siliconCollID " << siliconCollID << G4endl;
      
   if( siliconCollID < 0 )
   	return;
   
   G4HCofThisEvent *HCE = evt->GetHCofThisEvent();
   SiliconHitsCollection *SHC = 0;
   
   if(HCE)
   {
		SHC = (SiliconHitsCollection*)(HCE->GetHC(siliconCollID));   
   }


	//--- Get the vector for the hits within the Sensitive detector
  	std::vector<SiliconHit*> *SiHits = SHC->GetVector();   
	G4int size = SiHits->size();
   
   //--- if there are hits in the event
   if(size>0)
   {
   	mSetActualDataParameters( SiHits );
   }
   
   mReaction = LightData->Reaction;
	mAlphaTheta = LightData->LightTheta;
	mAlphaPhi = LightData->LightPhi;	
	mAlphaEnergy = LightData->LightEnergy;  	   
   
  	mSetReconDataParameters( SiHits );   
}

void EventAction::mSetReconDataParameters( std::vector<SiliconHit*> *Hits )
{
	const int size = Hits->size();
	mHits = 0;

	//--- Use a boolean variable to determine if we see a hit in the Di detector first and then one in the Do. 
	//--- there are a fair amount of events that hit the Do detector without the Di detector due to the geometry and lack of pcb board
	G4bool DiCDHit = false;
	G4bool DiBxHit = false;

	//--- declare the vectors to be filled 
	//--- Detector sector is the section of detection
	//--- detectorSector = 0 -> DCD
	//--- DetectorSector = 1 -> DBx
	//--- DetectorSector = 2 -> UBx
	//--- DetectorSector = 3 -> UCD
	vector<G4int>		FStrip, BStrip, DetectorID, ParticleID, DetectorSector;
	vector<G4double>	dE_Energy_F, dE_Energy_B, E_Energy, DetTheta, DetPhi, DetX, DetY, DetZ;
	
	//--- set all the vectors to zero, though they should be set that way. just done to make sure
	FStrip.clear();
	BStrip.clear();
	DetectorID.clear();
	ParticleID.clear();
	DetectorSector.clear();
	dE_Energy_F.clear();
	dE_Energy_B.clear();	
	E_Energy.clear();
	DetTheta.clear();
	DetPhi.clear();
	DetX.clear();
	DetY.clear();
	DetZ.clear();
	
	G4int AlphaFillCount = 0;
	for(int i =0; i < size; i++)
	{
		//--- Set the variables that we would detect, stuff like exact position and particle name are for debuggin purposes
		//--- these variables come from the sensitive detectors
		G4double edep = Hits->at(i)->GetEdep();
  		G4ThreeVector Position = Hits->at(i)->GetPos();
  		G4double x = Position.getX();
  		G4double y = Position.getY();
  		G4double z = Position.getZ();  		
  		G4String particleName = Hits->at(i)->GetParticleName();  		
  		G4String volumeName = Hits->at(i)->GetVolumeName();   			
		G4double RealTheta = Position.getTheta();
		G4double RealPhi = Position.getPhi();
		if( RealPhi < 0 )
			RealPhi += 2*pi;
		
		char LightParName[] = "alp";
		char HeavyParName[] = "C14";
		char tReaction[] = "tri";
			
		//--- strncmp Returns an integral value indicating the relationship between the strings:
		//--- A zero value indicates that the characters compared in both strings are all equal
		//--- A value greater than zero indicates that the first character that does not match has a greater value in str1 than in str2; 
		//--- And a value less than zero indicates the opposite
		int LightParticle = strncmp(LightParName,particleName,3);
		int HeavyParticle = strncmp(HeavyParName,particleName,3);
		
		if( !strncmp("tri",particleName,3) )		
			HeavyParticle = strncmp(tReaction,particleName,3);
		
		//--- downstream dE CD's
		if( !(strncmp("DiCD",volumeName,4)) && ( LightParticle == 0 || HeavyParticle == 0) )
		{
			G4int DetID = -1;
			
			//--- Set the detID
			if( !(strncmp("DiCD1",volumeName,5)) )
				DetID = 1;
			if( !(strncmp("DiCD2",volumeName,5)) )
				DetID = 2;
			if( !(strncmp("DiCD3",volumeName,5)) )
				DetID = 3;
			if( !(strncmp("DiCD4",volumeName,5)) )
				DetID = 4;
				
			//--- Declare the geometric paramters of the experimental setup and reconstruct the distance from the center of the detector 
			//--- hole to the Si Strip(R)
			G4double z = 78.2;  //--- units of mm
			G4double CDRadFirstStrip = 9.0;
			G4double CDRwidth = 2.0;
			G4double CDRadInterStrip = 0.1;
			G4double R = tan( RealTheta )*z;			
			
			//--- the strip number taking into account the interstrip distance, this number can stay -1 if a hit is detected within an interstrip area
			G4int FrontStrip = -1;
			G4int BackStrip = -1;
			
			//--- the strip number not taking into account the interstrip distance, should never be -1
			G4int FrontStripFull = -1;
			G4int BackStripFull = -1;	
			
			//--- reconstruct the theta from the front strip.  There is a dead layer between strips of 0.1mm and a few events fall into this area
			for( int j =0; j < 16; j++)
			{
				if( R > (CDRadFirstStrip + CDRwidth*(j) ) && R <  (CDRadFirstStrip + (CDRwidth)*(j+1) - CDRadInterStrip ) )
				{
					FrontStrip = j;
				}
				if( R > (CDRadFirstStrip + CDRwidth*(j) ) && R <  (CDRadFirstStrip + (CDRwidth)*(j+1) ) )
				{
					FrontStripFull = j;
				}				
			}

			G4double Cir = 2*pi*R*(3.4/360.);	
			G4double CirActive = Cir-0.1;
			G4double ActivePhi = 3.4*(CirActive/Cir)*pi/180;
			G4double CDDelPhi = 3.4*pi/180;
			
			//--- reconstruct the back strip detection for completeness
			//--- I need to figure out what the span of phi would be for the back strip pitch of 0.1mm
			for( int j =0; j < 24; j++)
			{
				if( RealPhi > (6.4*pi/180. + (CDDelPhi)*(j) + pi/2.0*(DetID-1) ) 
				    && RealPhi < (6.4*pi/180 + (CDDelPhi*(j+1)) - (CDDelPhi-ActivePhi) + pi/2.0*(DetID-1) ) )								    
				{
					BackStrip = j;
				}
				if( RealPhi > (6.4*pi/180. + (CDDelPhi)*(j) + pi/2.0*(DetID-1) ) 
				    && RealPhi < (6.4*pi/180 + (CDDelPhi*(j+1)) + pi/2.0*(DetID-1) ) )								    
				{
					BackStripFull = j;
				}				
			}
			
			G4double RmidFull = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*0.5 ) + (float)(FrontStripFull) * (CDRwidth);	
			G4double Rmid = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*0.5 ) + (float)(FrontStrip) * (CDRwidth);
			G4double RRand = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*G4UniformRand() ) + (float)(FrontStrip) * (CDRwidth);
			G4double RRandFull = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*G4UniformRand()) + (float)(FrontStripFull) * (CDRwidth);				
			G4double RRandFullP1 = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*G4UniformRand()) + (float)(FrontStripFull+1) * (CDRwidth);
			
			G4double ThetaMidFull = atan(RmidFull/z);
			G4double ThetaMid = atan(Rmid/z);
			G4double ThetaRand = atan(RRand/z);	
			G4double ThetaRandFull = atan(RRandFull/z);	
			G4double ThetaRandFullP1 = atan(RRandFullP1/z);	
						
			//mHe6Theta = ThetaMidFull;								
						
			G4double PhiFull = CDDelPhi*BackStripFull + 6.4*pi/180. + pi/2*(DetID-1);
			G4double PhiFullP1 = CDDelPhi*(BackStripFull+1) + 6.4*pi/180. + pi/2*(DetID-1);			
			G4double Phi = CDDelPhi*BackStrip + 6.4*pi/180. + pi/2*(DetID-1);	
			
			if( DetID == 2 )
			{
				PhiFull = CDDelPhi*24./2. + 6.4*pi/180. + pi/2*(DetID-1);
				Phi = CDDelPhi*24./2. + 6.4*pi/180. + pi/2*(DetID-1);
			}
			
			G4double energyDep = 0;
		   G4RandGauss::shoot();
		   energyDep = edep + G4RandGauss::shoot()*mDiCDResolution;
		   if( energyDep < 0 )
		   	energyDep = 0;	
		   
		   //--- make the percentage calculations for the distribution of energy for the front or back
		   //--- theta angle stuff
			G4double BotStripR = CDRadFirstStrip + ( CDRwidth-CDRadInterStrip ) + CDRwidth*FrontStripFull;
			G4double TopStripR = CDRadFirstStrip + ( CDRwidth ) + CDRwidth*FrontStripFull;
			
			//--- how close the point is to the current strip
			G4double BotStripDistance = R - BotStripR;

			//--- the percentage of energy shared to the bottom strip
			G4double BotStripPerEnergy = BotStripDistance/CDRadInterStrip;
			
			G4double BotStripEShare = 1 - BotStripPerEnergy;
			G4double TopStripEShare = BotStripPerEnergy;
			
			//--- Make percentage calculations for the phi angle
			G4double phiToInterstrip = CDDelPhi*BackStripFull + ActivePhi + 6.4*pi/180. + pi/2*(DetID-1);
			//--- angular amount from the interstrip angular range to the point of detection in the interstrip region.
			G4double PhiDifference = RealPhi - phiToInterstrip;
			G4double AngularCoverageInterstrip = 3.4*pi/180 - ActivePhi;
			
			G4double BackStripPercentCov = PhiDifference / AngularCoverageInterstrip;
			
			G4double LeftStripEShare = 1 - BackStripPercentCov;
			G4double RightStripEShare = BackStripPercentCov;
			
			//--- if the event deposits energy in the pixel
		   if( FrontStrip > -1 && BackStrip > -1 )
		   {
		   	FStrip.push_back( FrontStrip );
		   	BStrip.push_back( BackStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 0 );
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRand );
		   	DetPhi.push_back( Phi );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   }
			//--- if the event deposits energy in the front interstrip and a back strip		   
		   else if( ( FrontStrip == -1 && FrontStripFull > -1 ) && BackStrip > -1 )
		   {
		   	if( BotStripEShare > 1 || TopStripEShare > 1 )
		   	{
		   		cout << "BotStripEShare: " << BotStripEShare << " TopStripEShare " << TopStripEShare << " FrontStrip " << FrontStrip << " FrontStripFull " << FrontStripFull << endl;
		   		cout << " phiToInterstrip " << phiToInterstrip << " PhiDifference " << PhiDifference << " RealPhi " << RealPhi << endl;
		   	}		   
		   
		   	//--- first energy share event
		   	FStrip.push_back( FrontStripFull );
		   	BStrip.push_back( BackStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 0 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRandFull );
		   	DetPhi.push_back( Phi );
		   	
		   	dE_Energy_F.push_back( energyDep*BotStripEShare );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   	
		   	//--- second energy share event
		   	FStrip.push_back( FrontStripFull+1 );
		   	BStrip.push_back( BackStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 0 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRandFullP1 );
		   	DetPhi.push_back( Phi );
		   	
		   	dE_Energy_F.push_back( energyDep*TopStripEShare );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		   	
		   }
		   //--- if the event deposits energyin the front strip and the back interstrip distance
		   else if( FrontStrip > -1 && (BackStrip == -1 && BackStripFull > -1 ) )
		   {
		   	if( LeftStripEShare > 1 || RightStripEShare > 1 )
		   	{
		   		cout << "LeftStripEShare: " << LeftStripEShare << " RightStripEShare " << RightStripEShare << " BackStrip " << BackStrip << " BackStripFull " << BackStripFull << endl;
		   		cout << " phiToInterstrip " << phiToInterstrip << " PhiDifference " << PhiDifference << " RealPhi " << RealPhi << endl;
		   	}
  	
		   	//--- first energy share event
		   	FStrip.push_back( FrontStrip );
		   	BStrip.push_back( BackStripFull );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 0 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRand );
		   	DetPhi.push_back( PhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep*LeftStripEShare );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   	
		   	//--- second energy share event
		   	FStrip.push_back( FrontStrip );
		   	BStrip.push_back( BackStripFull+1 );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 0 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRand );
		   	DetPhi.push_back( PhiFullP1 );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep*RightStripEShare );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		   	
		   }
			//--- if the event deposits energy in the interstrip area in both front and back
		   else if( (FrontStrip == -1 && FrontStripFull > -1 ) && (BackStrip == -1 && BackStripFull > -1 ) )
		   {
		   
		   	//--- first energy share event
		   	FStrip.push_back( FrontStripFull );
		   	BStrip.push_back( BackStripFull );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 0 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRandFull );
		   	DetPhi.push_back( PhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep*BotStripEShare );
		   	dE_Energy_B.push_back( energyDep*LeftStripEShare );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   	
		   	//--- second energy share event
		   	FStrip.push_back( FrontStripFull+1 );
		   	BStrip.push_back( BackStripFull+1 );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 0 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRandFullP1 );
		   	DetPhi.push_back( PhiFullP1 );
		   	
		   	dE_Energy_F.push_back( energyDep*TopStripEShare );
		   	dE_Energy_B.push_back( energyDep*RightStripEShare );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		   	
		   }				   	      
		}
		if( !(strncmp("DoCD",volumeName,4)) && ( LightParticle == 0 || HeavyParticle == 0) )
		{
			G4int DetID = -1;
			
			//--- Set the detID
			if( !(strncmp("DoCD1",volumeName,5)) )
				DetID = 1;
			if( !(strncmp("DoCD2",volumeName,5)) )
				DetID = 2;
			if( !(strncmp("DoCD3",volumeName,5)) )
				DetID = 3;
			if( !(strncmp("DoCD4",volumeName,5)) )
				DetID = 4;
				
	   	FStrip.push_back( -1 );
	   	BStrip.push_back( -1 );
	   	DetectorID.push_back( DetID );
	   	DetectorSector.push_back( 0 );	   	
	   	
	   	DetX.push_back( -1 );
	   	DetY.push_back( -1 );
	   	DetZ.push_back( -1 );
	   	
	   	DetTheta.push_back( -1 );
	   	DetPhi.push_back( -1 );
	   	
		   G4RandGauss::shoot();
		   G4double energyDep = edep + G4RandGauss::shoot()*mDiCDResolution;
		   if( energyDep < 0 )
		   	energyDep = 0;		   	
	   	
	   	dE_Energy_F.push_back( -1 );
	   	dE_Energy_B.push_back( -1 );		   	
	   	E_Energy.push_back( energyDep );
	   	
	   	if( LightParticle == 0 )
	   		ParticleID.push_back( 1 );
	   	else if( HeavyParticle == 0 )
	   		ParticleID.push_back( 0 );					
		}
		if( !(strncmp("DiBx",volumeName,4)) && ( LightParticle == 0 || HeavyParticle == 0) )
		{
			//--- Declare the box geometric constants
			//--- All in units of mm
			G4double DBoxXoffSet = 40.8;			
			G4double DBoxYoffSet = 40.8;	
			//--- use this number for numerical purposes.  The particle will sometimes hit right at 15.9 and since they are floating point numbers
			//--- it doesn't recognize that's the beg of the strip					
			G4double DBoxZoffSet = 15.8999999;
			G4double BWidth = 1.0;
			G4double FWidth = 3.0;	
			
			G4int DetID = -1;
			G4int frontStrip = -1, backStrip = -1;
			G4int FullStripF = -1, FullStripB = -1;
			G4double BoxTheta = 0, BoxPhi = 0;			
			//--- cycle through the box's 
			for(int index = 0; index < 4; index++)
			{
			//--- create a buffer and using the index determine which box the hit is in and fill data accordingly
			char buff[10];
			sprintf( buff, "DiBx%d",index+1);
			if( !(strncmp(buff,volumeName,5)) )
			{				
				DetID = index+1;			
				//--- Pick the proper front strip and back strip depending on the detID		
				//--- the 0.1 mm added to the FWidth and BWidth is the interstrip connection thus there is a 0.1 mm dead area of detection between strips	
				if( DetID == 1 )
				{
					for(int j =0; j < 24; j++)
					{
						if( x > DBoxXoffSet - (FWidth*(j+1) - 0.1 ) && x < DBoxXoffSet - FWidth*(j) )
						{
							 frontStrip= j;
						}
						if( x > DBoxXoffSet - (FWidth*(j+1)) && x < DBoxXoffSet - FWidth*(j) )
						{
							FullStripF = j;
						}						
					}
					for(int j = 0; j < 48; j++)
					{
						if( z >= (DBoxZoffSet + BWidth*j) && z < (DBoxZoffSet + (BWidth*(j+1) - 0.1 ) ) )
						{
							backStrip = j; 
						}
						if( z >= (DBoxZoffSet + BWidth*j) && z < (DBoxZoffSet + (BWidth*(j+1)) ) )								
						{
							FullStripB = j;
						}
					}
				}		
				else if( DetID == 2 )
				{
					for(int j =0; j < 24; j++)
					{
						if( y > DBoxYoffSet - (FWidth*(j+1)-0.1) && y < DBoxYoffSet - FWidth*(j) )
						{
							 frontStrip= j;
						}
						if( y > DBoxYoffSet - (FWidth*(j+1)) && y < DBoxYoffSet - FWidth*(j) )						
						{
							FullStripF = j;						
						}					
					}
					for(int j = 0; j < 48; j++)
					{
						if( z >= (DBoxZoffSet + BWidth*j) && z < (DBoxZoffSet + (BWidth*(j+1) - 0.1 ) ) )
						{
							backStrip = j; 
						}
						if( z >= (DBoxZoffSet + BWidth*j) && z < (DBoxZoffSet + (BWidth*(j+1)) ) )
						{
							FullStripB = j;
						}					
					}
				}	
				else if( DetID == 3 )
				{
					for(int j =0; j < 24; j++)
					{
						if( x > -DBoxXoffSet + FWidth*j && x < -DBoxXoffSet + (FWidth*(j+1)- 0.1 ) )
						{
							frontStrip = j;
						}
						if( x > -DBoxXoffSet + FWidth*j && x < -DBoxXoffSet + (FWidth*(j+1)) )
						{
							FullStripF = j;						
						}						
					}
					for(int j = 0; j < 48; j++)
					{
						if( z >= (DBoxZoffSet + BWidth*j) && z < (DBoxZoffSet + (BWidth*(j+1) - 0.1 ) ) )
						{
							backStrip = j; 
						}
						if( z >= (DBoxZoffSet + BWidth*j) && z < (DBoxZoffSet + (BWidth*(j+1)) ) )
						{
							FullStripB = j;						
						}
					}
				}	
				else if( DetID == 4 )
				{
					for(int j =0; j < 24; j++)
					{
						if( y > -DBoxYoffSet + FWidth*j && y < -DBoxYoffSet + (FWidth*(j+1) - 0.1 ) )
						{
							frontStrip = j;
						}
						if( y > -DBoxYoffSet + FWidth*j && y < -DBoxYoffSet + (FWidth*(j+1)) )
						{
							FullStripF = j;						
						}
					}
					for(int j = 0; j < 48; j++)
					{
						if( z >= (DBoxZoffSet + BWidth*j) && z < (DBoxZoffSet + (BWidth*(j+1) - 0.1 ) ) )
						{
							backStrip = j; 
						}
						if( z >= (DBoxZoffSet + BWidth*j) && z < (DBoxZoffSet + (BWidth*(j+1)) ) )						
						{
							FullStripB = j;						
						}						
					}
				}
			}				
			}				
			
			//--- Reconstruct the x, y, z values assuming in the center of the front back strips, x,y are different for different boxes
			G4double xRecon, yRecon, zRecon;
			G4double xReconFull, yReconFull, zReconFull;
			G4double xReconEndActiveStrip, yReconEndActiveStrip, zReconEndActiveStrip;
			
			G4double xNextFStrip, yNextFStrip, zNextFStrip;
			G4double xNextBStrip, yNextBStrip, zNextBStrip;				
			G4double xNextFBStrip, yNextFBStrip, zNextFBStrip;			
			//--- difference of the length over the interstrip  and the length to the beginning of the interstrip distance
			G4double InterStripDifferenceF =- 1;			
			G4double InterStripDifferenceB =- 1;						
			if( DetID == 1 )
			{
				xRecon = DBoxXoffSet - FWidth*frontStrip - FWidth*0.5;
				yRecon = DBoxYoffSet;
				zRecon = DBoxZoffSet + BWidth*backStrip + BWidth*0.5;
				
				xReconFull = DBoxXoffSet - FWidth*FullStripF - FWidth*0.5;
				yReconFull = DBoxYoffSet;
				zReconFull = DBoxZoffSet + BWidth*FullStripB + BWidth*0.5;
				
				xReconEndActiveStrip = DBoxXoffSet - FWidth*FullStripF - ( FWidth - 0.1 );
				yReconEndActiveStrip = DBoxYoffSet;
				zReconEndActiveStrip = DBoxZoffSet + BWidth*FullStripB + ( BWidth - 0.1 );	
				
				xNextFStrip = DBoxXoffSet - FWidth*(FullStripF+1) - FWidth*0.5;
				yNextFStrip = DBoxYoffSet;
				zNextFStrip = DBoxZoffSet + BWidth*FullStripB + BWidth*0.5;
				xNextBStrip = DBoxXoffSet - FWidth*(FullStripF) - FWidth*0.5;
				yNextBStrip = DBoxYoffSet;
				zNextBStrip = DBoxZoffSet + BWidth*(FullStripB+1) + BWidth*0.5;	
				
				xNextFBStrip = DBoxXoffSet - FWidth*(FullStripF+1) - FWidth*0.5;
				yNextFBStrip = DBoxYoffSet;
				zNextFBStrip = DBoxZoffSet + BWidth*(FullStripB+1) + BWidth*0.5;								
				
				if( frontStrip == -1 )
				{
					if( x - xReconEndActiveStrip > 0 )
						InterStripDifferenceF = x - xReconEndActiveStrip;
					else
						InterStripDifferenceF = xReconEndActiveStrip - x;					
				}
				if( backStrip == -1 )
					InterStripDifferenceB = z - zReconEndActiveStrip;			
			}
			else if( DetID == 2 )
			{
				xRecon = -DBoxXoffSet;
				yRecon = DBoxYoffSet - FWidth*frontStrip - FWidth*0.5;
				zRecon = DBoxZoffSet + BWidth*backStrip + BWidth*0.5;
				
				xReconFull = -DBoxXoffSet;
				yReconFull = DBoxYoffSet - FWidth*FullStripF - FWidth*0.5;
				zReconFull = DBoxZoffSet + BWidth*FullStripB + BWidth*0.5;
				
				xReconEndActiveStrip = -DBoxXoffSet;
				yReconEndActiveStrip = DBoxYoffSet - FWidth*FullStripF - (FWidth-0.1);
				zReconEndActiveStrip = DBoxZoffSet + BWidth*FullStripB + (BWidth-0.1);
				
				xNextFStrip = -DBoxXoffSet;
				yNextFStrip = DBoxYoffSet - FWidth*(FullStripF+1) - FWidth*0.5;
				zNextFStrip = DBoxZoffSet + BWidth*FullStripB + BWidth*0.5;
				xNextBStrip = -DBoxXoffSet;
				yNextBStrip = DBoxYoffSet - FWidth*FullStripF - FWidth*0.5;
				zNextBStrip = DBoxZoffSet + BWidth*(FullStripB+1) + BWidth*0.5;				
				
				xNextFBStrip = -DBoxXoffSet;
				yNextFBStrip = DBoxYoffSet - FWidth*(FullStripF+1) - FWidth*0.5;
				zNextFBStrip = DBoxZoffSet + BWidth*(FullStripB+1) + BWidth*0.5;					
				
				if( frontStrip == -1 )
				{
					if( y - yReconEndActiveStrip > 0 )
						InterStripDifferenceF = y - yReconEndActiveStrip;
					else 
						InterStripDifferenceF = yReconEndActiveStrip - y;
				}
				if( backStrip == -1 )
					InterStripDifferenceB = z - zReconEndActiveStrip;								
			}
			else if( DetID == 3 )
			{
				xRecon = -DBoxXoffSet + FWidth*frontStrip + FWidth*0.5;
				yRecon = -DBoxYoffSet;
				zRecon = DBoxZoffSet + BWidth*backStrip + BWidth*0.5;
				
				xReconFull = -DBoxXoffSet + FWidth*FullStripF + FWidth*0.5;
				yReconFull = -DBoxYoffSet;
				zReconFull = DBoxZoffSet + BWidth*FullStripB + BWidth*0.5;
				
				xReconEndActiveStrip = -DBoxXoffSet + FWidth*FullStripF + (FWidth-0.1);
				yReconEndActiveStrip = -DBoxYoffSet;
				zReconEndActiveStrip = DBoxZoffSet + BWidth*FullStripB + (BWidth-0.1);	
				
				xNextFStrip = -DBoxXoffSet + FWidth*(FullStripF+1) + FWidth*0.5;
				yNextFStrip = -DBoxYoffSet;
				zNextFStrip = DBoxZoffSet + BWidth*FullStripB + BWidth*0.5;
				xNextBStrip = -DBoxXoffSet + FWidth*FullStripF + FWidth*0.5;
				yNextBStrip = -DBoxYoffSet;
				zNextBStrip = DBoxZoffSet + BWidth*(FullStripB+1) + BWidth*0.5;				
				
				xNextFBStrip = -DBoxXoffSet + FWidth*(FullStripF+1) + FWidth*0.5;
				yNextFBStrip = -DBoxYoffSet;
				zNextFBStrip = DBoxZoffSet + BWidth*(FullStripB+1) + BWidth*0.5;								
				
				if( frontStrip == -1 )
				{
					if( x - xReconEndActiveStrip > 0 )
						InterStripDifferenceF = x - xReconEndActiveStrip;
					else
						InterStripDifferenceF = xReconEndActiveStrip - x;					
				}
				if( backStrip == -1 )
					InterStripDifferenceB = z - zReconEndActiveStrip;								
			}
			else if( DetID == 4 )
			{
				xRecon = DBoxXoffSet;
				yRecon = -DBoxYoffSet + FWidth*frontStrip + FWidth*0.5;
				zRecon = DBoxZoffSet + BWidth*backStrip + BWidth*0.5;
				
				xReconFull = DBoxXoffSet;
				yReconFull = -DBoxYoffSet + FWidth*FullStripF + FWidth*0.5;
				zReconFull = DBoxZoffSet + BWidth*FullStripB + BWidth*0.5;
				
				xReconEndActiveStrip = DBoxXoffSet;
				yReconEndActiveStrip = -DBoxYoffSet + FWidth*FullStripF + (FWidth-0.1);
				zReconEndActiveStrip = DBoxZoffSet + BWidth*FullStripB + (BWidth-0.1);	
				
				xNextFStrip = DBoxXoffSet;
				yNextFStrip = -DBoxYoffSet + FWidth*(FullStripF+1) + FWidth*0.5;
				zNextFStrip = DBoxZoffSet + BWidth*FullStripB + BWidth*0.5;
				xNextBStrip = DBoxXoffSet;
				yNextBStrip = -DBoxYoffSet + FWidth*FullStripF + FWidth*0.5;
				zNextBStrip = DBoxZoffSet + BWidth*(FullStripB+1) + BWidth*0.5;	
				
				xNextFBStrip = DBoxXoffSet;
				yNextFBStrip = -DBoxYoffSet + FWidth*(FullStripF+1) + FWidth*0.5;
				zNextFBStrip = DBoxZoffSet + BWidth*(FullStripB+1) + BWidth*0.5;								
				
				if( frontStrip == -1 )
				{
					if( y - yReconEndActiveStrip > 0 )
						InterStripDifferenceF = y - yReconEndActiveStrip;
					else 
						InterStripDifferenceF = yReconEndActiveStrip - y;
				}
				if( backStrip == -1 )
					InterStripDifferenceB = z - zReconEndActiveStrip;									
			}												
	
			G4double Mag = sqrt( xRecon*xRecon + yRecon*yRecon + zRecon*zRecon );
			BoxTheta = acos( zRecon/Mag );
			if( xRecon > 0.0 && yRecon > 0.0 )
				BoxPhi = atan( yRecon / xRecon );
			else if( xRecon < 0.0 && yRecon > 0.0 )
				BoxPhi = atan( yRecon / xRecon ) + pi;
			else if( xRecon < 0.0 && yRecon < 0.0 )
				BoxPhi = atan( yRecon / xRecon ) + pi;					
			else if( xRecon > 0.0 && yRecon < 0.0 )
				BoxPhi = atan( yRecon / xRecon ) + 2*pi;
			
			G4double MagFull = sqrt( xReconFull*xReconFull + yReconFull*yReconFull + zReconFull*zReconFull );
			G4double BoxThetaFull = acos( zReconFull/MagFull );
			G4double BoxPhiFull;
			if( xReconFull > 0.0 && yReconFull > 0.0 )
				BoxPhiFull = atan( yReconFull / xReconFull );
			else if( xReconFull < 0.0 && yReconFull > 0.0 )
				BoxPhiFull = atan( yReconFull / xReconFull ) + pi;
			else if( xReconFull < 0.0 && yReconFull < 0.0 )
				BoxPhiFull = atan( yReconFull / xReconFull ) + pi;					
			else if( xReconFull > 0.0 && yReconFull < 0.0 )
				BoxPhiFull = atan( yReconFull / xReconFull ) + 2*pi;				
				
			G4double MagFullNextF = sqrt( xNextFStrip*xNextFStrip + yNextFStrip*yNextFStrip + zNextFStrip*zNextFStrip );
			G4double BoxThetaNextF = acos( zNextFStrip/MagFullNextF );
			G4double BoxPhiNextF=-1;
			if( xNextFStrip > 0.0 && yNextFStrip > 0.0 )
				BoxPhiNextF = atan( yNextFStrip / xNextFStrip );
			else if( xNextFStrip < 0.0 && yNextFStrip > 0.0 )
				BoxPhiNextF = atan( yNextFStrip / xNextFStrip ) + pi;
			else if( xNextFStrip < 0.0 && yNextFStrip < 0.0 )
				BoxPhiNextF = atan( yNextFStrip / xNextFStrip ) + pi;					
			else if( xNextFStrip > 0.0 && yNextFStrip < 0.0 )
				BoxPhiNextF = atan( yNextFStrip / xNextFStrip ) + 2*pi;				
				
			G4double MagFullNextB = sqrt( xNextBStrip*xNextBStrip + yNextBStrip*yNextBStrip + zNextBStrip*zNextBStrip );
			G4double BoxThetaNextB = acos( zNextBStrip/MagFullNextB );
			G4double BoxPhiNextB=-1;
			if( xNextBStrip > 0.0 && yNextBStrip > 0.0 )
				BoxPhiNextB = atan( yNextBStrip / xNextBStrip );
			else if( xNextBStrip < 0.0 && yNextBStrip > 0.0 )
				BoxPhiNextB = atan( yNextBStrip / xNextBStrip ) + pi;
			else if( xNextBStrip < 0.0 && yNextBStrip < 0.0 )
				BoxPhiNextB = atan( yNextBStrip / xNextBStrip ) + pi;					
			else if( xNextBStrip > 0.0 && yNextBStrip < 0.0 )
				BoxPhiNextB = atan( yNextBStrip / xNextBStrip ) + 2*pi;					
				
			G4double MagFullNextFB = sqrt( xNextFBStrip*xNextFBStrip + yNextFBStrip*yNextFBStrip + zNextFBStrip*zNextFBStrip );
			G4double BoxThetaNextFB = acos( zNextFBStrip/MagFullNextFB );
			G4double BoxPhiNextFB=-1;
			if( xNextFBStrip > 0.0 && yNextFBStrip > 0.0 )
				BoxPhiNextFB = atan( yNextFBStrip / xNextFBStrip );
			else if( xNextFBStrip < 0.0 && yNextFBStrip > 0.0 )
				BoxPhiNextFB = atan( yNextFBStrip / xNextFBStrip ) + pi;
			else if( xNextFBStrip < 0.0 && yNextFBStrip < 0.0 )
				BoxPhiNextFB = atan( yNextFBStrip / xNextFBStrip ) + pi;					
			else if( xNextFBStrip > 0.0 && yNextFBStrip < 0.0 )
				BoxPhiNextFB = atan( yNextFBStrip / xNextFBStrip ) + 2*pi;					
			
			//--- fill the box theta phi from the full front back strip hit
			if( AlphaFillCount == 1 &&  LightParticle == 0 )
			{	
				mAlphaTheta = BoxThetaFull;
			}			
			
			G4double energyDep = 0;
			G4RandGauss::shoot();
			energyDep = edep + G4RandGauss::shoot()*mDiBxResolution;
			if( energyDep < 0 )
				energyDep = 0;				
			
			G4double FrontRatioInter = InterStripDifferenceF / 0.1;
			G4double BackRatioInter = InterStripDifferenceB / 0.1;		
			
			G4double FrontPercentSame = 1-FrontRatioInter;
			G4double FrontPercentNext = FrontRatioInter;
			
			G4double BackPercentSame = 1-BackRatioInter;
			G4double BackPercentNext = BackRatioInter;

			//--- if the event deposits energy in the pixel
		   if( frontStrip > -1 && backStrip > -1 )
		   {		   			   
		   	FStrip.push_back( frontStrip );
		   	BStrip.push_back( backStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 1 );
		   	
		   	DetX.push_back( xRecon );
		   	DetY.push_back( yRecon );
		   	DetZ.push_back( zRecon );
		   	
		   	DetTheta.push_back( BoxTheta );
		   	DetPhi.push_back( BoxPhi );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   }
		   else if( ( frontStrip == -1 && FullStripF > -1 ) && backStrip > -1 )
		   {		   			   
		   	//--- first hit in front
		   	FStrip.push_back( FullStripF );
		   	BStrip.push_back( backStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 1 );		   	
		   	
		   	DetX.push_back( xReconFull );
		   	DetY.push_back( yReconFull );
		   	DetZ.push_back( zReconFull );
		   	
		   	DetTheta.push_back( BoxThetaFull );
		   	DetPhi.push_back( BoxPhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep*FrontPercentSame );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		
		   		
		   	//--- first hit in front
		   	FStrip.push_back( FullStripF+1 );
		   	BStrip.push_back( backStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 1 );		   	
		   	
		   	DetX.push_back( xNextFStrip );
		   	DetY.push_back( yNextFStrip );
		   	DetZ.push_back( zNextFStrip );
		   	
		   	DetTheta.push_back( BoxThetaNextF );
		   	DetPhi.push_back( BoxPhiNextF );
		   	
		   	dE_Energy_F.push_back( energyDep*FrontPercentNext );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );			   		   
		   }  
		   else if( frontStrip > -1 && backStrip == -1 && FullStripB > -1)
		   {   
		   	//--- first hit in front
		   	FStrip.push_back( frontStrip );
		   	BStrip.push_back( FullStripB );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 1 );		   	
		   	
		   	DetX.push_back( xReconFull );
		   	DetY.push_back( yReconFull );
		   	DetZ.push_back( zReconFull );
		   	
		   	DetTheta.push_back( BoxThetaFull );
		   	DetPhi.push_back( BoxPhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep*BackPercentSame );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		
		   		
		   	//--- first hit in front
		   	FStrip.push_back( frontStrip );
		   	BStrip.push_back( FullStripB+1 );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 1 );		   	
		   	
		   	DetX.push_back( xNextBStrip );
		   	DetY.push_back( yNextBStrip );
		   	DetZ.push_back( zNextBStrip );
		   	
		   	DetTheta.push_back( BoxThetaNextB );
		   	DetPhi.push_back( BoxPhiNextB );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep*BackPercentNext );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );			   		   
		   }  		   
		   else if( frontStrip == -1 && FullStripF > -1 && backStrip == -1 && FullStripB > -1)
		   {
		   	//--- first hit in front
		   	FStrip.push_back( FullStripF );
		   	BStrip.push_back( FullStripB );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 1 );		   	
		   	
		   	DetX.push_back( xReconFull );
		   	DetY.push_back( yReconFull );
		   	DetZ.push_back( zReconFull );
		   	
		   	DetTheta.push_back( BoxThetaFull );
		   	DetPhi.push_back( BoxPhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep*FrontPercentSame );
		   	dE_Energy_B.push_back( energyDep*BackPercentSame );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		
		   		
		   	//--- first hit in front
		   	FStrip.push_back( FullStripF+1 );
		   	BStrip.push_back( FullStripB+1 );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 1 );		   	
		   	
		   	DetX.push_back( xNextFBStrip );
		   	DetY.push_back( yNextFBStrip );
		   	DetZ.push_back( zNextFBStrip );
		   	
		   	DetTheta.push_back( BoxThetaNextFB );
		   	DetPhi.push_back( BoxPhiNextFB );
		   	
		   	dE_Energy_F.push_back( energyDep*FrontPercentNext );
		   	dE_Energy_B.push_back( energyDep*BackPercentNext );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );			   		   
		   } 		   
		}
		if( !(strncmp("DoBx",volumeName,4)) && ( LightParticle == 0 || HeavyParticle == 0) )
		{
			G4int DetID = -1;
			
			//--- Set the detID
			if( !(strncmp("DoBx1",volumeName,5)) )
				DetID = 1;
			if( !(strncmp("DoBx2",volumeName,5)) )
				DetID = 2;
			if( !(strncmp("DoBx3",volumeName,5)) )
				DetID = 3;
			if( !(strncmp("DoBx4",volumeName,5)) )
				DetID = 4;
				
	   	FStrip.push_back( -1 );
	   	BStrip.push_back( -1 );
	   	DetectorID.push_back( DetID );
	   	DetectorSector.push_back( 1 );	   	
	   	
	   	DetX.push_back( -1 );
	   	DetY.push_back( -1 );
	   	DetZ.push_back( -1 );
	   	
	   	DetTheta.push_back( -1 );
	   	DetPhi.push_back( -1 );
	   	
		   G4RandGauss::shoot();
		   G4double energyDep = edep + G4RandGauss::shoot()*mDiCDResolution;
		   if( energyDep < 0 )
		   	energyDep = 0;		   	
	   	
	   	dE_Energy_F.push_back( -1 );
	   	dE_Energy_B.push_back( -1 );		   	
	   	E_Energy.push_back( energyDep );
	   	
	   	if( LightParticle == 0 )
	   		ParticleID.push_back( 1 );
	   	else if( HeavyParticle == 0 )
	   		ParticleID.push_back( 0 );					
		}
		if( !(strncmp("UmBx",volumeName,4)) && ( LightParticle == 0 || HeavyParticle == 0) )
		{
			//--- Declare the box geometric constants
			//--- All in units of mm
			G4double UBoxXoffSet = 40.8;			
			G4double UBoxYoffSet = 40.8;	
			//--- use this number for numerical purposes.  The particle will sometimes hit right at 15.9 and since they are floating point numbers
			//--- it doesn't recognize that's the beg of the strip					
			G4double UBoxZoffSet = -12.799999;
			G4double BWidth = 1.0;
			G4double FWidth = 3.0;
			
			G4int DetID = -1;
			G4int frontStrip = -1, backStrip = -1;
			G4int FullStripF = -1, FullStripB = -1;
			G4double BoxTheta = 0, BoxPhi = 0;			
			//--- cycle through the box's 
			for(int index = 0; index < 4; index++)
			{
			//--- create a buffer and using the index determine which box the hit is in and fill data accordingly
			char buff[10];
			sprintf( buff, "UmBx%d",index+1);
			if( !(strncmp(buff,volumeName,5)) )
			{				
				DetID = index+1;			
				//--- Pick the proper front strip and back strip depending on the detID		
				//--- the 0.1 mm added to the FWidth and BWidth is the interstrip connection thus there is a 0.1 mm dead area of detection between strips	
				if( DetID == 1 )
				{
					for(int j =0; j < 24; j++)
					{
						if( x < -UBoxXoffSet + (FWidth*(j+1)-0.1) && x > -UBoxXoffSet + FWidth*(j) )
						{
							 frontStrip= j;
						}
						if( x < -UBoxXoffSet + (FWidth*(j+1)) && x > -UBoxXoffSet + FWidth*(j) )						
						{
							FullStripF = j;
						}						
					}
					for(int j = 0; j < 48; j++)
					{
						if( z <= (UBoxZoffSet - BWidth*j) && z > (UBoxZoffSet - (BWidth*(j+1)-0.1) ) )
						{
							backStrip = j; 
						}
						if( z <= (UBoxZoffSet - BWidth*j) && z > (UBoxZoffSet - (BWidth*(j+1)) ) )
						{
							FullStripB = j;
						}
					}
				}		
				else if( DetID == 2 )
				{
					for(int j =0; j < 24; j++)
					{
						if( y < -UBoxYoffSet + (FWidth*(j+1)-0.1) && y > -UBoxYoffSet + FWidth*(j) )
						{
							 frontStrip= j;
						}
						if( y < -UBoxYoffSet + (FWidth*(j+1)) && y > -UBoxYoffSet + FWidth*(j) )
						{
							FullStripF = j;						
						}					
					}
					for(int j = 0; j < 48; j++)
					{
						if( z <= (UBoxZoffSet - BWidth*j) && z > (UBoxZoffSet - (BWidth*(j+1)-0.1) ) )
						{
							backStrip = j; 
						}
						if( z <= (UBoxZoffSet - BWidth*j) && z > (UBoxZoffSet - (BWidth*(j+1)) ) )
						{
							FullStripB = j;
						}				
					}
				}	
				else if( DetID == 3 )
				{
					for(int j =0; j < 24; j++)
					{
						if( x < UBoxXoffSet - (FWidth)*j && x > UBoxXoffSet - (FWidth*(j+1)-0.1) )
						{
							frontStrip = j;
						}
						if( x < UBoxXoffSet - (FWidth)*j && x > UBoxXoffSet - (FWidth*(j+1)) )						
						{
							FullStripF = j;						
						}						
					}
					for(int j = 0; j < 48; j++)
					{
						if( z <= (UBoxZoffSet - BWidth*j) && z > (UBoxZoffSet - (BWidth*(j+1)-0.1) ) )
						{
							backStrip = j; 
						}
						if( z <= (UBoxZoffSet - BWidth*j) && z > (UBoxZoffSet - (BWidth*(j+1)) ) )
						{
							FullStripB = j;
						}
					}
				}	
				else if( DetID == 4 )
				{
					for(int j =0; j < 24; j++)
					{
						if( y < UBoxYoffSet - (FWidth)*j && y > UBoxYoffSet - (FWidth*(j+1)-0.1) )
						{
							frontStrip = j;
						}
						if( y < UBoxYoffSet - (FWidth)*j && y > UBoxYoffSet - (FWidth*(j+1)) )
						{
							FullStripF = j;						
						}
					}
					for(int j = 0; j < 48; j++)
					{
						if( z <= (UBoxZoffSet - BWidth*j) && z > (UBoxZoffSet - (BWidth*(j+1)-0.1) ) )
						{
							backStrip = j; 
						}
						if( z <= (UBoxZoffSet - BWidth*j) && z > (UBoxZoffSet - (BWidth*(j+1)) ) )
						{
							FullStripB = j;
						}					
					}
				}
			}				
			}				
			
			//--- Reconstruct the x, y, z values assuming in the center of the front back strips, x,y are different for different boxes
			G4double xRecon, yRecon, zRecon;
			G4double xReconFull, yReconFull, zReconFull;
			G4double xReconEndActiveStrip, yReconEndActiveStrip, zReconEndActiveStrip;
			
			G4double xNextFStrip, yNextFStrip, zNextFStrip;
			G4double xNextBStrip, yNextBStrip, zNextBStrip;				
			G4double xNextFBStrip, yNextFBStrip, zNextFBStrip;			
			//--- difference of the length over the interstrip  and the length to the beginning of the interstrip distance
			G4double InterStripDifferenceF =- 1;			
			G4double InterStripDifferenceB =- 1;						
			if( DetID == 1 )
			{
				xRecon = -UBoxXoffSet + FWidth*frontStrip + FWidth*0.5;
				yRecon = UBoxYoffSet;
				zRecon = UBoxZoffSet - BWidth*backStrip - BWidth*0.5;
				
				xReconFull = -UBoxXoffSet + FWidth*FullStripF + FWidth*0.5;
				yReconFull = UBoxYoffSet;
				zReconFull = UBoxZoffSet - BWidth*FullStripB - BWidth*0.5;
				
				xReconEndActiveStrip = -UBoxXoffSet + FWidth*FullStripF + ( FWidth - 0.1 );
				yReconEndActiveStrip = UBoxYoffSet;
				zReconEndActiveStrip = UBoxZoffSet - BWidth*FullStripB - ( BWidth - 0.1 );	
				
				xNextFStrip = -UBoxXoffSet + FWidth*(FullStripF+1) + FWidth*0.5;
				yNextFStrip = UBoxYoffSet;
				zNextFStrip = UBoxZoffSet - BWidth*FullStripB - BWidth*0.5;
				
				xNextBStrip = -UBoxXoffSet + FWidth*(FullStripF) + FWidth*0.5;
				yNextBStrip = UBoxYoffSet;
				zNextBStrip = UBoxZoffSet - BWidth*(FullStripB+1) - BWidth*0.5;	
				
				xNextFBStrip = -UBoxXoffSet + FWidth*(FullStripF+1) + FWidth*0.5;
				yNextFBStrip = UBoxYoffSet;
				zNextFBStrip = UBoxZoffSet - BWidth*(FullStripB+1) - BWidth*0.5;								
				
				if( frontStrip == -1 )
				{
					if( x - xReconEndActiveStrip > 0 )
						InterStripDifferenceF = x - xReconEndActiveStrip;
					else
						InterStripDifferenceF = xReconEndActiveStrip - x;					
				}
				if( backStrip == -1 )
					InterStripDifferenceB = zReconEndActiveStrip - z;			
			}
			else if( DetID == 2 )
			{
				xRecon = -UBoxXoffSet;
				yRecon = -UBoxYoffSet + FWidth*frontStrip + FWidth*0.5;
				zRecon = UBoxZoffSet - BWidth*backStrip - BWidth*0.5;
				
				xReconFull = -UBoxXoffSet;
				yReconFull = -UBoxYoffSet + FWidth*FullStripF + FWidth*0.5;
				zReconFull = UBoxZoffSet - BWidth*FullStripB - BWidth*0.5;
				
				xReconEndActiveStrip = -UBoxXoffSet;
				yReconEndActiveStrip = -UBoxYoffSet + FWidth*FullStripF + (FWidth-0.1);
				zReconEndActiveStrip = UBoxZoffSet - BWidth*FullStripB - ( BWidth - 0.1 );	
				
				xNextFStrip = -UBoxXoffSet;
				yNextFStrip = -UBoxYoffSet + FWidth*(FullStripF+1) + FWidth*0.5;
				zNextFStrip = UBoxZoffSet - BWidth*FullStripB - BWidth*0.5;
				
				xNextBStrip = -UBoxXoffSet;
				yNextBStrip = -UBoxYoffSet + FWidth*FullStripF + FWidth*0.5;
				zNextBStrip = UBoxZoffSet - BWidth*(FullStripB+1) - BWidth*0.5;	
				
				xNextFBStrip = -UBoxXoffSet;
				yNextFBStrip = -UBoxYoffSet + FWidth*(FullStripF+1) + FWidth*0.5;
				zNextFBStrip = UBoxZoffSet - BWidth*(FullStripB+1) - BWidth*0.5;
				
				if( frontStrip == -1 )
				{
					if( y - yReconEndActiveStrip > 0 )
						InterStripDifferenceF = y - yReconEndActiveStrip;
					else
						InterStripDifferenceF = yReconEndActiveStrip - y;					
				}
				if( backStrip == -1 )
					InterStripDifferenceB = zReconEndActiveStrip - z;								
			}
			else if( DetID == 3 )
			{
				xRecon = UBoxXoffSet - FWidth*frontStrip - FWidth*0.5;
				yRecon = -UBoxYoffSet;
				zRecon = UBoxZoffSet - BWidth*backStrip - BWidth*0.5;
				
				xReconFull = UBoxXoffSet - FWidth*FullStripF - FWidth*0.5;
				yReconFull = -UBoxYoffSet;
				zReconFull = UBoxZoffSet - BWidth*FullStripB - BWidth*0.5;
				
				xReconEndActiveStrip = UBoxXoffSet - FWidth*FullStripF - (FWidth-0.1);
				yReconEndActiveStrip = -UBoxYoffSet;
				zReconEndActiveStrip = UBoxZoffSet - BWidth*FullStripB - ( BWidth - 0.1 );	
				
				xNextFStrip = UBoxXoffSet - FWidth*(FullStripF+1) - FWidth*0.5;
				yNextFStrip = -UBoxYoffSet;
				zNextFStrip = UBoxZoffSet - BWidth*FullStripB - BWidth*0.5;
				
				xNextBStrip = UBoxXoffSet - FWidth*FullStripF - FWidth*0.5;
				yNextBStrip = -UBoxYoffSet;
				zNextBStrip = UBoxZoffSet - BWidth*(FullStripB+1) - BWidth*0.5;	
				
				xNextFBStrip = UBoxXoffSet - FWidth*(FullStripF+1) - FWidth*0.5;
				yNextFBStrip = -UBoxYoffSet;
				zNextFBStrip = UBoxZoffSet - BWidth*(FullStripB+1) - BWidth*0.5;
				
				if( frontStrip == -1 )
				{
					if( x - xReconEndActiveStrip > 0 )
						InterStripDifferenceF = x - xReconEndActiveStrip;
					else
						InterStripDifferenceF = xReconEndActiveStrip - x;					
				}
				if( backStrip == -1 )
					InterStripDifferenceB = zReconEndActiveStrip - z;							
			}
			else if( DetID == 4 )
			{
				xRecon = UBoxXoffSet;
				yRecon = UBoxYoffSet - FWidth*frontStrip - FWidth*0.5;
				zRecon = UBoxZoffSet - BWidth*backStrip - BWidth*0.5;
				
				xReconFull = UBoxXoffSet;
				yReconFull = UBoxYoffSet - FWidth*FullStripF - FWidth*0.5;
				zReconFull = UBoxZoffSet - BWidth*FullStripB - BWidth*0.5;
				
				xReconEndActiveStrip = UBoxXoffSet;
				yReconEndActiveStrip = UBoxYoffSet - FWidth*FullStripF - (FWidth-0.1);
				zReconEndActiveStrip = UBoxZoffSet - BWidth*FullStripB - ( BWidth - 0.1 );	
				
				xNextFStrip = UBoxXoffSet;
				yNextFStrip = UBoxYoffSet - FWidth*(FullStripF+1) - FWidth*0.5;
				zNextFStrip = UBoxZoffSet - BWidth*FullStripB - BWidth*0.5;
				
				xNextBStrip = UBoxXoffSet;
				yNextBStrip = UBoxYoffSet - FWidth*FullStripF - FWidth*0.5;
				zNextBStrip = UBoxZoffSet - BWidth*(FullStripB+1) - BWidth*0.5;	
				
				xNextFBStrip = UBoxXoffSet;
				yNextFBStrip = UBoxYoffSet - FWidth*(FullStripF+1) - FWidth*0.5;
				zNextFBStrip = UBoxZoffSet - BWidth*(FullStripB+1) - BWidth*0.5;
				
				if( frontStrip == -1 )
				{
					if( y - yReconEndActiveStrip > 0 )
						InterStripDifferenceF = y - yReconEndActiveStrip;
					else
						InterStripDifferenceF = yReconEndActiveStrip - y;					
				}
				if( backStrip == -1 )
					InterStripDifferenceB = zReconEndActiveStrip - z;									
			}												
	
			G4double Mag = sqrt( xRecon*xRecon + yRecon*yRecon + zRecon*zRecon );
			BoxTheta = acos( zRecon/Mag );
			if( xRecon > 0.0 && yRecon > 0.0 )
				BoxPhi = atan( yRecon / xRecon );
			else if( xRecon < 0.0 && yRecon > 0.0 )
				BoxPhi = atan( yRecon / xRecon ) + pi;
			else if( xRecon < 0.0 && yRecon < 0.0 )
				BoxPhi = atan( yRecon / xRecon ) + pi;					
			else if( xRecon > 0.0 && yRecon < 0.0 )
				BoxPhi = atan( yRecon / xRecon ) + 2*pi;
			
			G4double MagFull = sqrt( xReconFull*xReconFull + yReconFull*yReconFull + zReconFull*zReconFull );
			G4double BoxThetaFull = acos( zReconFull/MagFull );
			G4double BoxPhiFull;
			if( xReconFull > 0.0 && yReconFull > 0.0 )
				BoxPhiFull = atan( yReconFull / xReconFull );
			else if( xReconFull < 0.0 && yReconFull > 0.0 )
				BoxPhiFull = atan( yReconFull / xReconFull ) + pi;
			else if( xReconFull < 0.0 && yReconFull < 0.0 )
				BoxPhiFull = atan( yReconFull / xReconFull ) + pi;					
			else if( xReconFull > 0.0 && yReconFull < 0.0 )
				BoxPhiFull = atan( yReconFull / xReconFull ) + 2*pi;				
				
			G4double MagFullNextF = sqrt( xNextFStrip*xNextFStrip + yNextFStrip*yNextFStrip + zNextFStrip*zNextFStrip );
			G4double BoxThetaNextF = acos( zNextFStrip/MagFullNextF );
			G4double BoxPhiNextF=-1;
			if( xNextFStrip > 0.0 && yNextFStrip > 0.0 )
				BoxPhiNextF = atan( yNextFStrip / xNextFStrip );
			else if( xNextFStrip < 0.0 && yNextFStrip > 0.0 )
				BoxPhiNextF = atan( yNextFStrip / xNextFStrip ) + pi;
			else if( xNextFStrip < 0.0 && yNextFStrip < 0.0 )
				BoxPhiNextF = atan( yNextFStrip / xNextFStrip ) + pi;					
			else if( xNextFStrip > 0.0 && yNextFStrip < 0.0 )
				BoxPhiNextF = atan( yNextFStrip / xNextFStrip ) + 2*pi;				
				
			G4double MagFullNextB = sqrt( xNextBStrip*xNextBStrip + yNextBStrip*yNextBStrip + zNextBStrip*zNextBStrip );
			G4double BoxThetaNextB = acos( zNextBStrip/MagFullNextB );
			G4double BoxPhiNextB=-1;
			if( xNextBStrip > 0.0 && yNextBStrip > 0.0 )
				BoxPhiNextB = atan( yNextBStrip / xNextBStrip );
			else if( xNextBStrip < 0.0 && yNextBStrip > 0.0 )
				BoxPhiNextB = atan( yNextBStrip / xNextBStrip ) + pi;
			else if( xNextBStrip < 0.0 && yNextBStrip < 0.0 )
				BoxPhiNextB = atan( yNextBStrip / xNextBStrip ) + pi;					
			else if( xNextBStrip > 0.0 && yNextBStrip < 0.0 )
				BoxPhiNextB = atan( yNextBStrip / xNextBStrip ) + 2*pi;					
				
			G4double MagFullNextFB = sqrt( xNextFBStrip*xNextFBStrip + yNextFBStrip*yNextFBStrip + zNextFBStrip*zNextFBStrip );
			G4double BoxThetaNextFB = acos( zNextFBStrip/MagFullNextFB );
			G4double BoxPhiNextFB=-1;
			if( xNextFBStrip > 0.0 && yNextFBStrip > 0.0 )
				BoxPhiNextFB = atan( yNextFBStrip / xNextFBStrip );
			else if( xNextFBStrip < 0.0 && yNextFBStrip > 0.0 )
				BoxPhiNextFB = atan( yNextFBStrip / xNextFBStrip ) + pi;
			else if( xNextFBStrip < 0.0 && yNextFBStrip < 0.0 )
				BoxPhiNextFB = atan( yNextFBStrip / xNextFBStrip ) + pi;					
			else if( xNextFBStrip > 0.0 && yNextFBStrip < 0.0 )
				BoxPhiNextFB = atan( yNextFBStrip / xNextFBStrip ) + 2*pi;
			
			//--- fill the box theta phi from the full front back strip hit
			if( AlphaFillCount == 1 &&  LightParticle == 0 )
			{	
				mAlphaTheta = BoxThetaFull;
			}						
			
			G4double energyDep = 0;
			G4RandGauss::shoot();
			energyDep = edep + G4RandGauss::shoot()*mDiBxResolution;
			if( energyDep < 0 )
				energyDep = 0;				
			
			G4double FrontRatioInter = InterStripDifferenceF / 0.1;
			G4double BackRatioInter = InterStripDifferenceB / 0.1;		
			
			G4double FrontPercentSame = 1-FrontRatioInter;
			G4double FrontPercentNext = FrontRatioInter;
			
			G4double BackPercentSame = 1-BackRatioInter;
			G4double BackPercentNext = BackRatioInter;

			//--- if the event deposits energy in the pixel
		   if( frontStrip > -1 && backStrip > -1 )
		   {		   			   
		   	FStrip.push_back( frontStrip );
		   	BStrip.push_back( backStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 2 );
		   	
		   	DetX.push_back( xRecon );
		   	DetY.push_back( yRecon );
		   	DetZ.push_back( zRecon );
		   	
		   	DetTheta.push_back( BoxTheta );
		   	DetPhi.push_back( BoxPhi );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   }
		   else if( ( frontStrip == -1 && FullStripF > -1 ) && backStrip > -1 )
		   {		   		
		   	if( FrontPercentSame > 1 || FrontPercentNext > 1 )
		   	{
		   		cout << "FrontPercentSame: " << FrontPercentSame << " FrontPercentNext " << FrontPercentNext << " FrontStrip " << frontStrip 
		   		     << " FrontStripFull " << FullStripF << endl;
		   	}		
		   	//--- first hit in front
		   	FStrip.push_back( FullStripF );
		   	BStrip.push_back( backStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 2 );		   	
		   	
		   	DetX.push_back( xReconFull );
		   	DetY.push_back( yReconFull );
		   	DetZ.push_back( zReconFull );
		   	
		   	DetTheta.push_back( BoxThetaFull );
		   	DetPhi.push_back( BoxPhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep*FrontPercentSame );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		
		   		
		   	//--- first hit in front
		   	FStrip.push_back( FullStripF+1 );
		   	BStrip.push_back( backStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 2 );		   	
		   	
		   	DetX.push_back( xNextFStrip );
		   	DetY.push_back( yNextFStrip );
		   	DetZ.push_back( zNextFStrip );
		   	
		   	DetTheta.push_back( BoxThetaNextF );
		   	DetPhi.push_back( BoxPhiNextF );
		   	
		   	dE_Energy_F.push_back( energyDep*FrontPercentNext );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );			   		   
		   }  
		   else if( frontStrip > -1 && backStrip == -1 && FullStripB > -1)
		   {   
		   	//--- first hit in front
		   	FStrip.push_back( frontStrip );
		   	BStrip.push_back( FullStripB );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 2 );		   	
		   	
		   	DetX.push_back( xReconFull );
		   	DetY.push_back( yReconFull );
		   	DetZ.push_back( zReconFull );
		   	
		   	DetTheta.push_back( BoxThetaFull );
		   	DetPhi.push_back( BoxPhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep*BackPercentSame );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		
		   		
		   	//--- first hit in front
		   	FStrip.push_back( frontStrip );
		   	BStrip.push_back( FullStripB+1 );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 2 );		   	
		   	
		   	DetX.push_back( xNextBStrip );
		   	DetY.push_back( yNextBStrip );
		   	DetZ.push_back( zNextBStrip );
		   	
		   	DetTheta.push_back( BoxThetaNextB );
		   	DetPhi.push_back( BoxPhiNextB );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep*BackPercentNext );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );			   		   
		   }  		   
		   else if( frontStrip == -1 && FullStripF > -1 && backStrip == -1 && FullStripB > -1)
		   {
		   	//--- first hit in front
		   	FStrip.push_back( FullStripF );
		   	BStrip.push_back( FullStripB );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 2 );		   	
		   	
		   	DetX.push_back( xReconFull );
		   	DetY.push_back( yReconFull );
		   	DetZ.push_back( zReconFull );
		   	
		   	DetTheta.push_back( BoxThetaFull );
		   	DetPhi.push_back( BoxPhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep*FrontPercentSame );
		   	dE_Energy_B.push_back( energyDep*BackPercentSame );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		
		   		
		   	//--- first hit in front
		   	FStrip.push_back( FullStripF+1 );
		   	BStrip.push_back( FullStripB+1 );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 2 );		   	
		   	
		   	DetX.push_back( xNextFBStrip );
		   	DetY.push_back( yNextFBStrip );
		   	DetZ.push_back( zNextFBStrip );
		   	
		   	DetTheta.push_back( BoxThetaNextFB );
		   	DetPhi.push_back( BoxPhiNextFB );
		   	
		   	dE_Energy_F.push_back( energyDep*FrontPercentNext );
		   	dE_Energy_B.push_back( energyDep*BackPercentNext );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );			   		   
		   } 		   
		}
		//--- downstream UCD
		if( !(strncmp("UmCD",volumeName,4)) && ( LightParticle == 0 || HeavyParticle == 0) )
		{
			G4int DetID = -1;
			
			//--- Set the detID
			if( !(strncmp("UmCD1",volumeName,5)) )
				DetID = 1;
			if( !(strncmp("UmCD2",volumeName,5)) )
				DetID = 2;
			if( !(strncmp("UmCD3",volumeName,5)) )
				DetID = 3;
			if( !(strncmp("UmCD4",volumeName,5)) )
				DetID = 4;		
				
			//--- Declare the geometric paramters of the experimental setup and reconstruct the distance from the center of the detector 
			//--- hole to the Si Strip(R)
			G4double z = -66.5;  //--- units of mm
			G4double CDRadFirstStrip = 9.0;
			G4double CDRwidth = 2.0;
			G4double CDRadInterStrip = 0.1;
			G4double R = tan( RealTheta )*z;			
			
			//--- the strip number taking into account the interstrip distance, this number can stay -1 if a hit is detected within an interstrip area
			G4int FrontStrip = -1;
			G4int BackStrip = -1;
			
			//--- the strip number not taking into account the interstrip distance, should never be -1
			G4int FrontStripFull = -1;
			G4int BackStripFull = -1;	
			
			//--- reconstruct the theta from the front strip.  There is a dead layer between strips of 0.1mm and a few events fall into this area
			for( int j =0; j < 16; j++)
			{
				if( R > (CDRadFirstStrip + CDRwidth*(j) ) && R <  (CDRadFirstStrip + (CDRwidth)*(j+1) - CDRadInterStrip ) )
				{
					FrontStrip = j;
				}
				if( R > (CDRadFirstStrip + CDRwidth*(j) ) && R <  (CDRadFirstStrip + (CDRwidth)*(j+1) ) )
				{
					FrontStripFull = j;
				}				
			}

			G4double Cir = 2*pi*R*(3.4/360.);	
			G4double CirActive = Cir-0.1;
			G4double ActivePhi = 3.4*(CirActive/Cir)*pi/180;
			G4double CDDelPhi = 3.4*pi/180;
			
			//--- reconstruct the back strip detection for completeness
			//--- I need to figure out what the span of phi would be for the back strip pitch of 0.1mm
			for( int j =0; j < 24; j++)
			{
				if( RealPhi > ( (CDDelPhi)*(j) + pi/2.0*(DetID-1) ) 
				    && RealPhi < ( (CDDelPhi*(j+1)) - (CDDelPhi-ActivePhi) + pi/2.0*(DetID-1) ) )								    
				{
					BackStrip = j;
				}
				if( RealPhi > ( (CDDelPhi)*(j) + pi/2.0*(DetID-1) ) 
				    && RealPhi < ( (CDDelPhi*(j+1)) + pi/2.0*(DetID-1) ) )								    
				{
					BackStripFull = j;
				}				
			}
			
			G4double RmidFull = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*0.5 ) + (float)(FrontStripFull) * (CDRwidth);	
			G4double Rmid = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*0.5 ) + (float)(FrontStrip) * (CDRwidth);
			G4double RRand = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*G4UniformRand() ) + (float)(FrontStrip) * (CDRwidth);
			G4double RRandFull = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*G4UniformRand()) + (float)(FrontStripFull) * (CDRwidth);				
			G4double RRandFullP1 = CDRadFirstStrip + ( ( CDRwidth-CDRadInterStrip)*G4UniformRand()) + (float)(FrontStripFull+1) * (CDRwidth);
			
			G4double ThetaMidFull = atan(RmidFull/z);
			G4double ThetaMid = atan(Rmid/z);
			G4double ThetaRand = atan(RRand/z);	
			G4double ThetaRandFull = atan(RRandFull/z);	
			G4double ThetaRandFullP1 = atan(RRandFullP1/z);	
			if( ThetaMidFull < 0 )
				ThetaMidFull += pi;
			if( ThetaMid < 0 )
				ThetaMid += pi;
			if( ThetaRand < 0 )
				ThetaRand += pi;
			if( ThetaRandFull < 0 )
				ThetaRandFull += pi;
			if( ThetaRandFullP1 < 0 )
				ThetaRandFullP1 += pi;																
						
			//mHe6Theta = ThetaMidFull;								
						
			G4double PhiFull = CDDelPhi*(23-BackStripFull) + pi/2*(DetID-1);
			G4double PhiFullP1 = CDDelPhi*(BackStripFull+1) + pi/2*(DetID-1);			
			G4double Phi = CDDelPhi*BackStrip + pi/2*(DetID-1);	

			
			G4double energyDep = 0;
		   G4RandGauss::shoot();
		   energyDep = edep + G4RandGauss::shoot()*mDiCDResolution;
		   if( energyDep < 0 )
		   	energyDep = 0;	
		   
		   //--- make the percentage calculations for the distribution of energy for the front or back
		   //--- theta angle stuff
			G4double BotStripR = CDRadFirstStrip + ( CDRwidth-CDRadInterStrip ) + CDRwidth*FrontStripFull;
			G4double TopStripR = CDRadFirstStrip + ( CDRwidth ) + CDRwidth*FrontStripFull;
			
			//--- how close the point is to the current strip
			G4double BotStripDistance = R - BotStripR;

			//--- the percentage of energy shared to the bottom strip
			G4double BotStripPerEnergy = BotStripDistance/CDRadInterStrip;
			
			G4double BotStripEShare = 1 - BotStripPerEnergy;
			G4double TopStripEShare = BotStripPerEnergy;
			
			//--- Make percentage calculations for the phi angle
			G4double phiToInterstrip = CDDelPhi*BackStripFull + ActivePhi + pi/2*(DetID-1);
			//--- angular amount from the interstrip angular range to the point of detection in the interstrip region.
			G4double PhiDifference = RealPhi - phiToInterstrip;
			G4double AngularCoverageInterstrip = 3.4*pi/180 - ActivePhi;
			
			G4double BackStripPercentCov = PhiDifference / AngularCoverageInterstrip;
			
			G4double LeftStripEShare = 1 - BackStripPercentCov;
			G4double RightStripEShare = BackStripPercentCov;
			
			//--- if the event deposits energy in the pixel
		   if( FrontStrip > -1 && BackStrip > -1 )
		   {			   
		   	FStrip.push_back( FrontStrip );
		   	BStrip.push_back( BackStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 3 );
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRand );
		   	DetPhi.push_back( Phi );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   }
			//--- if the event deposits energy in the front interstrip and a back strip		   
		   else if( ( FrontStrip == -1 && FrontStripFull > -1 ) && BackStrip > -1 )
		   {
		   	if( BotStripEShare > 1 || TopStripEShare > 1 )
		   	{
		   		cout << "BotStripEShare: " << BotStripEShare << " TopStripEShare " << TopStripEShare << " FrontStrip " << FrontStrip << " FrontStripFull " << FrontStripFull << endl;
		   		cout << " phiToInterstrip " << phiToInterstrip << " PhiDifference " << PhiDifference << " RealPhi " << RealPhi << endl;
		   	}		   
		   
		   	//--- first energy share event
		   	FStrip.push_back( FrontStripFull );
		   	BStrip.push_back( BackStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 3 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRandFull );
		   	DetPhi.push_back( Phi );
		   	
		   	dE_Energy_F.push_back( energyDep*BotStripEShare );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   	
		   	//--- second energy share event
		   	FStrip.push_back( FrontStripFull+1 );
		   	BStrip.push_back( BackStrip );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 3 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRandFullP1 );
		   	DetPhi.push_back( Phi );
		   	
		   	dE_Energy_F.push_back( energyDep*TopStripEShare );
		   	dE_Energy_B.push_back( energyDep );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		   	
		   }
		   //--- if the event deposits energyin the front strip and the back interstrip distance
		   else if( FrontStrip > -1 && (BackStrip == -1 && BackStripFull > -1 ) )
		   {
		   	if( LeftStripEShare > 1 || RightStripEShare > 1 )
		   	{
		   		cout << "LeftStripEShare: " << LeftStripEShare << " RightStripEShare " << RightStripEShare << " BackStrip " << BackStrip << " BackStripFull " << BackStripFull << endl;
		   		cout << " phiToInterstrip " << phiToInterstrip << " PhiDifference " << PhiDifference << " RealPhi " << RealPhi << endl;
		   	}
  	
		   	//--- first energy share event
		   	FStrip.push_back( FrontStrip );
		   	BStrip.push_back( BackStripFull );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 3 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRand );
		   	DetPhi.push_back( PhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep*LeftStripEShare );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   	
		   	//--- second energy share event
		   	FStrip.push_back( FrontStrip );
		   	BStrip.push_back( BackStripFull+1 );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 3 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRand );
		   	DetPhi.push_back( PhiFullP1 );
		   	
		   	dE_Energy_F.push_back( energyDep );
		   	dE_Energy_B.push_back( energyDep*RightStripEShare );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		   	
		   }
			//--- if the event deposits energy in the interstrip area in both front and back
		   else if( (FrontStrip == -1 && FrontStripFull > -1 ) && (BackStrip == -1 && BackStripFull > -1 ) )
		   {
		   	if( BotStripEShare > 1 || TopStripEShare > 1 )
		   	{
		   		cout << "BotStripEShare: " << BotStripEShare << " TopStripEShare " << TopStripEShare << " FrontStrip " << FrontStrip << " FrontStripFull " << FrontStripFull << endl;
		   		cout << " phiToInterstrip " << phiToInterstrip << " PhiDifference " << PhiDifference << " RealPhi " << RealPhi << endl;
		   	}	
		   	if( LeftStripEShare > 1 || RightStripEShare > 1 )
		   	{
		   		cout << "LeftStripEShare: " << LeftStripEShare << " RightStripEShare " << RightStripEShare << " BackStrip " << BackStrip << " BackStripFull " << BackStripFull << endl;
		   		cout << " phiToInterstrip " << phiToInterstrip << " PhiDifference " << PhiDifference << " RealPhi " << RealPhi << endl;
		   	}		   			   
		   	//--- first energy share event
		   	FStrip.push_back( FrontStripFull );
		   	BStrip.push_back( BackStripFull );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 3 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRandFull );
		   	DetPhi.push_back( PhiFull );
		   	
		   	dE_Energy_F.push_back( energyDep*BotStripEShare );
		   	dE_Energy_B.push_back( energyDep*LeftStripEShare );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );
		   	
		   	//--- second energy share event
		   	FStrip.push_back( FrontStripFull+1 );
		   	BStrip.push_back( BackStripFull+1 );
		   	DetectorID.push_back( DetID );
		   	DetectorSector.push_back( 3 );		   	
		   	
		   	DetX.push_back( -1 );
		   	DetY.push_back( -1 );
		   	DetZ.push_back( -1 );
		   	
		   	DetTheta.push_back( ThetaRandFullP1 );
		   	DetPhi.push_back( PhiFullP1 );
		   	
		   	dE_Energy_F.push_back( energyDep*TopStripEShare );
		   	dE_Energy_B.push_back( energyDep*RightStripEShare );		   	
		   	E_Energy.push_back( -1 );
		   	
		   	if( LightParticle == 0 )
		   		ParticleID.push_back( 1 );
		   	else if( HeavyParticle == 0 )
		   		ParticleID.push_back( 0 );		   	
		   }				   	      
		}						
	}
	
	//--- begin the actual tree data filling
	vector<float> EDetEnergy;
	vector<int>   EDetID;
	
	int VectorSize = dE_Energy_F.size();
	
	//--- declare the vectors to be filled 
	//--- vector<G4int>		FStrip, BStrip, DetectorID, ParticleID, DetectorSector;
	//--- vector<G4double>	dE_Energy_F, dE_Energy_B, E_Energy, DetTheta, DetPhi, DetX, DetY, DetZ;	
	
	//--- Fill the energy deposition within the original arrays so that when I populate events in the TTree I only compare strips, detIDs, and sectors
	vector<G4double>	dE_Energy_FTemp, dE_Energy_BTemp;
	dE_Energy_FTemp = dE_Energy_F;
	dE_Energy_BTemp = dE_Energy_B;
	for( int i = 0; i < VectorSize; i++)
	{
	//--- cycle through the dE > 0 events and sum the energies approiately within the arrays
	for( int j = 0; j <  i; j++)
	{
		if( DetectorSector[i] == DetectorSector[j] )
		{
			if( DetectorID[i] == DetectorID[j] )
			{
				if( FStrip[i] == FStrip[j] )
				{
					if( dE_Energy_F[i] != dE_Energy_F[j] )
					{
						dE_Energy_FTemp[j] = dE_Energy_F[i] + dE_Energy_F[j];
						dE_Energy_FTemp[i] = dE_Energy_F[i] + dE_Energy_F[j];						
					}
				}
				if( BStrip[i] == BStrip[j] )
				{
					if( dE_Energy_B[i] != dE_Energy_B[j] )
					{
						dE_Energy_BTemp[j] = dE_Energy_B[i] + dE_Energy_B[j];
						dE_Energy_BTemp[i] = dE_Energy_B[i] + dE_Energy_B[j];							
					}				
				}
			}
		}
	}
	}
	
	//--- set the TTree values
	for( int i = 0; i < VectorSize; i++)
	{
	//--- set the front and back energies to the temporary vectors populated 
	//dE_Energy_F[i] = dE_Energy_FTemp[i];		
	//dE_Energy_B[i] = dE_Energy_BTemp[i];			
	//--- if the events in the dE are detected
	if( dE_Energy_F[i] > 0.0 )
	{
		//--- if a strip is detected and the events is from a light or heavy reaction product
		if( (FStrip[i] > -1 || BStrip[i] > -1) && (ParticleID[i] == 0 || ParticleID[i] == 1) )
		{
			//--- if no hits have been populated then add the event to the array
			if( mHits == 0 )
			{
				mHits++;
				mFrontStrip[mHits-1] = FStrip[i];
				mBackStrip[mHits-1] = BStrip[i];	
				mDetSec[mHits-1] = DetectorSector[i];				
				mDetID[mHits-1] = DetectorID[i];	
				mSi_dE_B[mHits-1] = dE_Energy_BTemp[i];	
				mSi_dE_F[mHits-1] = dE_Energy_FTemp[i];						
				m6HeParticle[mHits-1] = ParticleID[i];
				mDetectorTheta[mHits-1] = DetTheta[i];
				mDetectorPhi[mHits-1] = DetPhi[i];
				mDetectorX[mHits-1] = DetX[i];
				mDetectorY[mHits-1] = DetY[i];
				mDetectorZ[mHits-1] = DetZ[i];
			}
			//--- if 1 or more events is populated then compare the current event to all other events populated to determine a new event
			else if( mHits > 0 )
			{
				G4bool NewEvent = false;
				for( int j = 0; j < i; j++ )
				{
					if( DetectorSector[i] != DetectorSector[j] )
					{
						NewEvent = true;
					}
					else if( DetectorSector[i] == DetectorSector[j] )
					{
						if( DetectorID[i] != DetectorID[j] )
						{
							NewEvent = true;
						}
						else if( DetectorID[i] == DetectorID[j] )
						{
							if( FStrip[i] != FStrip[j] || BStrip[i] != BStrip[j] )
							{
								NewEvent = true;
							}
							else if( FStrip[i] == FStrip[j] && BStrip[i] == BStrip[j] )
							{
								NewEvent = false;
								j = i;
							}
						}
					}
				}
				if( NewEvent )
				{
					mHits++;
					mFrontStrip[mHits-1] = FStrip[i];
					mBackStrip[mHits-1] = BStrip[i];	
					mDetSec[mHits-1] = DetectorSector[i];				
					mDetID[mHits-1] = DetectorID[i];	
					mSi_dE_B[mHits-1] = dE_Energy_BTemp[i];	
					mSi_dE_F[mHits-1] = dE_Energy_FTemp[i];						
					m6HeParticle[mHits-1] = ParticleID[i];
					mDetectorTheta[mHits-1] = DetTheta[i];
					mDetectorPhi[mHits-1] = DetPhi[i];
					mDetectorX[mHits-1] = DetX[i];
					mDetectorY[mHits-1] = DetY[i];
					mDetectorZ[mHits-1] = DetZ[i];				
				}
			}
		} 		
	}
	
	//--- if there is a hit in the E detector
	if( E_Energy[i] > 0 && (ParticleID[i] == 0 || ParticleID[i] == 1) )
	{
		if( EDetEnergy.size() == 0 )
		{
			EDetEnergy.push_back(E_Energy[i]);
			EDetID.push_back( DetectorID[i] );
		}
		else if( EDetEnergy.size() > 0 )
		{
			for(int k =0; k < EDetEnergy.size(); k++ )
			{
				if( E_Energy[i] != EDetEnergy[k] && DetectorID[i] == EDetID[k] )
				{
					EDetEnergy[k] += E_Energy[i];
				}
				else if( E_Energy[i] != EDetEnergy[k] && DetectorID[i] != EDetID[k] )
				{
					EDetEnergy.push_back(E_Energy[i]);
					EDetID.push_back( DetectorID[i] );	
					k = EDetEnergy.size();															
				}
			}
		}
	}				
	}

	//--- fill the array for the E detector from the vector populated above
	int VecCount = 0;
	for(int i = 0; i < mHits; i++)
	{
		if( EDetEnergy.size()>0 && VecCount < EDetEnergy.size() )
		{
		if( mSi_dE_F[i] > 0 && mDetID[i] == EDetID[VecCount])
		{
			mSi_E[i] = EDetEnergy[VecCount];
			VecCount++;
		}
		}
	}  	
	
	//--- add E detector energy for same detector events
	for(int i =0; i < mHits-1; i++)
	{
		if( mDetID[i] == mDetID[i+1] &&  mSi_E[i] != mSi_E[i+1] )
		//if(false)
		{
			if( mSi_E[i] > 0 )
				mSi_E[i+1] = mSi_E[i];
			else if( mSi_E[i+1] > 0 )
				mSi_E[i] = mSi_E[i+1];				
		} 
	}		
	
	//if( mDetectorTheta[0] > 0.5 &&  mDetectorTheta[0] < 1.5 && true )
	//if( mDetectorTheta[0] > 0.0 &&  mDetectorTheta[0] < 0.5 && true )
	int count = 0;
	for( int l = 0; l < mHits; l++ )
	{
	if(false)
	//if( mSi_dE_F[l] > 40 && mDetectorTheta[l] > 0.5 )		
	//if( mSi_dE_B[l] > 12.31 && mSi_dE_B[l] < 13.57 && mSi_dE_F[l] > 23.91 && mDetectorTheta[l] > 0.5 )	
	//if( dE_Energy_F.size() >= 4)
	{	
	for(int i =0; i < mHits; i++ )
	{
		count++;
		cout << "i " << i << " mDetectorTheta: " << mDetectorTheta[i]*180/pi << " mSi_dE_F: " << mSi_dE_F[i] << " mSi_dE_B: " << mSi_dE_B[i] << " mSi_E[i] " << mSi_E[i] << " m6HeParticle[i] " 
		     << m6HeParticle[i] << endl;
	}	
	for(int j = 0; j < dE_Energy_F.size(); j++)
	{
		count++;
 		cout << " j " << j << " dE_Energy_F " << dE_Energy_F[j] << " dE_Energy_B " << dE_Energy_B[j] << " E_Energy[j] " << E_Energy[j] << " DetTheta " 
		     << DetTheta[j]*180/pi << " FStrip " << FStrip[j] << " BStrip " << BStrip[j] << " ParticleID " << ParticleID[j] << " DetectorID " 
		     << DetectorID[j] << " size " << size << endl;
	}
	for(int j = 0; j < dE_Energy_FTemp.size(); j++)
	{
 		cout << "   j " << j << " dE_Energy_FTemp " << dE_Energy_FTemp[j] << " dE_Energy_BTemp " << dE_Energy_BTemp[j] << endl;
	}	
	for(int k =0; k < size; k++)
	{
		//--- Set the variables that we would detect, stuff like exact position and particle name are for debuggin purposes
		//--- these variables come from the sensitive detectors
		G4double edep = Hits->at(k)->GetEdep();
  		G4ThreeVector Position = Hits->at(k)->GetPos();
  		G4double x = Position.getX();
  		G4double y = Position.getY();
  		G4double z = Position.getZ();  		
  		G4String particleName = Hits->at(k)->GetParticleName();  		
  		G4String volumeName = Hits->at(k)->GetVolumeName();   			
		G4double RealTheta = Position.getTheta();
		G4double RealPhi = Position.getPhi();
		cout << "        E: " << edep << " theta: " <<  RealTheta*180/pi << " phi " << RealPhi*180/pi << " volumeName: " 
		     << volumeName  << " particleName " << particleName << endl;
	}	
	}
	}
		

	mReconDataTree->Fill();
	for(int j =0; j < mHits; j++ )
	{
		mDetID[j] = -1;
  		mDetSec[j] = -1;			
		mFrontStrip[j] = -1;
		mBackStrip[j] = -1;  	
		mDetectorTheta[j] = -1;  							
		mDetectorPhi[j] = -1;
		mDetectorX[j] = 0;  		
		mDetectorY[j] = 0;  		
  		mDetectorZ[j] = 0;   
		mSi_dE_F[j] = 0;
		mSi_dE_B[j] = 0;
		mSi_E[j] = 0;
  		m6HeParticle[j] = -1;  				
	}
	mHits = 0;	
	mAlphaTheta = -1.0;
	mAlphaEnergy = -1.0;					
}

void EventAction::mSetActualDataParameters( std::vector<SiliconHit*> *Hits )
{
	G4int size = Hits->size();
	G4double pi = 4.0*atan(1.0);	
	
	for(int i =0; i < size; i++)
	{
  		G4double edep = Hits->at(i)->GetEdep();
  		G4ThreeVector Position = Hits->at(i)->GetPos();
  		G4double theta = Position.getTheta();
  		G4double phi = Position.getPhi();
  		if( phi < 0 )
  			phi += 2*pi;  		
  		G4double x = Position.getX();
  		G4double y = Position.getY();
  		G4String particleName = Hits->at(i)->GetParticleName();
  		G4String volumeName = Hits->at(i)->GetVolumeName(); 
  		
  		//--- fill the variables for 12C   		
  		if( !(strncmp("C14",particleName,3)) )
  		{
  			mTotalEDep12C += edep;
  			
  			if( !(strncmp("Di",volumeName,2)) )
  			{
  				mdEEnergy12C += edep;   			
   			mActualTheta12C = theta;   		
  			}
  			else if( !(strncmp("Do",volumeName,2)) )
  			{
  				mEEnergy12C += edep;
  			}
  			else if( !(strncmp("Um",volumeName,2)) )
  			{
				mActualTheta12C = theta;	   			   			
  			}   			
  		}
  		
  		//--- fill the variables for 6He
  		if( !(strncmp("alp",particleName,3)) )
  		{
  			mTotalEDep6He += edep;
  			
  			if( !(strncmp("Di",volumeName,2)) ) 
  			{  			
  				mdEEnergy4He += edep;   			
				mActualTheta6He = theta;
  			}
  			else if( !(strncmp("Do",volumeName,2)) )
  			{
  				mEEnergy4He += edep;	
  			}
  			else if( !(strncmp("Um",volumeName,2)) )
  			{
				mActualTheta6He = theta;	   			   			
  			}
  		}
         
      if( i == size - 1 )
      {
         //--- Fill the histogram at the end of the hits in the Sensitive detector(Si) and reset the variables
        	mActualDataTree->Fill();  
	         
			mdEEnergy4He = 0;
			mdEEnergy12C = 0;
			mEEnergy4He = 0;
			mEEnergy12C = 0;		         
	                
         mTotalEDep12C = 0;
         mTotalEDep6He = 0;   
         
         mActualTheta6He = 0;
         mActualTheta12C = 0;      	
      }
   }
}

void EventAction::mSetBranches()
{	
	//--- The Branches for the 6He
   mActualDataTree->Branch("dEEnergy4He",&(mdEEnergy4He),"mdEEnergy4He/D");     
   mActualDataTree->Branch("EEnergy4He",&(mEEnergy4He),"mEEnergy4He/D");        
   mActualDataTree->Branch("TotalEDep6He",&(mTotalEDep6He),"mTotalEDep6He/D");  
   mActualDataTree->Branch("ActualTheta6He",&(mActualTheta6He),"mActualTheta6He/D");
   //--- The Branches for the 12C
   mActualDataTree->Branch("dEEnergy12C",&(mdEEnergy12C),"mdEEnergy12C/D");     
   mActualDataTree->Branch("EEnergy12C",&(mEEnergy12C),"mEEnergy12C/D");          
   mActualDataTree->Branch("TotalEDep12C",&(mTotalEDep12C),"mTotalEDep12C/D");  
   mActualDataTree->Branch("ActualTheta12C",&(mActualTheta12C),"mActualTheta12C/D");
   
   //--- The Branches for the data reconstruction
   mReconDataTree->Branch("Reaction",&(mReaction),"mReaction/I");      
   mReconDataTree->Branch("AlphaEnergy",&(mAlphaEnergy),"mAlphaEnergy/D");      
   mReconDataTree->Branch("AlphaTheta",&(mAlphaTheta),"mAlphaTheta/D");     
   mReconDataTree->Branch("AlphaPhi",&(mAlphaPhi),"mAlphaPhi/D");        
   mReconDataTree->Branch("Hits",&(mHits),"mHits/I");        
   mReconDataTree->Branch("DetID",&(mDetID),"mDetID[mHits]/I");     
   mReconDataTree->Branch("FrontStrip",&(mFrontStrip),"mFrontStrip[mHits]/I");
   mReconDataTree->Branch("BackStrip",&(mBackStrip),"mBackStrip[mHits]/I");   
   mReconDataTree->Branch("DetectorTheta",&(mDetectorTheta),"mDetectorTheta[mHits]/D");
   mReconDataTree->Branch("DetectorPhi",&(mDetectorPhi),"mDetectorPhi[mHits]/D");       
   mReconDataTree->Branch("DetectorX",&(mDetectorX),"mDetectorX[mHits]/D");
   mReconDataTree->Branch("DetectorY",&(mDetectorY),"mDetectorY[mHits]/D");
   mReconDataTree->Branch("DetectorZ",&(mDetectorZ),"mDetectorZ[mHits]/D");     
   mReconDataTree->Branch("Si_dE_F",&(mSi_dE_F),"mSi_dE_F[mHits]/D");     
   mReconDataTree->Branch("Si_dE_B",&(mSi_dE_B),"mSi_dE_B[mHits]/D");           
   mReconDataTree->Branch("Si_E",&(mSi_E),"mSi_E[mHits]/D"); 
   mReconDataTree->Branch("Particle6He",&(m6HeParticle),"m6HeParticle[mHits]/I");         
}

