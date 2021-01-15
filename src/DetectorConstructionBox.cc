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
/// \file DetectorConstructionBox.cc
/// \brief Implementation of the DetectorConstructionBox class

#include "DetectorConstructionBox.hh"

#include "G4Material.hh"
#include "G4String.hh"
#include "G4Element.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstructionBox::DetectorConstructionBox(std::vector<std::pair<G4String, G4double>> materialLayers):G4VUserDetectorConstruction() {
  this->materialLayers = materialLayers;
  colorMap["copper"] = G4Colour(0, 0, .8, 1);
  colorMap["kapton"] = G4Colour(.8, 0, 0, 1);
  colorMap["argon"] = G4Colour(1, 0, 0, 1);
  colorMap["fr4"] = G4Colour(.8, 0, 0, 1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstructionBox::~DetectorConstructionBox()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstructionBox::Construct()
{
  ConstructMaterials();

  G4Element* Cl = new G4Element("Chlorine", "Cl", 17., 35.5*g/mole);
  G4Element* C = new G4Element("Carbon", "C", 6., 12.0*g/mole);
  G4Element* H = new G4Element("Hydrozen", "H", 1., 1.00794*g/mole);
  G4Element* O = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
  G4Element *N = new G4Element("Nitrogen", "N2", 7., 14.01*g/mole);
    
  double copperDensity = 8.960*g/cm3;
  double copperAtomicWeight = 63.55*g/mole;
  G4Material *copper = new G4Material("Copper", 29., copperAtomicWeight, copperDensity);

  double kaptonDensity = 1.42*g/cm3;
  G4Material *kapton = new G4Material("Kapton", kaptonDensity, 4);
  kapton->AddElement(H, 0.0273);
  kapton->AddElement(C, 0.7213);
  kapton->AddElement(N, 0.0765);
  kapton->AddElement(O, 0.1749);
  
  G4Material *pvc = new G4Material("PVC", 1.68*g/cm3, 3);
  pvc->AddElement(C, 2);
  pvc->AddElement(H, 3);
  pvc->AddElement(Cl, 1);

  G4Material *argon = G4Material::GetMaterial("G4_Ar");

  G4Material *fr4 = createFR4();

  if (materialMap.size()==0) {
    materialMap["copper"] = copper;
    materialMap["kapton"] = kapton;
    materialMap["pvc"] = pvc;
    materialMap["argon"] = argon;
    materialMap["fr4"] = fr4;
  }
  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  G4double sourceToChamberZ = 10*mm;

  G4double copperWindowThickness = 35*um;
  G4double gasThickness = 0*mm;
  G4double driftFr4Thickness = 3*mm;
  G4double driftCopperThickness = 35*um;
  G4double driftGapThickness = 3*mm;

  /*G4double copperThickness = fCopperThickness*um;
  G4double fr4Thickness = 3*mm;
  G4double cathodeThickness = 35*um;
  G4double copperToFr4Distance = 30*cm-fCopperThickness;*/

  // maximum object size in xy
  G4double sizeXY = 10*mm;
  G4double chamberSizeXY = 10*cm;
  // envelope parameters
  G4double envSizeXY = 250*cm;
  G4double envSizeZ = 250*cm;

  for (auto materialNameThicknessPair:materialLayers) {
    envSizeZ += materialNameThicknessPair.second*1.5;
  }

  //sourceToChamberZ+copperWindowThickness+gasThickness+driftFr4Thickness+driftCopperThickness+2*driftGapThickness;
  
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;
  
  //     
  // World
  //
  G4double worldSizeXY = 1.2*envSizeXY;
  G4double worldSizeZ  = 1.2*envSizeZ;
  G4Material* worldMaterial = nist->FindOrBuildMaterial("G4_AIR");

  G4Box* solidWorld = new G4Box("World", 0.5*worldSizeXY, 0.5*worldSizeXY, 0.5*worldSizeZ);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, checkOverlaps);
  
  //
  // Envelope
  //
  G4Material* envMaterial = nist->FindOrBuildMaterial("G4_AIR");
  G4Box* solidEnv = new G4Box("Envelope", 0.5*envSizeXY, 0.5*envSizeXY, 0.5*envSizeZ);
  G4LogicalVolume* logicEnv = new G4LogicalVolume(solidEnv, envMaterial, "Envelope");
  new G4PVPlacement(0, G4ThreeVector(), logicEnv, "Envelope", logicWorld, false, 0, checkOverlaps);

  G4double layerPosition = -0.15*envSizeZ;
  //G4double layerPosition = 0.;
  G4int materialIndex = 0;
  for (auto materialNameThicknessPair:materialLayers) { // add all material layers
    G4String materialName = materialNameThicknessPair.first;
    G4double materialThickness = materialNameThicknessPair.second*mm;

    if (materialName==G4String("vacuum")) { // just leave empty space 
      layerPosition += materialThickness;
      continue;
    }
    materialIndex++;
    layerPosition += 0.5*materialThickness;

    G4String boxName = G4String("Box")+materialName+std::to_string(materialIndex);
    G4String logicalName = G4String("Logical")+materialName+std::to_string(materialIndex);
    G4String physicalName = G4String("Physical")+materialName+std::to_string(materialIndex);

    G4Box *box = new G4Box(boxName, 0.5*chamberSizeXY, 0.5*chamberSizeXY, 0.5*materialThickness);
    G4LogicalVolume *logical = new G4LogicalVolume(box, materialMap[materialName], logicalName);
    G4VisAttributes *visAttributes = new G4VisAttributes(colorMap[materialName]);
    logical->SetVisAttributes(visAttributes);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,layerPosition), logical, physicalName, logicEnv, false, 0, checkOverlaps);

    layerPosition += 0.5*materialThickness;
  }

  /*
  //
  // First copper layer, 35 um
  //
  G4double windowCopperZ = -0.5*envSizeZ + sourceToChamberZ + 0.5*copperWindowThickness;
  G4Box *windowBoxSolid = new G4Box("WindowCopperBox", 0.5*sizeXY, 0.5*sizeXY, 0.5*copperWindowThickness);
  G4LogicalVolume *windowCopperLogical = new G4LogicalVolume(windowBoxSolid, kapton, "WindowCopperLogical");
  new G4PVPlacement(0, G4ThreeVector(0.,0.,windowCopperZ), windowCopperLogical, "WindowCopperPhysical", logicEnv, false, 0, checkOverlaps);

  //
  // Drift FR4 layer, 3 mm
  //
  G4double driftFr4Z = -0.5*envSizeZ + sourceToChamberZ + copperWindowThickness + gasThickness + 0.5*driftFr4Thickness;
  G4Box *driftFr4Solid = new G4Box("DriftFr4Box", 0.5*sizeXY, 0.5*sizeXY, 0.5*driftFr4Thickness);
  G4LogicalVolume *driftFr4Logical = new G4LogicalVolume(driftFr4Solid, fr4, "DriftFr4Logical");
  new G4PVPlacement(0, G4ThreeVector(0.,0.,driftFr4Z), driftFr4Logical, "DriftFr4Physical", logicEnv, false, 0, checkOverlaps);
  
  //
  // Cathode, 35 um copper
  //
  G4double driftCopperZ = -0.5*envSizeZ + sourceToChamberZ + copperWindowThickness + gasThickness + driftFr4Thickness + 0.5*driftCopperThickness;
  G4Box *driftCopperSolid = new G4Box("DriftCopperBox", 0.5*sizeXY, 0.5*sizeXY, 0.5*driftCopperThickness);
  G4LogicalVolume *driftCopperLogical = new G4LogicalVolume(driftCopperSolid, copper, "DriftCopperLogical");
  new G4PVPlacement(0, G4ThreeVector(0.,0.,driftCopperZ), driftCopperLogical, "DriftCopperPhysical", logicEnv, false, 0, checkOverlaps);
  */

  // Add gas gap, 3 mm argon (unused)
  G4double driftGapZ = layerPosition + 0.5*driftGapThickness;
  G4Box *driftGapSolid = new G4Box("DriftGapBox", 0.5*sizeXY, 0.5*sizeXY, 0.5*driftGapThickness);
  G4LogicalVolume *driftGapLogical = new G4LogicalVolume(driftGapSolid, argon, "DriftGapLogical");
  new G4PVPlacement(0, G4ThreeVector(0.,0.,driftGapZ), driftGapLogical, "DriftGapPhysical", logicEnv, false, 0, checkOverlaps);

  // Add chamber walls
  G4double wallThickness = 5*mm;
  G4double wallSizeX = 94*cm, wallSizeY = 144*cm, wallSizeZ = 200*cm;

  G4Box *wallZ1Solid = new G4Box("WallZ1Box", 0.5*wallSizeX, 0.5*wallSizeY, 0.5*wallThickness);
  G4LogicalVolume *wallZ1Logical = new G4LogicalVolume(wallZ1Solid, copper, "WallZ1Logical");
  new G4PVPlacement(0, G4ThreeVector(0.,0.,0.5*wallSizeZ), wallZ1Logical, "WallZ1Physical", logicEnv, false, 0, checkOverlaps);

  G4Box *wallY1Solid = new G4Box("WallY1Box", 0.5*wallSizeX, 0.5*wallThickness, 0.5*wallSizeZ);
  G4LogicalVolume *wallY1Logical = new G4LogicalVolume(wallY1Solid, copper, "WallY1Logical");
  new G4PVPlacement(0, G4ThreeVector(0.,0.5*wallSizeY,0.), wallY1Logical, "WallY1Physical", logicEnv, false, 0, checkOverlaps);
  G4Box *wallY2Solid = new G4Box("WallY2Box", 0.5*wallSizeX, 0.5*wallThickness, 0.5*wallSizeZ);
  G4LogicalVolume *wallY2Logical = new G4LogicalVolume(wallY2Solid, copper, "WallY2Logical");
  new G4PVPlacement(0, G4ThreeVector(0.,-0.5*wallSizeY,0.), wallY2Logical, "WallY2Physical", logicEnv, false, 0, checkOverlaps);

  G4Box *wallX1Solid = new G4Box("WallX1Box", 0.5*wallThickness, 0.5*wallSizeY, 0.5*wallSizeZ);
  G4LogicalVolume *wallX1Logical = new G4LogicalVolume(wallX1Solid, copper, "WallX1Logical");
  new G4PVPlacement(0, G4ThreeVector(0.5*wallSizeX,0.,0.), wallX1Logical, "WallX1Physical", logicEnv, false, 0, checkOverlaps);
  G4Box *wallX2Solid = new G4Box("WallX2Box", 0.5*wallThickness, 0.5*wallSizeY, 0.5*wallSizeZ);
  G4LogicalVolume *wallX2Logical = new G4LogicalVolume(wallX2Solid, copper, "WallX2Logical");
  new G4PVPlacement(0, G4ThreeVector(-0.5*wallSizeX,0.,0.), wallX2Logical, "WallX2Physical", logicEnv, false, 0, checkOverlaps);

  G4VisAttributes *visAttributes = new G4VisAttributes(G4Colour(255,248,220,.2));
  wallZ1Logical->SetVisAttributes(visAttributes);
  wallY1Logical->SetVisAttributes(visAttributes);
  wallY2Logical->SetVisAttributes(visAttributes);
  wallX1Logical->SetVisAttributes(visAttributes);
  wallX2Logical->SetVisAttributes(visAttributes);


  return physWorld;
}

G4Material *DetectorConstructionBox::createFR4() {
  G4int natoms, numel;
  G4double density, fractionMass;

  G4Element* Cl = new G4Element("Chlorine", "Cl", 17., 35.5*g/mole);
  G4Element* C = new G4Element("Carbon", "C", 6., 12.0*g/mole);
  G4Element* H = new G4Element("Hydrozen", "H", 1., 1.00794*g/mole);
  G4Element* O  = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
  G4Element* Si = new G4Element("Silicon", "Si", 14., 28.0855*g/mole);
  
  G4Material *epoxy = new G4Material("epoxy", density=1.2*g/cm3, numel=2);
  epoxy->AddElement(H, natoms=2);
  epoxy->AddElement(C, natoms=2);

  G4Material *silica = new G4Material("silica", density=2.2*g/cm3, numel=2);
  silica->AddElement(Si, natoms=1);
  silica->AddElement(O, natoms=2);

  G4Material *fr4 = new G4Material("FR4", density=1.86*g/cm3, numel=2);
  fr4->AddMaterial(epoxy, fractionMass=0.472);
  fr4->AddMaterial(silica, fractionMass=0.528);

  return fr4;
}

void DetectorConstructionBox::ConstructMaterials() {
   auto nistManager = G4NistManager::Instance();

   // Air 
   nistManager->FindOrBuildMaterial("G4_AIR");

   // Argon gas
   nistManager->FindOrBuildMaterial("G4_Ar");
   // With a density different from the one defined in NIST
   // G4double density = 1.782e-03*g/cm3; 
   // nistManager->BuildMaterialWithNewDensity("B5_Ar","G4_Ar",density);
   // !! cases segmentation fault

   // Scintillator
   // (PolyVinylToluene, C_9H_10)
   nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

   // Vacuum "Galactic"
   // nistManager->FindOrBuildMaterial("G4_Galactic");

   // Vacuum "Air with low density"
   // auto air = G4Material::GetMaterial("G4_AIR");
   // G4double density = 1.0e-5*air->GetDensity();
   // nistManager
   //   ->BuildMaterialWithNewDensity("Air_lowDensity", "G4_AIR", density);

   G4cout << G4endl << "The materials defined are: " << G4endl << G4endl;
   G4cout << *(G4Material::GetMaterialTable()) << G4endl;
 }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
