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
  fLayersMap = runAction->fLayersMap;
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event *event) {
  if (volumeBranchNames.size()==0) {
    volumeBranchNames.push_back(G4String("primary"));
    G4int materialIndex = 0;
    for (auto materialNameThicknessPair:fLayersMap) {
      G4String materialName = materialNameThicknessPair.first;
      if (materialName==G4String("vacuum")) continue;
      materialIndex++;
      volumeBranchNames.push_back(G4String(materialName+std::to_string(materialIndex)));
    }
  }

  for (G4String volumeBranchName:this->volumeBranchNames) this->hitEnergies[volumeBranchName] = new G4DataVector();
  this->hitPositions[volumeBranchNames[volumeBranchNames.size()-1]] = new vector<G4ThreeVector>();
  this->hitMomenta[volumeBranchNames[volumeBranchNames.size()-1]] = new vector<G4ThreeVector>();

  this->electrons = new vector<particle>(0);
  this->photons = new vector<particle>(0);

  G4int eventID = event->GetEventID();
  if (eventID%10000 == 0) G4cout << eventID << "/" << runAction->nOfEvents << "\t\t" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event) {
  G4String volumeName;
  G4DataVector *volumeHitEnergies;
  for (auto it=hitEnergies.begin(); it!=hitEnergies.end(); it++) {
    volumeName = it->first;
    volumeHitEnergies = it->second;
    for (G4double energy:*volumeHitEnergies) this->runAction->FillNtuples(volumeName, energy);
  }
  int primaries = this->TransportPhotons()+this->TransportElectrons();
  //int primaries = this->TransportPhotons();
  if (primaries>20) this->runAction->FillNtuples("conversion", primaries/gasIonizationEnergy, primaries);
}

void EventAction::AddHit(G4String volume, G4double energy) {
  this->hitEnergies[volume]->push_back(energy);
}

void EventAction::AddPhoton(G4double energy, G4ThreeVector position, G4ThreeVector momentum) {
  particle photon;
  photon.energy = energy;
  photon.position = position;
  photon.momentum = momentum;
  photons->push_back(photon);
}

int EventAction::TransportPhotons() {
  int primaries = 0;
  for (int i=0; i<photons->size(); i++) {
    primaries += runAction->heedSimulation->TransportPhoton(this,
      photons->at(i).energy,
      photons->at(i).position,
      photons->at(i).momentum
    );
  }
  return primaries;
}

void EventAction::AddElectron(G4double energy, G4ThreeVector position, G4ThreeVector momentum) {
  particle electron;
  electron.energy = energy;
  electron.position = position;
  electron.momentum = momentum;
  electrons->push_back(electron);
}

int EventAction::TransportElectrons() {
  int primaries = 0;
  for (int i=0; i<electrons->size(); i++) {
    primaries += runAction->heedSimulation->TransportElectron(this,
      electrons->at(i).energy,
      electrons->at(i).position,
      electrons->at(i).momentum
    );
  }
  return primaries;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
