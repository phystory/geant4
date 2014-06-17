#ifndef EventAction_H
#define EventAction_H 1

#include "G4Event.hh"
#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"

#include "SiliconHit.hh"
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include "G4ThreeVector.hh"
#include "G4RandomDirection.hh"
#include "DataPass.hh"

using namespace std;

class EventAction:public G4UserEventAction
{
   public:
      EventAction(struct DataPass *LightInfoPass);
      ~EventAction();
      void BeginOfEventAction(const G4Event* event);
      void EndOfEventAction(const G4Event* evt);
      
  		DataPass *LightData;      

   private:
   	TFile             *moutFile;
   	TTree				   *mActualDataTree;
   	TTree					*mReconDataTree;
   
      G4int             mEventID;
      G4int					siliconCollID;
      
      //--- Set the detector resolution
      G4double				mDiCDResolution;
      G4double 			mDiCD2Resolution;
      G4double				mDoCDResolution;
      G4double				mDiBxResolution;
      G4double 			mDoBxResolution;
      G4double 			mUmBxResolution;
      G4double				mUmCDResolution;
      
      G4double				mThreshHold;
      
      //--- record the acutal values for 6He
      G4double				mdEEnergy4He;
      G4double				mEEnergy4He;
      G4double          mTotalEDep6He;
      G4double          mActualTheta6He;
      
      //--- record the actual values for 12C
      G4double				mdEEnergy12C;
      G4double				mEEnergy12C;
      G4double          mTotalEDep12C;
      G4double          mActualTheta12C;   
      
      //--- reconstruct the detected values to compare to experiment
      G4int					mReaction;
      G4double				mAlphaTheta;
      G4double				mAlphaPhi;      
      G4double 			mAlphaEnergy;
      G4int					mHits;      
      G4int					mDetID[50];
      G4int					mDetSec[50];      
      G4int					mFrontStrip[50];
      G4int					mBackStrip[50];
      G4double				mDetectorTheta[50];   
      G4double				mDetectorPhi[50];         
      G4double				mDetectorX[50];         
      G4double				mDetectorY[50];         
      G4double				mDetectorZ[50];
		G4double 			mSi_dE_F[50];
		G4double 			mSi_dE_B[50];		
		G4double 			mSi_E[50];				
		G4int					m6HeParticle[50];

		G4bool 				DBoxMultiHit[4];
		G4String				DBoxHitVolName;

		void 				 	mSetBranches();
		
		void 					mSetActualDataParameters( std::vector<SiliconHit*> *Hits );		
		void 					mSetReconDataParameters( std::vector<SiliconHit*> *Hits );
};

#endif
