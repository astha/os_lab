#include "Event.h"
#include <iostream>

using namespace std;

// constructor functions
Event::Event(int id, eventType ty, double ti) {
  process_id=id; type=ty; time=ti; 
}

// Event::Event(int id=0, eventType ty=ARRIVAL, double ti=0.0, double st=0.0) {
//   ID=id; type=ty; time=ti; stime=st; timeout=0.0;
// }


//Overload "less than" operator
//Less then is redefined to make it work with the STL priority queue
//which is a max-heap. We want min-heap.
//This will get the needed effect.
bool Event::operator<(const Event& e) const  {
  if (time!=e.time) {return (time > e.time);}
  else if (type == CPU_END) return false;
  else return true	;
}

// print event details
void Event::print(ostream & outstream) const {
  outstream << "ID: " << process_id << ", type=" << type << ", time=" << time << endl;
}
 
// //print type of event 
// void Event::printType() const{
//   switch(type){
//     case (0) :
//       cout << "ARRIVAL";
//       break;
//     case (1) :
//       cout << "DEPARTURE";
//       break;
//     case (2) :
//       cout << "PROBE";
//       break;
//     case (3) :
//       cout << "TIMEOUT";
//       break;
//   }
// }

// // accessor methods  
// double Event::getTime() const {
//   return time;
// }

// int Event::getType() const {
//   return type;
// }

// double Event::getsTime() const {
//   return stime;
// }

// int Event::getID() const {
//   return ID;
// }

// double Event::getTimeOut() const {
//   return timeout;
// }

// double Event::getKey(){ 
//   return time;
// }

// // writing methods
// void Event::setTime(double t){
//   time=t;
// }

// void Event::setKey(double t){
//   time=t;
// }

