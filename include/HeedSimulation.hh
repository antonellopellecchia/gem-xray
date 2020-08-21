#ifndef HeedSimulation_h
#define HeedSimulation_h 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>

#include "EventAction.hh"
#include "RunAction.hh"

#include "Garfield/TrackHeed.hh"

using namespace Garfield;
using namespace std;

class EventAction;
class RunAction;

class HeedSimulation {
public:
  HeedSimulation(RunAction *runAction);
  virtual ~HeedSimulation();
  
  void TransportPhoton(EventAction *eventAction, G4double energy, G4ThreeVector position, G4ThreeVector momentum);

private:
  RunAction *runAction;
  TrackHeed *track;
  double gasIonizationEnergy = 31.2; // from previous HEED simulation
};

#endif
