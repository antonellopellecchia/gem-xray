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
/// \file exampleB1.cc
/// \brief Main program of the B1 example

#include "DetectorConstruction.hh"
#include "DetectorConstructionBox.hh"
#include "DetectorConstruction10x10.hh"
#include "DetectorConstructionME0.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  bool headless = true;

  string outFilePath = "";
  string geometry = "custom10x10"; // 10x10, ME0 or custom
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
    headless = false;
  } else if (argc >= 3) {
    if (string("test") == argv[2]) headless = false; // headless, but no output
    else outFilePath = string(argv[2]); // headless, with output file
  }
  if (argc >= 4) geometry = string(argv[3]);

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // Set mandatory initialization classes
  //
  // Detector construction

  std::vector<std::pair<G4String, G4double>> exampleMaterialLayers;
  if (string("custom")==geometry) {
    exampleMaterialLayers.push_back(std::make_pair(G4String("vacuum"),G4double(1.5)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("copper"),G4double(35e-3)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("vacuum"),G4double(1.0)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("fr4"),G4double(3)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("copper"),G4double(35e-2)));
  } else if (string("custom10x10")==geometry) {
    exampleMaterialLayers.push_back(std::make_pair(G4String("vacuum"),G4double(500)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("copper"),G4double(35e-3)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("fr4"),G4double(3.0)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("copper"),G4double(35e-3)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("vacuum"),G4double(1.5)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("kapton"),G4double(125e-3)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("vacuum"),G4double(3.0)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("kapton"),G4double(5e-3)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("copper"),G4double(5e-3)));
  } else if (string("10x10")==geometry) {
    exampleMaterialLayers.push_back(std::make_pair(G4String("vacuum"),G4double(1.5)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("kapton"),G4double(125e-3)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("vacuum"),G4double(3.0)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("kapton"),G4double(5e-3)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("copper"),G4double(5e-3)));
  }
  else if (string("ME0")==geometry) {
    exampleMaterialLayers.push_back(std::make_pair(G4String("vacuum"),G4double(1.5)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("copper"),G4double(35e-3)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("fr4"),G4double(3.0)));
    exampleMaterialLayers.push_back(std::make_pair(G4String("copper"),G4double(35e-3)));
  }
  runManager->SetUserInitialization(new DetectorConstructionBox(exampleMaterialLayers));

  // Physics list
  G4VModularPhysicsList* physicsList = new PhysicsList(); //new QBBC;
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);
    
  // User action initialization
  runManager->SetUserInitialization(new ActionInitialization(headless, outFilePath, exampleMaterialLayers));
  
  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) { 
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else { 
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !
  
  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
