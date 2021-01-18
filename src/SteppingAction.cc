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
  fLayersMap = eventAction->fLayersMap;

  //volumeBranchNames["WindowKaptonLogical"] = "window";
  //volumeBranchNames["WindowCopperLogical"] = "copper1";
  //volumeBranchNames["DriftKaptonLogical"] = "driftKapton";
  //volumeBranchNames["DriftFr4Logical"] = "driftFr4";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step) {
  G4Track *track = step->GetTrack();
  G4int trackID = track->GetTrackID();

  if (track->GetCurrentStepNumber()==1 and trackID==1) eventAction->AddHit("primary", track->GetVertexKineticEnergy()*1e3);

  //if (!windowKaptonVolume || !driftKaptonVolume || !driftCopperVolume) {
  if (volumesBeforeDrift.size()==0) {
    G4int materialIndex = 0;
    for (auto materialNameThicknessPair:fLayersMap) {
      G4String materialName = materialNameThicknessPair.first;
      if (materialName==G4String("vacuum")) continue;
      materialIndex++;
      G4String logicalName = G4String("Logical")+materialName+std::to_string(materialIndex);
      G4LogicalVolume *volume = G4LogicalVolumeStore::GetInstance()->GetVolume(logicalName);
      //cout << volume << " " << (volume==NULL) << endl;
      volumesBeforeDrift.push_back(volume);
      volumeBranchNames.push_back(G4String(materialName+std::to_string(materialIndex)));
    }
    //windowKaptonVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("WindowKaptonLogical");
    //windowCopperVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("WindowCopperLogical");
    //driftKaptonVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("DriftKaptonLogical");
    //driftKaptonVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("DriftFr4Logical");
    //driftCopperVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("DriftCopperLogical");
    driftGapVolume = G4LogicalVolumeStore::GetInstance()->GetVolume("DriftGapLogical");
  }

  //cout << (driftKaptonVolume==NULL) << endl;
  //cout << volumesBeforeDrift.size() << endl;
  
  G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  G4String particleName = track->GetParticleDefinition()->GetParticleName();
  G4LogicalVolume *nextVolume = track->GetNextVolume()->GetLogicalVolume();

  if (step->IsLastStepInVolume() and particleName==G4String("gamma")) {
    for (int i=0; i<volumesBeforeDrift.size()-1; i++) {
      if (volume==volumesBeforeDrift[i])
        this->eventAction->AddHit(volumeBranchNames[i], step->GetPreStepPoint()->GetTotalEnergy()*1.e3);
    }
    //if (volume==windowKaptonVolume) this->eventAction->AddHit("window", step->GetPreStepPoint()->GetTotalEnergy()*1.e3);
    //else if (volume==driftFr4Volume) this->eventAction->AddHit("driftFr4", step->GetPreStepPoint()->GetTotalEnergy()*1.e3);
    //else if (volume==driftKaptonVolume) this->eventAction->AddHit("driftKapton", step->GetPreStepPoint()->GetTotalEnergy()*1.e3);
    //if (volume==driftCopperVolume) {
    if (volume==volumesBeforeDrift[volumesBeforeDrift.size()-1]) { // last volume before gas is always copper drit
      //cout << track->GetCreatorProcess()->GetProcessName() << endl;
      this->eventAction->AddHit(volumeBranchNames[volumeBranchNames.size()-1], step->GetPreStepPoint()->GetTotalEnergy()*1.e3);
      this->eventAction->AddPhoton(
        step->GetPostStepPoint()->GetTotalEnergy()*1.e3,
        step->GetPostStepPoint()->GetPosition(),
        step->GetPostStepPoint()->GetMomentumDirection()
      );
      //cout << step->GetPreStepPoint()->GetPosition() << " ";
      //cout << step->GetPostStepPoint()->GetPosition() << endl;
    }
  } else if (step->IsFirstStepInVolume() and volume==driftGapVolume) {
    if (particleName!=G4String("gamma") and particleName!=G4String("e-")) {
      cout << particleName << endl;
    } else if (particleName==G4String("gamma")) {
      //if (track->GetCreatorProcess()) cout << track->GetCreatorProcess()->GetProcessName() << endl;
    } else if (particleName==G4String("e-")) {
      //cout << track->GetCreatorProcess()->GetProcessName() << endl;
      this->eventAction->AddElectron(
        step->GetPostStepPoint()->GetKineticEnergy()*1.e3,
        step->GetPostStepPoint()->GetPosition(),
        step->GetPostStepPoint()->GetMomentumDirection()
      );
      //cout << step->GetPreStepPoint()->GetPosition() << " ";
      //cout << step->GetPostStepPoint()->GetPosition() << endl;
    }
  } else if (particleName==G4String("e-") and volume==driftCopperVolume) {
      //cout << track->GetCreatorProcess()->GetProcessName() << endl;
      /*this->eventAction->AddElectron(
        step->GetPostStepPoint()->GetKineticEnergy()*1.e3,
        step->GetPostStepPoint()->GetPosition(),
        step->GetPostStepPoint()->GetMomentumDirection()
      );*/
  }
}

      /*G4cout << "primary " << step->GetTrack()->GetParticleDefinition()->GetParticleName();
      G4cout << " process " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
      G4cout << " energy before " << step->GetPreStepPoint()->GetTotalEnergy()*1e3 << " keV";
      G4cout << " energy after " << step->GetPostStepPoint()->GetTotalEnergy()*1e3 << " keV";
      G4cout << " from volume " << step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
      G4cout << " to volume " << step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
      G4cout << G4endl;*/


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

