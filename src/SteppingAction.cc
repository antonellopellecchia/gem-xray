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
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include <TMath.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction)
  :G4UserSteppingAction(),
   copperVolume(0),
   fr4Volume(0),
   cathodeVolume(0) {
  this->eventAction = eventAction;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Track *track = step->GetTrack();
  G4int trackID = track->GetTrackID();

  if (!copperVolume || !fr4Volume || !cathodeVolume) {
    const DetectorConstruction* detectorConstruction = static_cast<const DetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    copperVolume = detectorConstruction->GetCopper();
    fr4Volume = detectorConstruction->GetFR4();
    cathodeVolume = detectorConstruction->GetCathode();
  }
  
  G4LogicalVolume* volume = step->GetPreStepPoint()-> GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();

  if (volume==fr4Volume && particleName==G4String("gamma") && step->IsFirstStepInVolume()) { // get spectrum after copper tape entering FR4
    this->eventAction->AddHit("fr4", step->GetPreStepPoint()->GetTotalEnergy()*1.e3);
  } else if (volume==cathodeVolume && particleName==G4String("gamma")) {
    /*G4cout << "primary " << step->GetTrack()->GetParticleDefinition()->GetParticleName();
    G4cout << " process " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    G4cout << " energy before " << step->GetPreStepPoint()->GetTotalEnergy()*1e3 << " keV";
    G4cout << " energy after " << step->GetPostStepPoint()->GetTotalEnergy()*1e3 << " keV";
    G4cout << " from volume " << step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
    G4cout << " to volume " << step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
    G4cout << G4endl;*/
    if (step->IsFirstStepInVolume()) // get spectrum after FR4 entering cathode
      this->eventAction->AddHit("cathode", step->GetPreStepPoint()->GetTotalEnergy()*1.e3);
    G4String nextVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
    if (nextVolume==G4String("Envelope")) { // get spectrum after cathode entering gas
      this->eventAction->AddHit("gas",
				      step->GetPostStepPoint()->GetTotalEnergy()*1.e3,
				      step->GetPostStepPoint()->GetPosition(),
				      step->GetPostStepPoint()->GetMomentumDirection());
    }
  }
  
  /*if (trackID <= 1) {
    // process primary track
    if (volume==fCopperVolume) {
      // check if we are in the copper layer
      G4cout << "primary " << step->GetTrack()->GetParticleDefinition()->GetParticleName();
      G4cout << " process " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
      G4cout << " energy before " << step->GetPreStepPoint()->GetTotalEnergy()*1e3 << " keV";
      G4cout << " energy after " << step->GetPostStepPoint()->GetTotalEnergy()*1e3 << " keV";
      G4cout << " from volume " << step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
      G4cout << " to volume " << step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
      G4cout << " pre " << step->GetPreStepPoint()->GetPosition();
      G4cout << " post " << step->GetPostStepPoint()->GetPosition();
      G4cout << G4endl;
      }
  } else {
    // if not primary particle
    G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();
    if (particleName!=G4String("e-")) {
      // skip electrons
      if (volume == fCopperVolume) {
	G4cout << "secondary " << particleName;
	G4cout << " from " << step->GetTrack()->GetCreatorProcess()->GetProcessName();
	G4cout << " process " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
	G4cout << " energy before " << step->GetPreStepPoint()->GetTotalEnergy()*1e3 << " keV";
	G4cout << " energy after " << step->GetPostStepPoint()->GetTotalEnergy()*1e3 << " keV";
	G4cout << " in " << step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
	G4cout << " pre " << step->GetPreStepPoint()->GetPosition();
	G4cout << " post " << step->GetPostStepPoint()->GetPosition();
	G4cout << G4endl;
      }
      }
  }*/

    /*if (particleName!=G4String("e-")) {
      G4cout << " process " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
      G4cout << " energy before " << step->GetPreStepPoint()->GetTotalEnergy()*1e3 << " keV";
      G4cout << " energy after " << step->GetPostStepPoint()->GetTotalEnergy()*1e3 << " keV";
      G4cout << " in " << step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
      G4cout << " pre " << step->GetPreStepPoint()->GetPosition();
      G4cout << " post " << step->GetPostStepPoint()->GetPosition();
      G4cout << G4endl;
      }*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

