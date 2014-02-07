/***********

Main file for starting single server queue simulation. Instantiates a
Simulation Object and a Queuing System Object, and starts the
simulation

*****************/

#include <iostream>
#include "Event.h"
#include "QueueingSystem.h"
#include "Simulation.h"
#include "declarations.h"
#include "LocatorHeap.h"
using namespace std;

/****
Overload << operator so that cout works to print Event objects
Needed for trace/debugging
****/

ostream & operator << (ostream& ostr, const Event& e) {
  e.print(ostr);
  return ostr;
}

//Main - just instantiates objects, starts simulation
int main() {
  // Instantiate Simulation
  Simulation qsim;
  // Start Simulation
  qsim.run();  
  return 0;
}
