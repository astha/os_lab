/*************************
Event class describes a simulation "event". A simulation "event" is
defined by an ID, a type and a time at which the event is scheduled.
**********************/

#ifndef EVENT_H                           // avoid repeated expansion
#define EVENT_H
#include <iostream>
using namespace std;

//Enumerated type and namespace to use mnemonics for Event types
namespace EventTypeNames { enum eventType {IO_START,IO_END,CPU_START,CPU_END};}
using namespace EventTypeNames;

class Event {

 public:

  double start_time;    //time of event
  int process_id;
  eventType type; // event type
  // double stime;   //stores service time for arrivals and waiting time for departures
  // double timeout; //stores the timeout for arrival events
 
  //default constructor, creates an arrival event at time 0
  Event() { process_id= 0; type=CPU_START; start_time=0.0; }  
  
  // constructor with 5 parameters, 5th parameter is timeout time 
  Event(int id, eventType ty, double ti);  
  
  // //constructor with 4 parameters
  // Event(int id, eventType ty, double ti, double st);
  
  bool operator<(const Event& e) const;   //overload "<" to compare events
  void print(ostream & outstream=cout) const; //forgot why I needed this
  
  // double getKey();                          //accessor method
  // int getID() const;                        //accessor method
  // double getTime() const;                   //accessor method
  // double getTimeOut() const;                //accessor method
  // int getType() const;                      //accessor method
  // double getsTime() const;                  //accessor method
  
  // // Writing methods
  // void setTime(double t);                   //set time of event  
  // void setKey(double t);                    //set key of event
  
  // void printType() const;                   //print type of event
};


#endif
