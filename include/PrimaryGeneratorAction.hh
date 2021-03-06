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
/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4String.hh"
#include "globals.hh"

#include "RunAction.hh"

class G4ParticleGun;
class EventAction;
class G4Event;
class G4Box;

/// The primary generator action class with particle gun.
///
/// The default kinematic is a 6 MeV gamma, randomly distribued 
/// in front of the phantom across 80% of the (X,Y) phantom size.

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction(EventAction *eventAction, G4String source, bool headless);    
  virtual ~PrimaryGeneratorAction();

  // method from the base class
  virtual void GeneratePrimaries(G4Event*);         
  
  // method to access particle gun
  const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

  void ReadSpectrumData();
  
private:
  G4ParticleGun*        fParticleGun;
  G4Box*                fEnvelopeBox;
  G4Box*                fCopperBox;
  EventAction*          fEventAction;
  RunAction* runAction;

  bool fHeadless;
  G4String fSource;

  G4DataVector *primaryEnergies;
  G4DataVector *primarySpectrum;
  G4double primarySpectrumSum;

  G4double ironLineEnergies[2] = {5.89, 6.49};
  G4double ironLineIntensities[2] = {0.87985866, 0.12014134};

  //G4double cadmiumLineEnergies[3] = {3.1, 22.1, 25.0};
  //G4double cadmiumLineIntensities[3] = {0.09256109569420821, 0.7483663056127473, 0.1590725986930445};
  G4double cadmiumLineEnergies[2] = {22.1, 25.0};
  G4double cadmiumLineIntensities[2] = {0.7483663056127473, 0.1590725986930445};
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
