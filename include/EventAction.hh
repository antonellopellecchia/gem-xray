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
/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"

#include "RunAction.hh"

#include <TH1F.h>

#include <vector>
#include <map>

using namespace std;

class RunAction;

/// Event action class
///

struct particle { // needed to interface with HEED
  G4double energy;
  G4ThreeVector position;
  G4ThreeVector momentum;
};

class EventAction : public G4UserEventAction
{
public:
  EventAction(RunAction* runAction);
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

  void AddHit(G4String volume, G4double energy);
  void AddPhoton(G4double energy, G4ThreeVector position, G4ThreeVector momentum);
  void AddElectron(G4double energy, G4ThreeVector position, G4ThreeVector momentum);
  int TransportPhotons();
  int TransportElectrons();

  std::vector<std::pair<G4String, G4double>> fLayersMap;
  
private:
  RunAction* runAction;


  map<string,G4DataVector*> hitEnergies;
  map<string,vector<G4ThreeVector>*> hitPositions;
  map<string,vector<G4ThreeVector>*> hitMomenta;
  
  std::vector<G4String> volumeBranchNames;
  G4String volumes[3] = {"window", "driftKapton", "driftCopper"};

  vector<particle> *electrons;
  vector<particle> *photons;
  
  double gasIonizationEnergy = 31.2; // from previous HEED simulation
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
