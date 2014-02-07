#include "Request.h"
#include <iostream>

using namespace std;

//Constructor with argument defaults
Request::Request(int id=0, double atime=0.0, double stime=0.0) {  
  ID = id;
  serverAssigned=0;
  arrivalTime=atime;
  serviceTime=stime;
  ID1=0;
}

//Operator overload, a request r1 is "less than" a request r2 if the 
//"age" of r1 in the system is lesser than the "age" of r2.
//This will happen if r1's arrival time is greater (more recent) than
//r2's arrival time

bool Request::operator<(const Request& r) const  {
  return (arrivalTime > r.arrivalTime);
}

// print arrival time and number of instructions
void Request::printdetail(){
  cout << "(" <<arrivalTime<<","<<serviceTime<<")";
}

// set ID1 of request, ID1 is used in TimeOut implementation
void Request::setID1(int k){
  ID1=k;
}

// accessor functions
double Request::getArrivalTime() {
  return arrivalTime;
}
  
double Request::getServiceTime() {
  return serviceTime;
}

void Request::setServer(int k) {
  serverAssigned=k;
}
  
int Request::getServerAssigned(){
  return serverAssigned;
}

long Request::getID(){
  return ID;
}

int Request::getID1(){
  return ID1;
}
