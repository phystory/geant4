#include "SiliconSD.hh"
#include "SiliconHit.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"

SiliconSD::SiliconSD( G4String name )
:G4VSensitiveDetector(name)
{
	G4String HCname;
	collectionName.insert(HCname="siliconCollection");
}

SiliconSD::~SiliconSD()
{}

void SiliconSD::Initialize( G4HCofThisEvent *HCE )
{
	static int HCID = -1;
	siliconCollection = new SiliconHitsCollection( SensitiveDetectorName, collectionName[0] );
	
	if( HCID < 0 )
	{
		HCID = GetCollectionID(0);
	}
	HCE->AddHitsCollection(HCID, siliconCollection);
}

G4bool SiliconSD::ProcessHits( G4Step *step, G4TouchableHistory* )
{
	G4Track *track = step->GetTrack(); 	
	
	G4double edep = step->GetTotalEnergyDeposit();
   
	if( edep==0.0)
		return false;
	
   G4String particleName = track -> GetDefinition() -> GetParticleName();
	G4String volumeName = track -> GetVolume() -> GetName();
   	
	SiliconHit *newHit = new SiliconHit();
	newHit->SetEdep( edep );
	newHit->SetPos( step->GetPreStepPoint()->GetPosition() );
	newHit->SetParticleName( particleName );
	newHit->SetVolumeName( volumeName );
	siliconCollection->insert( newHit );
	
	return true;
}

void SiliconSD::EndOfEvent( G4HCofThisEvent* )
{}

void SiliconSD::clear()
{}

void SiliconSD::DrawAll()
{}

void SiliconSD::PrintAll()
{}
