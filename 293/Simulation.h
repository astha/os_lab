/*****************

Main Simulation class. Comprises of an event list, a current
simulation clock, a queueing system that is being simulated and some
counters and metrics.

************/

#ifndef SIMULATION_H                           // avoid repeated expansion
#define SIMULATION_H

#include<queue>
#include <vector>
#include<map>
#include "Event.h"
#include "QueueingSystem.h"
#include "Server.h"
#include <fstream>
#include "LocatorHeap.h"

class Simulation {

 private:
  LocatorHeap<Event> eventList; 

  double simTime;                  //Simulation clock
  double lastSimTime;              //Last time when event occured
  int numservers;                         //Number of servers
  double minSpeed, maxSpeed, speed;       //Speed of servers, system speed
  double stepSize, probeInterval;         //Stepsize and probe interval
  int jLow, jHigh;                        //min and max size of waiting queue when probe should change speed
  
  //performance metrics
  int totalRequests;               //Total number of requests arrived
  double sumRespTimes;             //Sum of response times
	double sumWaitingTimes;		       //Sum of waiting times
  double sumServiceTimes;          //Sum of service times
  double MaxWaitingTime;           //Maximum waiting time of a customer
  double totalOperatingSpeed;      //summation of time interval and speed during that time
  double sumQueueLength;           //Weighted sum of Waiting queue lengths
  int timeouts;                    // how many timeouts
  int customersDeparted;           //How many customers have finished service
  
  queue<int> freeservers;          //Index of the servers that are free	   
  vector<Server> servers;          //Vector containing servers
  priority_queue<double> waitingTimes;//Queue containing all waiting times  
  priority_queue<double> serverSpeeds;//Queue containing all service times
  QueueingSystem sys;              //The system being simulated
  ofstream fileout;                //For file output, request.log 
  ofstream fileout1;              //For file output, request.log
	queue<int> freeID;
 public:
  //Constructor, initializes the simulation.
  Simulation() {
    double a, s, t;
//    int numservers, size, jLow, jHigh;
    int size;
    cin >> numservers >> size;
    cin >> minSpeed >> maxSpeed >> stepSize >> probeInterval;
    cin >> jLow >> jHigh;
    speed=minSpeed;
    sys.setSize(size);
    eventList.setnmax(numservers+size+3);
    // set size of server list to number of servers
    // initialise server IDs and all servers are empty servers
    servers.resize(numservers);
    for (int i=0; i<numservers; i++){
      freeservers.push(i);
      servers.at(i).setID(i);
      servers.at(i).setSpeed(minSpeed);
    }
    for (int j=0; j<size; j++){
      freeID.push(j);
    }
    serverSpeeds.push(speed);
    totalOperatingSpeed=0.0;
    //Initialize clock and all counters/accummulators 
    simTime = 0.0;  
    totalRequests=0;                
    sumRespTimes=0.0;
    customersDeparted =0;
    sumWaitingTimes=0.0;
    sumServiceTimes=0.0;
    MaxWaitingTime=0.0;
    lastSimTime=0.0;
    timeouts=0;
    sumQueueLength=0.0;
    //create the first arrival event, by reading standard input for an arrival
    //time and a service time
    cin >> a >> s >> t;
    //a=-1 indicates end of input
    if (a != -1) {
      static Event e(numservers,ARRIVAL,a,s,t); 
      /*create an event with ID 0, as
				this will be the first event, and
				we know it is an ARRIVAL event at 
				time "a" as was read from
				input. This needs to be "static"
				because the Event just created
				should live after function exits. */ 
      
      eventList.push(e);          //push it into the priority queue. 
      // add a probe event
      static Event f(numservers+1,PROBE,probeInterval,0,0);
      eventList.push(f);
      // only one arrival/probe event at a time
      // and maximum 'numservers' departure events
      // hence ID of dep events ranges from 0 to numservers-1
      // depending on which server they depart from
      // and ID of probe/arrival event is constant
    }
  }

  void run();                     // Will start the event handling
  bool isBusy();                  // Are all servers busy or not
  void addWaitingTime(double t);  // Add another waiting time
  double percentile90(priority_queue<double> *q);             // Find 90%ile of waiting time
  void increaseSpeed();
  void decreaseSpeed();
  void updateServers();
  
  //Event handlers
  void arrival_event_handler(Event e);   
  void departure_event_handler(Event e);
  void probe_event_handler(Event e);
  void timeout_event_handler(Event e);
};
  
#endif
  
  
