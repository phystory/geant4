#ifndef HeTube01TrackingAction_h
#define HeTube01TrackingAction_h 1

#include "G4UserTrackingAction.hh"


class HeTube01TrackingAction : public G4UserTrackingAction {

  public:
    HeTube01TrackingAction(){};
    virtual ~HeTube01TrackingAction(){};

    virtual void PreUserTrackingAction(const G4Track*);

};

#endif

