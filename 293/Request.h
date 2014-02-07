/*************
Describes a request in a  queueing system. A request is described by
an ID, which server it is assigned to run on (in case multiple
servers), its arrival time and its service time
******************/

#ifndef REQ_H                           // avoid repeated expansion
#define REQ_H

class Request {

 private:
  long ID;                    // ID to uniquely identify the request
  int serverAssigned;         // Which server, in case of multiple servers
  double arrivalTime;         // arrival time
  double serviceTime;         // Time required to process the request
  int ID1;                    // Defined for implementing Timeout requests queue
 
 public:
  //Various accessor functions
  double getArrivalTime();   
  double getServiceTime();
  long getID();
  int getID1();
  int getServerAssigned();

  //assign a server to the request
  void setServer(int k); 
  
  //set ID1 of request
  void setID1(int k);
  
  //Overload the less than operator to compare Requests
  bool operator<(const Request& r) const;
  
  //print arrival and departure time
  void printdetail();
  
  //Constructor: request should always be created with an ID, arrival
  //time, and service time
  Request(){ID=0, arrivalTime=0.0; serviceTime=0.0;};
  Request(int id, double atime, double stime);
};

#endif
