#include "QueueingSystem.h"

void QueueingSystem::enqueue(Request r) {
  // if there is space for more requests in buffer
  // add it to buffer queue
  // else ignore the request
    buffer.push(r);
    pending++;
  
  // update maximum queue length reached
  if (pending>MaxQueueLength) {MaxQueueLength=pending;}
  
}  

Request QueueingSystem::nextReq() {

  //Returns request at the front of the queue
  Request r = buffer.front();
 // cout <<"ID in next req"<< r.getID1()<<endl;
    
  return r;
}

Request QueueingSystem::dequeue() {

  //Removes request at the front of the queue 
  Request r = buffer.pop();
  pending--;
  return r;
}

bool QueueingSystem::bufferEmpty(){ 
  //is buffer empty or not
  return (buffer.empty()); 
}


int QueueingSystem::getMax(){
  //return maximum buffer length reached
  return MaxQueueLength;
}
  
void QueueingSystem::setSize(int k) {
  //set maximum buffer size
  buffersize=k;
  //set size in TimeOut Queue
  buffer.setnmax(k+1);
}
 // get number of pending requests 
int QueueingSystem::getPending(){
  return pending;
}

//return max posible size of buffer
int QueueingSystem::getBufferSize(){
  return buffersize;
}

//remove element from queue with given key
void QueueingSystem::remove(int k){
  buffer.remove(k);
}

// when request is timed out and removed, set number of pending requests 
void QueueingSystem::decreaseSize(){
  pending--;
}  
