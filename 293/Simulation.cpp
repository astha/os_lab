/**********
Implements the methods of the Simulation class. Simulation comprises
of the main event processing loop, and the handling of the events.
 *********/

#include "Event.h"
#include "Simulation.h"
#include "QueueingSystem.h"
#include "declarations.h"
#include "Server.h"
#include "LocatorHeap.h"
#include <fstream>
#include <stack>

using namespace std;

void Simulation::run(){
  Event nextEvent;
  cout << "sim_time  curr_event  next_req(a,s)  num_waiting  next_event"<<endl;
  //Process events until no more events in the event list
  // open files for request and server logs
  fileout.open ("request.log");
	fileout1.open ("server.log");
	
	while (!eventList.empty()) {
    //Pop next event 
    nextEvent = eventList.top();
    eventList.pop();
    //Advance simulation clock to time of this event
    simTime = nextEvent.getTime();
    sumQueueLength+=sys.getPending()*(simTime-lastSimTime);
    lastSimTime=simTime;
    
    // output for event trace
    cout << simTime <<"  ";
    // prints type of event 
    nextEvent.printType(); 
    cout << "   ";
    // prints next request in buffer 
    // if no request then (INF,INF)
    if (sys.getPending()==0) {
      cout << "(INF,INF)";
    }    
    else {
      Request req=sys.nextReq();
      req.printdetail();
    }
    // number of waiting customers
    cout <<"   " << sys.getPending() << "   ";
    
    // print none if eventlist is empty
    if (eventList.empty()) {cout <<"NONE"<<endl;}
    // else print type of next event
    else {eventList.top().printType(); cout<<endl;}
    
    
    //Event handling switch. Call the correct event handler  based on event type.
    if (nextEvent.getType() == ARRIVAL)  
      arrival_event_handler(nextEvent);
    else if (nextEvent.getType() == DEPARTURE){ 
      departure_event_handler(nextEvent);
      //if size=1, it means that only one probe event is left in eventlist
      //because if any other event is present, then it will have a probe event after it
      //this is similar to saying that eventlist always ends with a probe
      //we should not consider this probe
      //simply pop it
      if (eventList.size()==1) {
        Event p=eventList.top();
        eventList.pop();
        //total operating speed should consider the duration between last probe and last departure
        //because probe event is not considered after this
        totalOperatingSpeed+=speed*(simTime-(p.getTime()-probeInterval));
      }
    }
    else if (nextEvent.getType() == PROBE)
      probe_event_handler(nextEvent);
    else if (nextEvent.getType()==TIMEOUT)
      timeout_event_handler(nextEvent);
  }
  
  //Simulation loop over, now calculate and print the metrics
  cout << "Number of requests arrived (including dropped):" <<totalRequests<<endl;
  cout << "Number of requests serviced:"<<customersDeparted<<endl;
  cout << "Number of requests dropped:"<< totalRequests-customersDeparted-timeouts<<endl;
  cout << "Number of customer timeouts:"<<timeouts<<endl;
  cout << "Average waiting time:" << (sumWaitingTimes/customersDeparted)<<endl;
  cout << "Average service time:"<< (sumServiceTimes/customersDeparted)<<endl;
  cout << "90%ile of waiting time:"<< percentile90(&waitingTimes)<<endl;
  cout << "Maximum waiting time:"<<MaxWaitingTime<<endl;
  cout << "Max queue length reached:"<< sys.getMax() <<endl;
  cout << "Average operating speed of the server:"<<totalOperatingSpeed/simTime<<endl;
  cout << "90%ile of speed:"<<percentile90(&serverSpeeds)<<endl;
  double totalUtilisation=0.0;
  for (int j=0; j<numservers; j++) {
    totalUtilisation+=(servers.at(j).getCummulativeBusyPeriod()/simTime);
  }
  cout << "Average server utilization:"<<(totalUtilisation/numservers)<<endl;
  cout << "Average Queue Length:"<<(sumQueueLength/simTime)<<endl;
  //cout << "Avg response time =" << sumRespTimes/customersDeparted << endl;
  
  // close log files
  fileout.close();
  fileout1.close();
    
}


/*****
Arrival event handler first creates request object from the event
information. Then checks if queueing system is busy. If busy,
request must be enqueued if there is space in buffer, else, processing can start
immediately. If request is put in buffer, create a timeout event for that request.
Arrival event handler also looks up the next arrival and
creates the next arrival event.
*****/

void Simulation::arrival_event_handler(Event e) {
  
  //Process the arrival. First create request object from event time,
  //we know the number of service instructions, arrival time, timeout time
  //request ID is the number at which this request has arrived
  Request req(totalRequests,e.getTime(),e.getsTime());
 	totalRequests++;
  
  //Check if system busy
  if (!(this->isBusy())) {
    //If not busy  start service, (Schedule departure event). 
    //Departure event time is current time plus service time
    //ID of departure event is the server at which it is being processed
    //fourth parameter is waiting time, which is 0 in this case
    Event serviceEvent(freeservers.front(),DEPARTURE,simTime+(req.getServiceTime()/speed),0);  
    
    //Add departure event into event list.
    eventList.push(serviceEvent);  
    
    //assign server to request
    req.setServer(freeservers.front());
    
    //set request in server
    servers.at(freeservers.front()).setReqInService(req,speed,simTime);
    
    //set number of instructions in buffer
    servers.at(freeservers.front()).setWork(req.getServiceTime(),simTime);
    
    //remove the server from free server list
    freeservers.pop();
    
    //add 0 waiting time
    addWaitingTime(0.0);
  }

  // If all servers are busy, put the request into buffer list for waiting
  // if there is space in buffer. Create a timeout event for this request
  // else ignore the request
  else {
    if (sys.getPending()<sys.getBufferSize())
    { 
      // timeout event for this request
      // id of time out event is taken from a queue which stores free ids for such requests
      // since departure events have id from 0 to numservers-1
      // arrival event has id = numservers
      // probe has id = numservers+1
      // timeout event ids start from numservers+2
      Event timeoutevent(freeID.front()+numservers+2,TIMEOUT,simTime+e.getTimeOut(),0);
      eventList.push(timeoutevent);
      
      // set this ID for request (ID1)
      req.setID1(freeID.front());
      // remove ID from free Id queue
      freeID.pop();
      // now that we know which ID is linked for this request
      // push this request in TimeOutQueue
      sys.enqueue(req);
    }
  }
  
  //Schedule next arrival - read next arrival and service times and timeout times
  //put into event list   
  double a, s, t;
  cin >> a;
  if (a != -1) {
    cin >> s >> t;
    Event  arrivalEvent(numservers,ARRIVAL,a,s,t);
    eventList.push(arrivalEvent);
  }  
}


/*******
Departure event handler needs to deque and update performance metric
accummulators, and reset system busy state if necessary. If queue is
not empty, departure event for the next request should be created and
added to event list
**********/
void Simulation::departure_event_handler(Event e) {

  //get server at which departure happened
  int k= e.getID();
  
  //get request details from server
  Request reqdone = servers.at(k).getReqInService();
  //set this server as free 
  servers.at(k).setfree();
  //Add this server to free servers queue 
  freeservers.push(k);
  //Update Busy Period for this server
  servers.at(k).addCummulativeBusyPeriod(simTime - (reqdone.getArrivalTime()+e.getsTime()));

  //Update counters, metrics
  customersDeparted++;
  sumRespTimes += e.getTime()- reqdone.getArrivalTime(); 
  sumServiceTimes+=simTime - (reqdone.getArrivalTime()+e.getsTime());
  
  //Print in request.log
  fileout << reqdone.getID() <<" "<< reqdone.getArrivalTime() <<" ";
  fileout << reqdone.getArrivalTime()+e.getsTime() << " " << simTime<<" ";
  fileout << e.getsTime() <<" ";
  fileout << simTime - (reqdone.getArrivalTime()+e.getsTime())<<endl;
  
  //If buffer has pending requests
  if (!sys.bufferEmpty()){
    // return next request and remove it from buffer
    Request reqNext = sys.nextReq(); 
    sys.dequeue();
    
    // remove the timeout event corresponding to this request
    eventList.remove(reqNext.getID1()+numservers+2);
    // add this ID to free IDs available
    freeID.push(reqNext.getID1());
    
    // assign server to the request
    reqNext.setServer(freeservers.front());
    // set request in server 
    servers.at(freeservers.front()).setReqInService(reqNext,speed, simTime);
    
    // Update metrics
    double waiting=simTime-reqNext.getArrivalTime();
    // add to total waiting time
    sumWaitingTimes+=waiting ;
    // update max waiting time
    if (waiting> MaxWaitingTime) {MaxWaitingTime=waiting;}
    // push waiting time in waiting time queue
    addWaitingTime(waiting);
          
    // Create departure event for this request 
    Event serviceEvent(freeservers.front(),DEPARTURE,simTime+(reqNext.getServiceTime()/speed),waiting); 
    
    eventList.push(serviceEvent);  //add to event list
    
    // remove the server assigned from free servers
    freeservers.pop();
  
  }
}


/****
Handles probe events.
if waiting customers is >jhigh, increase system speed
if waiting customers is <jlow, decrease system speed
Update system stats
****/
void Simulation::probe_event_handler(Event e){
  // update metrics
  totalOperatingSpeed+=probeInterval*speed;
  
  //Print in server.log
  fileout1<< (simTime/probeInterval)-1<< " " << sys.getPending();
  fileout1<< " " <<speed<<" ";
  
  // change system speed and update system state
  if (sys.getPending()>jHigh) {increaseSpeed(); }
  else if (sys.getPending()<jLow) {decreaseSpeed();}
  
  // push this speed into speed queue, initial speed was already pushed in constructor function
  serverSpeeds.push(speed);
  
  // print new speed in server.log
  fileout1<<speed<<endl;
  
  // add another probe event to eventlist
  Event probeevent(numservers+1,PROBE,simTime+probeInterval,0);
  eventList.push(probeevent);
  
}

/***
Handles Timeout Events
Whenever a timeout event comes, we can get event ID
This will give request ID corresponding to that event
Remove this request from the queue. TimeOutQueue allows to
remove an element from the queue with guven ID.
Update other metrics
***/
void Simulation::timeout_event_handler(Event e){
  // remove the corresponding request from queue 
  sys.remove(e.getID()-2-numservers);
  // this ID is now free, so push it to free ID queue
  freeID.push(e.getID()-2-numservers);
  // update system state
  sys.decreaseSize();
  timeouts++;
}

bool Simulation::isBusy(){
  // if queue containing indexes of free servers is empty, system is busy  
  return (freeservers.empty()); 
}

  
void Simulation::addWaitingTime(double t){
  // push another waiting time to the waiting time queue
  waitingTimes.push(t);
}

// return 90%ile value of a max priority queue
double Simulation::percentile90(priority_queue<double> * q){
  // if queue is empty
  if ((*q).empty()) {return 0;}
  
  // else remove first 10% elements and return the next element
  int k=(int)(((*q).size())*0.1);
  while(k>0){
    (*q).pop();
    k--;
  }
  return (*q).top();
}
 
// function to increase speed and update servers and eventlist accordingly
// if max speed is reached, dont change speed
void Simulation::increaseSpeed(){
  double newspeed=speed+stepSize;
  // using float because comparisions in double sometimes give wrong results 
  if ((float)newspeed<=(float)maxSpeed){
    speed=newspeed;
    //update server stats
    updateServers();
  }
}

// function to decrease speed and update servers and eventlist accordingly
// if min speed is reached, dont change speed
void Simulation::decreaseSpeed(){
  double newspeed=speed-stepSize;
  if ((float)newspeed>=(float)minSpeed){
    speed=newspeed;
    // update server stats
    updateServers();
  }
}

// Old update server function with prioroty queue implementation of eventlist
/*
void Simulation::updateServers(){
  int k=eventList.size(), i;
  queue<Event> queue1;
  for (i=0; i<k; i++){
    Event e=eventList.top();
    eventList.pop();
    //cout<<i<<endl;
    if (e.getType() != DEPARTURE) {queue1.push(e);}
    else {
      int n=e.getID();
   //   cout<<"ID" <<e.getID()<<endl;
      servers.at(n).update(speed, simTime);
      double t=servers.at(n).getServiceFT();
      e.setTime(t);// update waiting time
    //  cout << "server" << n << " speed " <<servers.at(n).getSpeed()<<endl;
      queue1.push(e);
    }
  }
  for (i=0; i<k; i++){
    eventList.push(queue1.front());
    queue1.pop();
    }
}
*/

// update server stats
void Simulation::updateServers(){
  // for all servers
  for (int i=0; i<numservers; i++){
    if (servers[i].isBusy()){
      // change server speed and time
      // other parameters get updated accordingly
      servers.at(i).update(speed, simTime);
      // get new service finish time
      double t=servers.at(i).getServiceFT();
      // change departure time of the event with this id to new service finish time
      eventList.changeKey(i,t);
    }
  }
}
