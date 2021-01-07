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
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Gamma.hh"

#include "RunAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(EventAction *eventAction)
  : G4VUserPrimaryGeneratorAction(),
    fParticleGun(0), 
    fEnvelopeBox(0),
    fCopperBox(0),
    fEventAction(eventAction)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  G4ParticleDefinition* particle = G4Gamma::Definition();
  fParticleGun->SetParticleDefinition(particle);

  /*this->energies = runAction->GetPrimaryEnergies();
  this->spectrum = runAction->GetPrimarySpectrum();
  this->sumSpectrum = runAction->GetPrimarySpectrumSum();*/
  
  ReadSpectrumData();
  
  /*this->energies = runAction->primaryEnergies;
  this->spectrum = runAction->primarySpectrum;
  this->sumSpectrum = runAction->primarySpectrumSum;*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::ReadSpectrumData() {
  primaryEnergies = new G4DataVector;
  primarySpectrum = new G4DataVector;
  primarySpectrumSum = 0.;
  
  G4String primarySpectrumPath = "xray-spectrum.csv";
  ifstream primarySpectrumFile(primarySpectrumPath);

  G4double energy;
  G4double spectrumline;
  
  G4String line = "";
  while (getline(primarySpectrumFile, line)) {
    sscanf(line.c_str(), "%lf, %lf", &energy, &spectrumline);
    primaryEnergies->push_back(energy);
    primarySpectrum->push_back(spectrumline);
    primarySpectrumSum += spectrumline;
    //G4cout << line << " " << energy << " " << spectrumline << G4endl;
    //G4cout << primaryEnergies->size() << primaryEnergies[0] << G4endl;
  }
  primarySpectrumFile.close();

  /*G4String primaryAngularDistPath = "source_angular_dist.csv";
  ifstream primaryAngularDistFile(primaryAngularDistPath);

  G4double angle;
  G4double angularline;
  
  while (getline(primaryAngularDistFile, line)) {
    sscanf(line.c_str(), "%lf, %lf", &angle, &angularline);
    primaryAngles->push_back(angle);
    primaryAngularDist->push_back(angularline);
    primaryAngularDistSum += angularline;
  }
  primaryAngularDistFile.close();*/

  G4cout << "Finished ReadSpectrumData()" << G4endl;
  G4cout << primaryEnergies << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of ecah event
  //

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get Envelope volume
  // from G4LogicalVolumeStore.
  
  G4double envSizeXY = 0;
  G4double envSizeZ = 0;

  if (!runAction) runAction = (RunAction *)G4RunManager::GetRunManager()->GetUserRunAction();

  if (!fEnvelopeBox)
    {
      G4LogicalVolume* envLV = G4LogicalVolumeStore::GetInstance()->GetVolume("Envelope");
      if ( envLV ) fEnvelopeBox = dynamic_cast<G4Box*>(envLV->GetSolid());
    }

  if ( fEnvelopeBox ) {
    envSizeXY = fEnvelopeBox->GetXHalfLength()*2.;
    envSizeZ = fEnvelopeBox->GetZHalfLength()*2.;
  }  
  else  {
    G4ExceptionDescription msg;
    msg << "Envelope volume of box shape not found.\n"; 
    msg << "Perhaps you have changed geometry.\n";
    msg << "The gun will be placed at the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
		"MyCode0002",JustWarning,msg);
  }

  G4double size = 0.8;
  G4double x0 = size * envSizeXY * (G4UniformRand()-0.5);
  G4double y0 = size * envSizeXY * (G4UniformRand()-0.5);
  G4double z0 = -0.5 * envSizeZ;

  // gaussian positioning, center in (0,0), sigma 1 mm
  G4double beamSigma = 1.*mm;
  //G4double r0 = G4RandGauss::shoot(0, beamSigma);
  //G4double theta0 = G4UniformRand()*CLHEP::pi;
  //x0 = r0*cos(theta0);
  //y0 = r0*sin(theta0);
  //std::tuple<G4double, G4double> posBeginning = std::make_tuple(x0, y0);
  //fEventAction->AddBeginningPosition(posBeginning);

  x0 = 0;
  y0 = 0;
  //x0 = G4RandGauss::shoot(0, beamSigma);
  //y0 = G4RandGauss::shoot(0, beamSigma);
  fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));

  G4double particleEnergy = 0.;
  G4double partSumSpectrum = 0.;
  G4int j = 0;
  G4double random = primarySpectrumSum*G4UniformRand();
  while (partSumSpectrum<random) {
    partSumSpectrum += (*primarySpectrum)[j];
    j++;
  }
  particleEnergy = (*primaryEnergies)[j];

  /*
  FOR Fe55 SPECTRUM
  G4double particleEnergy = 0.;
  G4double energyRand = G4UniformRand();
  if (energyRand<=this->ironLineIntensities[0]) particleEnergy = ironLineEnergies[0];
  else particleEnergy = ironLineEnergies[1];
  */

  fParticleGun->SetParticleEnergy(particleEnergy*keV);
  runAction->FillNtuples("primary", particleEnergy);
  
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

