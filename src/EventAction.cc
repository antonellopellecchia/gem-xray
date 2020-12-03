//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include <vector>

#include "EventAction.hh"
#include "RunAction.hh"

#include "G4ThreeVector.hh"
#include "G4String.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction): G4UserEventAction() {
  this->runAction = runAction;
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event *event) {
  for (G4String volume:this->volumes) this->hitEnergies[volume] = new G4DataVector();
  this->hitPositions["driftCopper"] = new vector<G4ThreeVector>();
  this->hitMomenta["driftCopper"] = new vector<G4ThreeVector>();

  G4int eventID = event->GetEventID();
  if (eventID%10000 == 0) G4cout << eventID << "/" << runAction->nOfEvents << "\t\t" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event) {
  cout << "end of event" << endl;
  G4String volumeName;
  G4DataVector *volumeHitEnergies;
  for (auto it=hitEnergies.begin(); it!=hitEnergies.end(); it++) {
    volumeName = it->first;
    volumeHitEnergies = it->second;
    if (volumeName!=G4String("driftCopper"))
      for (G4double energy:*volumeHitEnergies) this->runAction->FillNtuples(volumeName, energy);
    else
      for (int i=0; i<volumeHitEnergies->size(); i++)
	this->runAction->FillNtuples(volumeName, (*volumeHitEnergies)[i], (*hitPositions["driftCopper"])[i], (*hitMomenta["driftCopper"])[i]);
  }
}

void EventAction::AddHit(G4String volume, G4double energy) {
  this->hitEnergies[volume]->push_back(energy);
}

void EventAction::AddHit(G4String volume, G4double energy, G4ThreeVector position, G4ThreeVector momentum) {
  cout << "adding hit " << volume << endl; 
  /*this->hitEnergies[volume]->push_back(energy);
  this->hitPositions[volume]->push_back(position);
  this->hitMomenta[volume]->push_back(momentum);

  if (volume==G4String("driftCopper")) {
    runAction->heedSimulation->TransportPhoton(this, energy, position, momentum);
  }*/
  cout << "added hit " << volume << endl; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
