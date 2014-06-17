#include "RunAction.hh"

RunAction::RunAction()
{}

RunAction::~RunAction()
{}

void RunAction::BeginOfRunAction(const G4Run* run)
{
   runNumber = run -> GetRunID();
  // G4cout << G4endl << "*****  BEGIN Run Number " << runNumber << "  *****" << G4endl;
}

void RunAction::EndOfRunAction(const G4Run*)
{
   //G4cout << G4endl << "*****  END Run Number " << runNumber << "  *****" << G4endl;
}



