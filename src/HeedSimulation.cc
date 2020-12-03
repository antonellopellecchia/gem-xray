#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>

#include <TPad.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TFile.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TTree.h>
#include <TBranch.h>

#include "Garfield/TrackHeed.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/SolidBox.hh"
#include "Garfield/GeometrySimple.hh"
#include "Garfield/ViewGeometry.hh"
#include "Garfield/ViewField.hh"
#include "Garfield/ComponentConstant.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/FundamentalConstants.hh"
#include "Garfield/Random.hh"
#include "Garfield/Plotting.hh"

#include "HeedSimulation.hh"
#include "EventAction.hh"
#include "RunAction.hh"

using namespace Garfield;
using namespace std;

HeedSimulation::HeedSimulation(RunAction *runAction) {
  this->runAction = runAction;
  
  MediumMagboltz* gas = new MediumMagboltz();
  gas->SetComposition("Ar", 70., "CO2", 30.);
  gas->SetTemperature(293.15);
  gas->SetPressure(AtmosphericPressure);

  // gas gap sides in cm
  const double length = 10;
  const double width = 10;
  const double depth = 0.3;
  SolidBox *box = new SolidBox(0., 0., 0., length/2., width/2., depth/2.);
  GeometrySimple *geo = new GeometrySimple();
  geo->AddSolid(box, gas);
	
  this->track = new TrackHeed();

  Sensor *sensor = new Sensor();
  ComponentConstant *field = new ComponentConstant();
  field->SetGeometry(geo);
  sensor->AddComponent(field);
  track->SetSensor(sensor);
}

HeedSimulation::~HeedSimulation() {}

void HeedSimulation::TransportPhoton(EventAction *eventAction, G4double energy, G4ThreeVector position, G4ThreeVector momentum) {
  const double x0 = position.getX()*1e-1;
  const double y0 = position.getY()*1e-1;
  const double z0 = -0.15;
  const double t0 = 0.;
  const double dx = momentum.getX();
  const double dy = momentum.getY();
  const double dz = momentum.getZ();
  const double e0 = energy*1.e3;
  int primaries = 0;
  this->track->TransportPhoton(x0, y0, z0, t0, e0, dx, dy, dz, primaries);
  if (primaries>0) this->runAction->FillNtuples("conversion", primaries/gasIonizationEnergy);
  cout << "HEED " <<  primaries/gasIonizationEnergy << endl;
}
