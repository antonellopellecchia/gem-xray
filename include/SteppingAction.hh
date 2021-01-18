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
/// \file SteppingAction.hh
/// \brief Definition of the SteppingAction class

#ifndef SteppingAction_h
#define SteppingAction_h 1

#include<vector>

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class EventAction;

class G4LogicalVolume;

/// Stepping action class
/// 

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(EventAction* eventAction);
  virtual ~SteppingAction();

  // method from the base class
  virtual void UserSteppingAction(const G4Step*);

private:
  EventAction*     eventAction = 0;
  //G4LogicalVolume* windowKaptonVolume;
  //G4LogicalVolume* driftKaptonVolume;
  //G4LogicalVolume* driftFr4Volume;
  G4LogicalVolume* driftCopperVolume;
  G4LogicalVolume* driftGapVolume;

  std::vector<std::pair<G4String, G4double>> fLayersMap;
  std::vector<G4LogicalVolume *> volumesBeforeDrift;
  std::vector<G4String> volumeBranchNames;
  /*std::map<std::string, std::string> volumeBranchNames;
  std::string volumeNamesBeforeDrift[4] = {
  	"WindowKaptonLogical",
  	"WindowCopperLogical",
  	"DriftKaptonLogical",
  	"DriftFr4Logical"
  };*/
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
