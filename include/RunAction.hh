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
/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RunAction_h
#define RunAction_h 1

#include <TTree.h>
#include <TH1F.h>

#include "HeedSimulation.hh"

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "globals.hh"

#include <vector>
#include <map>

using namespace std;

class G4Run;
class HeedSimulation;

class RunAction:public G4UserRunAction {
public:
  RunAction(bool headless, string outFilePath);
  virtual ~RunAction();

  // virtual G4Run* GenerateRun();
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

  void FillNtuples(G4String volume, G4double energy);
  void FillNtuples(G4String volume, G4double energy, G4int primaries);
  void FillNtuples(G4String volume, G4double energy, G4ThreeVector position, G4ThreeVector momentum);
  
  G4int nOfEvents;

  HeedSimulation *heedSimulation;

  /*G4DataVector *primaryEnergies;// = new G4DataVector;
  G4DataVector *primarySpectrum;// = new G4DataVector;
  G4double primarySpectrumSum;// = 0.;

  G4DataVector *primaryAngles;
  G4DataVector *primaryAngularDist;
  G4double primaryAngularDistSum;*/

private:
  TFile *runFile;
  TTree *primaryTree;
  TTree *afterWindowTree;
  TTree *afterDriftTree;
  TTree *inGasTree;

  bool headless;

  // variables for ntuples
  map<G4String, G4double> hitEnergyMap;
  
  G4double gasPrimaries;

  G4double hitPositionX;
  G4double hitPositionY;
  G4double hitPositionZ;

  G4double hitMomentumX;
  G4double hitMomentumY;
  G4double hitMomentumZ;

  map<G4String, TTree*> treeMap;
  G4String volumes[5] = {"primary", "window", "driftKapton", "driftCopper", "conversion"};
};

#endif

