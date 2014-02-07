#include "Server.h"

// to check if server is busy, not used in current code
bool Server::isBusy(){
  return busy;
}  
  
// return current request
Request& Server:: getReqInService(){
  return reqInService;
}

// set request in process 
void Server::setReqInService(Request req,  double sp, double time){
  reqInService=req;
  lastUpdateTime=time;
  busy=true;
  work=req.getServiceTime();
  speed=sp;
  serviceFinishTime=time + work/speed;

}

// set server ID
void Server::setID (int id){
  ID=id;
}

//set the server as free
void Server::setfree(){
  busy=false;
  work=0;
  lastUpdateTime=serviceFinishTime;
}

// set service finish time
void Server::setServiceFT(double t){
  serviceFinishTime=t;
}

// get service finish time
double Server::getServiceFT() {
  return serviceFinishTime;
}

// get speed
double Server::getSpeed() {
  return speed;
}
  
// set nummber of instructions to be processed
void Server::setWork(double w, double time){
  work=w;lastUpdateTime=time;serviceFinishTime=w/speed;
}

// set speed of server
void Server::setSpeed(double s) {
  speed=s;
}

// update server state
void Server::update(double sp,double time) {
  // update number of instructions left to process
  double workdone=speed*(time-lastUpdateTime);
  work = work-workdone;
  // update time
  lastUpdateTime=time;
  // updte speed
  speed=sp;
  // update service finish time
  serviceFinishTime=time+ (work/speed);
}

// add to busy period of server
// stores how long was the server in use
void Server::addCummulativeBusyPeriod(double t){
  cummulativeBusyPeriod+=t;
}
  
// access busy period
double Server::getCummulativeBusyPeriod(){
  return cummulativeBusyPeriod;
}
  
