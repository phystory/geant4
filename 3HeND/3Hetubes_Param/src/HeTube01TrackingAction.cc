
#include "HeTube01TrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "MyStat.hh"

extern MyStat *MyStatistic;

void HeTube01TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
 G4ParticleDefinition* particleType = aTrack->GetDefinition();
 G4String partname = particleType->GetParticleName();
 if ((aTrack->GetVolume()->GetName()=="HeTubes_Param_phys")&& (aTrack->GetParentID()==1) && (MyStatistic->in_tube))
 { if (partname=="triton"||partname=="proton") MyStatistic->have_reacted=1;
   if (partname=="triton")
   {
     MyStatistic->have_reacted_DoIt_triton=aTrack->GetTrackID();
     MyStatistic->e_triton=aTrack->GetKineticEnergy();
   }
   if (partname=="proton")
   {
     MyStatistic->have_reacted_DoIt_proton=aTrack->GetTrackID();
     MyStatistic->e_proton=aTrack->GetKineticEnergy();
   }
 }
//  if ((partname!="gamma")&&(partname!="e-"))
//   G4cout << "   Track ID = " << aTrack->GetTrackID() << "   Created  by Track ID  = " << aTrack->GetParentID()<< "  "  << particleType->GetParticleName() << G4endl;

}


