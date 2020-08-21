#include "PhysicsList.hh"
#include "QBBC.hh"
#include "globals.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmExtraPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4Cerenkov.hh"

#include "G4HadronInelasticQBBC.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsXS.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4ChargeExchangePhysics.hh"
#include "G4IonPhysicsXS.hh"
#include "G4IonElasticPhysics.hh"
#include "G4NeutronTrackingCut.hh"

#include "G4BetheBlochIonGasModel.hh"
#include "G4BraggIonGasModel.hh"
#include "G4EmConfigurator.hh"
#include "G4LossTableManager.hh"
#include "G4IonFluctuations.hh"
#include "G4UniversalFluctuation.hh"

#include "G4EmLivermorePhysics.hh"
#include "G4EmProcessOptions.hh"

#include "G4ParticleTable.hh"

PhysicsList::PhysicsList():G4VModularPhysicsList() {
  emPhysicsList = new G4EmLivermorePhysics();
  RegisterPhysics(emPhysicsList);
  RegisterPhysics(new G4EmStandardPhysics());
  
  G4double cutForGamma = 0.1*mm;
  SetCutValue(cutForGamma, "gamma");

  SetVerboseLevel(1);
}

PhysicsList::~PhysicsList() {
  //delete emPhysicsList;
}

void PhysicsList::ConstructProcess() {
  AddTransportation();
  emPhysicsList->ConstructProcess();
  G4cout << "constructing physics process" << G4endl;

  G4EmProcessOptions *emOptions = new G4EmProcessOptions();
  emOptions->SetFluo(true);
}
  
