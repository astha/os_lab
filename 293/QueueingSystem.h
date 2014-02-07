/*********
QueueingSystem is the class that captures all aspects of the queueing
system being simulated. A queueing system is defined by

a buffer for queueing requests
the server state
the performance metrics associated with the system

Operations are to enqueue, dequeue and setting the metrics
************/
#ifndef QS_H                           // avoid repeated expansion
#define QS_H

#include<queue>                        //STL queue template
using namespace std;              
#include "Request.h"                   //defines Request class
#include "TimeOutQueue.h"

class QueueingSystem {             
  
 private:
  TimeOutQueue buffer;      // buffer is an STL queue of Requests
  int buffersize;             // max buffer size
  int pending;                // number of pending requests in buffer (actual size of buffer)
  int MaxQueueLength;         // maximum queue length reached
 
 public:
  //add request to buffer if there is empty space in the buffer
  void enqueue(Request r);
  
  //is buffer empty or not
  bool bufferEmpty();     // is buffer emty or not
  
  Request dequeue();      // remove first request in buffer
  Request nextReq();      // return first request in buffer
  int getMax();           // return maximum queue length reached
  void setSize(int k);    // set maximum buffer size
  int getPending();       // returns number of pending requests in buffer
  int getBufferSize();    // returns maximum buffersize possible 
  
  //Constructor that initiates empty queueing system
  QueueingSystem() {
    pending=0; 
    MaxQueueLength=0;
  } 
  
  // remove element with certain key
  void remove(int k);
  
  // when request is timed out and removed, set number of pending requests
  void decreaseSize();
};
#endif  
