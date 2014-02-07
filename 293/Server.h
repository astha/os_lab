//Generic server of a queueing system
#ifndef SERVER_H                           // avoid repeated expansion
#define SERVER_H

#include "Request.h" 
class Server {


 private:

  int ID;
  bool busy;
  Request reqInService;

  double cummulativeBusyPeriod;
  double serviceFinishTime;
  double work;
  double speed;
  double lastUpdateTime;
 public:
  Server(int id=0, bool b=false) {
    ID = id;
    busy=b;
    cummulativeBusyPeriod=0;
    serviceFinishTime=0.0;
    speed=1.0;
    work=0.0;
  }
    
  bool isBusy();                          // return server state
  Request & getReqInService();            // return current request
  void setReqInService(Request req, double sp, double time);      // set request in process
  void setID(int id);                     // set server id 
  void setfree();                         // set the server state to free
  void setServiceFT(double t);            // set service finish time
  double getServiceFT();                  // access service finish time
  void setWork(double w, double time);    // set work to be done
  void setSpeed(double s);                // set speed
  void update(double sp,double time);     // update server state
  double getSpeed();                      // get server speed
  
  // add to busy period of server
  // stores how long was the server in use
  void addCummulativeBusyPeriod(double t);
  // access busy period
  double getCummulativeBusyPeriod();
};

#endif
