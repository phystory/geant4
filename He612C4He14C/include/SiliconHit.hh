#ifndef SiliconHit_h
#define SiliconHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class G4AttDef;

class SiliconHit : public G4VHit
{
	public:
		SiliconHit();
		~SiliconHit();
		SiliconHit( const SiliconHit &right );
		const SiliconHit& operator=(const SiliconHit &right);
		G4int operator==(const SiliconHit &right) const;
		
		inline void *operator new(size_t);
		inline void operator delete(void *aHit);
		
		void Draw();
		const std::map<G4String, G4AttDef>* GetAttDefs() const;
		std::vector<G4AttValue>* CreateAttValues() const;
		void Print();
		
	private:
		G4double 		edep;
		G4ThreeVector 	pos;
		G4String		  	ParticleName;
		G4String			VolumeName;
		static std::map<G4String, G4AttDef> fAttDefs;
		
	public:
		//--- set and get the energy deposited
		inline void SetEdep(G4double de)
			{ edep = de; }
		inline G4double GetEdep()
			{ return edep; }
		//--- set and get the position vector
		inline void SetPos( G4ThreeVector xyz )
			{ pos = xyz; }
		inline G4ThreeVector GetPos()
			{ return pos; }
		//--- set and get the particle name
		inline void SetParticleName( G4String parName )
			{ ParticleName = parName; }				
		inline G4String GetParticleName()
			{ return ParticleName; }
		//--- set and get the detector name
		inline void SetVolumeName( G4String volName )
			{ VolumeName = volName; }				
		inline G4String GetVolumeName()
			{ return VolumeName; }			
};

typedef G4THitsCollection<SiliconHit> SiliconHitsCollection;

extern G4Allocator<SiliconHit> SiliconHitAllocator;

inline void* SiliconHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) SiliconHitAllocator.MallocSingle();
  return aHit;
}

inline void SiliconHit::operator delete(void *aHit)
{
  SiliconHitAllocator.FreeSingle((SiliconHit*) aHit);
}

#endif

