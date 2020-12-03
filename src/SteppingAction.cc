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
#include "G4LogicalVolumeStore.hh"

#include <TMath.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction):G4UserSteppingAction() {
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

  if (!windowKaptonVolume || !driftKaptonVolume || !driftCopperVolume) {
    windowKaptonVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("WindowKaptonLogical");
    driftKaptonVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("DriftKaptonLogical");
    driftCopperVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("DriftCopperLogical");
  }
  
  G4LogicalVolume* volume = step->GetPreStepPoint()-> GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  G4LogicalVolume *nextVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  cout << "step " << volume->GetName() << endl;

  if (volume==windowKaptonVolume && particleName==G4String("gamma")) {
    // get spectrum entering detector kapton window:
    if (step->IsFirstStepInVolume()) this->eventAction->AddHit("window", step->GetPreStepPoint()->GetTotalEnergy()*1.e3);
    cout << "window" << endl;
  } else if (volume==driftKaptonVolume && particleName==G4String("gamma")) {
    /*G4cout << "primary " << step->GetTrack()->GetParticleDefinition()->GetParticleName();
    G4cout << " process " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    G4cout << " energy before " << step->GetPreStepPoint()->GetTotalEnergy()*1e3 << " keV";
    G4cout << " energy after " << step->GetPostStepPoint()->GetTotalEnergy()*1e3 << " keV";
    G4cout << " from volume " << step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
    G4cout << " to volume " << step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
    G4cout << G4endl;*/
    // get spectrum entering drift kapton:
    if (step->IsFirstStepInVolume()) this->eventAction->AddHit("driftKapton", step->GetPreStepPoint()->GetTotalEnergy()*1.e3);
    cout << "drift kapton" << endl;
  } else if (volume==driftCopperVolume && particleName==G4String("gamma")) {
      // get spectrum after cathode entering gas:
      if (nextVolume->GetName()==G4String("Envelope")) {
        this->eventAction->AddHit("driftCopper",
          step->GetPostStepPoint()->GetTotalEnergy()*1.e3,
          step->GetPostStepPoint()->GetPosition(),
          step->GetPostStepPoint()->GetMomentumDirection());
      }
      cout << "drift copper" << endl;
  }
  cout << "end step " << volume->GetName() << endl;
  cout << "PRINTING EVENT ACTION" << endl;
  cout << this->eventAction << endl;
  
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

