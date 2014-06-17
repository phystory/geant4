// --------------------------------------------------------------
//      GEANT 4 - practice01, main file = mydetsim.cc
// --------------------------------------------------------------

#ifndef MyPhysicsList_h
#define MyPhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

class MyPhysicsList: public G4VUserPhysicsList
{
  public:
    MyPhysicsList();
    ~MyPhysicsList();

  protected:
    // Construct particle and physics process
    void ConstructParticle();
    void ConstructProcess();
    void ConstructMultiScattering();
    void SetCuts();

};

#endif







