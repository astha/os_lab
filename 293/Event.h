/*************************
Event class describes a simulation "event". A simulation "event" is
defined by an ID, a type and a time at which the event is scheduled.
**********************/

#ifndef EVENT_H                           // avoid repeated expansion
#define EVENT_H
#include <iostream>
using namespace std;

//Enumerated type and namespace to use mnemonics for Event types
namespace EventTypeNames { enum eventType {ARRIVAL,DEPARTURE,PROBE,TIMEOUT};}
using namespace EventTypeNames;

class Event {

 private:
  int ID;         // event ID
  eventType type; // event type
  double time;    //time of event
  double stime;   //stores service time for arrivals and waiting time for departures
  double timeout; //stores the timeout for arrival events
 
 public:
  //default constructor, creates an arrival event at time 0
  Event() {ID=0; type=ARRIVAL; time=0.0; stime=0.0; timeout=0.0;}  
	
	// constructor with 5 parameters, 5th parameter is timeout time 
	Event(int id, eventType ty, double ti, double st,double to);  
	
	//constructor with 4 parameters
  Event(int id, eventType ty, double ti, double st);
  
  bool operator<(const Event& e) const;   //overload "<" to compare events
  void print(ostream & outstream=cout) const; //forgot why I needed this
  
  double getKey();                          //accessor method
  int getID() const;                        //accessor method
  double getTime() const;                   //accessor method
  double getTimeOut() const;                //accessor method
  int getType() const;                      //accessor method
  double getsTime() const;                  //accessor method
  
  // Writing methods
  void setTime(double t);                   //set time of event  
  void setKey(double t);                    //set key of event
  
  void printType() const;                   //print type of event
};


#endif
