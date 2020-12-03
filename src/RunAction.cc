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
/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/stat.h>

#include <TCanvas.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TH2F.h>
#include <TFile.h> 
#include <TStyle.h>
#include <TF1.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TTree.h>

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(G4bool headless, string outFilePath): G4UserRunAction() {
  this->headless = headless;
  this->heedSimulation = new HeedSimulation(this);
  
  if (this->headless) runFile = new TFile(outFilePath.c_str(), "RECREATE", "Simulation output ntuples");

  for (G4String volume:volumes) {
    hitEnergyMap[volume] = 0.;
    treeMap[volume] = new TTree(volume, "");
    treeMap[volume]->Branch("energy", &hitEnergyMap[volume], "energy/D");
  }
  treeMap["driftCopper"]->Branch("positionX", &hitPositionX, "positionX/D");
  treeMap["driftCopper"]->Branch("positionY", &hitPositionY, "positionY/D");
  treeMap["driftCopper"]->Branch("positionZ", &hitPositionZ, "positionZ/D");
  treeMap["driftCopper"]->Branch("momentumX", &hitMomentumX, "momentumX/D");
  treeMap["driftCopper"]->Branch("momentumY", &hitMomentumY, "momentumY/D");
  treeMap["driftCopper"]->Branch("momentumZ", &hitMomentumZ, "momentumZ/D");
  treeMap["conversion"]->Branch("primaries", &gasPrimaries, "primaries/D");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* run) {
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  nOfEvents = run->GetNumberOfEventToBeProcessed();
  G4cout << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run) {
  /*G4String out_dir = G4String("./out/");
  G4String root_out_dir = G4String(out_dir+"root/");
  G4String eps_out_dir = G4String(out_dir+"eps/");
  mkdir(out_dir.c_str(), 0700);
  mkdir(root_out_dir.c_str(), 0700);
  mkdir(eps_out_dir.c_str(), 0700);*/

  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  if (this->headless) {
    G4cout << G4endl;
    for (auto treePair:treeMap) treePair.second->Print();
    runFile->Write();
    runFile->Close();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::FillNtuples(G4String volume, G4double energy) {
  if (this->headless) {
    hitEnergyMap[volume] = energy;
    treeMap[volume]->Fill();
  }
}

void RunAction::FillNtuples(G4String volume, G4double energy, G4int primaries) {
  if (this->headless) {
    hitEnergyMap[volume] = energy;
    gasPrimaries = primaries;
    treeMap[volume]->Fill();
  }
}

void RunAction::FillNtuples(G4String volume, G4double energy, G4ThreeVector position, G4ThreeVector momentum) {
  hitEnergyMap[volume] = energy;

  hitPositionX = position.getX();
  hitPositionY = position.getY();
  hitPositionZ = position.getZ();

  hitMomentumX = momentum.getX();
  hitMomentumY = momentum.getY();
  hitMomentumZ = momentum.getZ();
  
  treeMap[volume]->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

