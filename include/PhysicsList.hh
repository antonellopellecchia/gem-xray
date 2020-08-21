#include "QBBC.hh"
#include "globals.hh"

class PhysicsList : public G4VModularPhysicsList
{
public:

  explicit PhysicsList();
  virtual ~PhysicsList();

  void ConstructProcess();
  
private:

  // copy constructor and hide assignment operator
  PhysicsList(PhysicsList &);
  PhysicsList & operator=(const PhysicsList &right);

  G4VPhysicsConstructor *emPhysicsList;
};

