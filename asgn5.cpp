#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
#include "Event.h"
using namespace std;

enum state {RUNNING,READY,BLOCKED,TERMINATED};

struct process_phase{
	int iterations;
	int cpu_time;
	int io_time;
};

class process{
public:
	int p_id;
	int start_priority;
	int admission;
	state status;
	vector<process_phase> phases;
	int currentPhase ;
	int start_time;
	int currentCPUTime;
	int currentIOTime;

	bool isReady(){
		return status == READY;
	}
	int getPhaseCPUTime (){
		return phases[currentPhase].cpu_time;
	}

	void preempt(int time){
		//cout<< "T = "<< p_id<<" "  << time << start_time <<  currentCPUTime << endl;
		currentCPUTime -= (time - start_time);
	}
	int getPhaseIOTime (){
		return currentIOTime;
	}

	void reduceIterations(int t){
		currentCPUTime = phases[currentPhase].cpu_time;
		currentIOTime = phases[currentPhase].io_time;
		start_time = t;
		//cout<< "E = "<< p_id<<" " << start_time << " " << currentCPUTime << " " <<currentPhase << endl;		
		
		phases[currentPhase].iterations --;
		if (phases[currentPhase].iterations == 0){
			currentPhase ++;
		}
	}

	bool hasMoreIterations(){
		//printf("IDHAR =  %d %d\n",currentPhase,phases[currentPhase].iterations);
		if ((currentPhase >= phases.size())) {
			return false;
		}
		else 
			return true;
	}

	bool hasMoreIterations2(){
		//printf("IDHAR =  %d %d\n",currentPhase,phases[currentPhase].iterations);
		if ((currentPhase >= phases.size() - 1) && phases[currentPhase].iterations == 0) {
			return false;
		}
		else 
			return true;
	}

};

struct sc_level{
	int level_number;
	int priority;
	int time_slice;
};

struct scheduler{
	int no_levels;
	vector<sc_level> levels;
};

struct cpu{
	process p;
	bool busy;
	sc_level s;
	int scheduler_time_next;
	int next_time_slot;
};

int runningProcess;
vector<process> process_list;
scheduler my_scheduler;
priority_queue<Event> event_list ;
int present_time;
cpu c;

void process_proc_file(){
	string line, line2;
	int pid, prior;
	int adm;
	int iter;
	int cpu_t, io_t;
	ifstream infile("PROCESS_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="PROCESS"){
			process proc;
			getline(infile, line2);
			std::istringstream iss(line2);
		    if (!(iss >> pid >> prior >> adm)) { break; } // error
		    // cout<<pid<<endl<<prior<<endl;

		    proc.p_id = pid;
		    proc.start_priority = prior;
		    proc.admission = adm;
		    proc.currentPhase = 0;

		    getline(infile, line2);
		    while(line2 != "END"){
		    	std::istringstream iss(line2);
		    	process_phase pp;
			    if (!(iss >> iter >> cpu_t >> io_t)) { break; } // error
			    // cout<<cpu_t<<endl<<io_t<<endl;
			    
			    pp.iterations = iter;
			    pp.cpu_time = cpu_t;
			    pp.io_time = io_t;
			    // pp.status = READY;
			    (proc.phases).push_back(pp);
			    getline(infile, line2);
			}
			proc.currentCPUTime = proc.phases[0].cpu_time;

			process_list.push_back(proc);
		}
	}
	// cout<<(process_list[1].phases[1]).io_time<<endl;
	// return 0;
}

void process_scheduler_file(){
	string line, line2;
	int level_count;
	int prior;
	int s_lvl;
	int t_slice;
	ifstream infile("SCHEDULER_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="SCHEDULER"){
			getline(infile, line2);
			std::istringstream iss(line2);
		    if (!(iss >> level_count)) { break; } // error
		    // cout<<pid<<endl<<prior<<endl;

		    my_scheduler.no_levels = level_count;
		    for(int i=0; i<level_count; i++){
		    	getline(infile, line2);
		    	std::istringstream iss(line2);
				if (!(iss >> s_lvl >> prior >> t_slice)) { break; } // error
				sc_level scl;
				scl.level_number = s_lvl;
				scl.priority = prior;
				scl.time_slice = t_slice;
				my_scheduler.levels.push_back(scl);
			}
		}
	}
}

void processEventListMulti(){
	// int count = 50;	
	while (!event_list.empty() ){
		// cout << "ASTHA" << endl;
		Event e = event_list.top();
		printf("EVENT = %d %d %lf\n",e.process_id,e.type,e.time);
		
		present_time = e.time;

		eventType type = e.type;
		int id = e.process_id;
		if (type == LEVEL_END){
			c.s = my_scheduler.levels[id];
			c.scheduler_time_next += c.s.time_slice;
		}
		else if (type == CPU_START) {
			// if (process_list[id-1].start_priority < c.s.priority){
			// 	Event k(id, CPU_START, c.scheduler_time_next);
			// 	event_list.push(k);
			// }
			//else{
			if (c.busy){
				if (c.p.start_priority < process_list[id-1].start_priority){
					// run this process
					Event k(id, CPU_END, present_time + process_list[id-1].getPhaseCPUTime());
					event_list.push(k);

					Event k1(c.p.p_id, CPU_CONT, present_time + process_list[id-1].getPhaseCPUTime());
					process_list[c.p.p_id -1].preempt(present_time);
					process_list[id-1].reduceIterations(present_time);
					c.p = process_list[id -1];
					event_list.push(k1);
				}
				else {
						// printf("HERE = %d %d %d\n",process_list[c.p.p_id-1].currentCPUTime,present_time,process_list[c.p.p_id-1	].start_time);
						//exit(0);
					Event k(id, CPU_START, process_list[c.p.p_id -1 ].currentCPUTime  + process_list[c.p.p_id-1].start_time);
					event_list.push(k);
						//printf("HERE\n");
				}

			}
			else {
				c.p = process_list[id -1];
				c.busy = true;
				Event k(id, CPU_END, present_time + process_list[id-1].getPhaseCPUTime());
				event_list.push(k);
				process_list[id-1].reduceIterations(present_time);
			}
			//}
			c.busy = true;
		}
		else if (type == CPU_CONT){
			// if (process_list[id-1].start_priority < c.s.priority){
			// 	Event k(id, CPU_START, c.scheduler_time_next);
			// 	event_list.push(k);
			// }
			// else {

			if (c.busy){
				if (c.p.start_priority < process_list[id-1].start_priority){
					// run this process
					process_list[c.p.p_id-1].preempt(present_time);
					Event k(id, CPU_END, present_time + process_list[id-1].currentCPUTime);
					event_list.push(k);
					process_list[id - 1].start_time = present_time;
					Event k1(c.p.p_id, CPU_CONT,present_time + process_list[id-1].currentCPUTime);
					event_list.push(k1);
					c.p = process_list[id -1];
				}
				else {
					Event k(id, CPU_CONT, process_list[c.p.p_id -1 ].currentCPUTime  + process_list[c.p.p_id-1].start_time);
					event_list.push(k);
				}

			}
			else {
				c.p = process_list[id -1];
				c.busy = true;
					//cout << "TIME = " << process_list[id-1].currentCPUTime << endl;
				Event k(id, CPU_END, present_time + process_list[id-1].currentCPUTime);
				event_list.push(k);
				process_list[id-1].start_time = present_time;
			}
			//}
			c.busy = true;

		}
		else if (type == CPU_END){
			c.busy = false;
			if (c.p.p_id == id && process_list[id -1].hasMoreIterations()){
				Event e(id, CPU_START, present_time + process_list[id-1].getPhaseIOTime());
				event_list.push(e);
			}
		}
		event_list.pop();

	}

}

int getNextProcess(int currentProcess){
	int n = process_list.size();
	currentProcess--;
	for (int i = 1; i <= n ; i++){

		if (process_list[(i+currentProcess)%n].isReady()){
			return ((i+currentProcess)%n + 1);
		}
	}
	return -1;
}


void processEventListTimeSharing(){
	int time_slice = my_scheduler.levels[0].time_slice;
	int count = 50;	
	while (!event_list.empty() && count-- ){
		// cout << "ASTHA" << endl;
		Event e = event_list.top();
		event_list.pop();
		printf("EVENT = %d %s %lf\n",e.process_id,e.printType().c_str(),e.time);
		
		present_time = e.time;

		eventType type = e.type;
		int id = e.process_id;
		// if (type == LEVEL_END){
		// 	c.s = my_scheduler.levels[id];
		// 	c.scheduler_time_next += c.s.time_slice;
		// }
		if (type == CPU_START) {
			// if (process_list[id-1].start_priority < c.s.priority){
			// 	Event k(id, CPU_START, c.scheduler_time_next);
			// 	event_list.push(k);
			// }
			//else{
			if (c.busy){
				process_list[id-1].status = READY;
			}
			else {
				runningProcess = id;
				c.p = process_list[id-1];
				c.busy = true;
				//int time_required = ;
				process_list[id-1].start_time = present_time;
				process_list[id-1].status = RUNNING;
				printf("HEREHERE = %d\n",process_list[id-1].currentCPUTime );
				if (time_slice >= process_list[id-1].currentCPUTime){
					Event k(id, CPU_END, present_time + process_list[id-1].currentCPUTime);
					event_list.push(k);
					process_list[id-1].currentCPUTime = 0;
				}
				else {
					Event k(id, TIME_SLICE, present_time + time_slice);
					event_list.push(k);
					process_list[id-1].currentCPUTime -= time_slice;
					//printf("HERE = %d\n",process_list[id-1].currentCPUTime );
				}
			}			
		}
		else if (type == TIME_SLICE) {
			if (c.busy){
				//process_list[runningProcess-1].preempt(present_time);
				process_list[id-1].status = READY;
			}
			int new_id = getNextProcess(runningProcess);
			if (new_id==-1){
				//event_list.pop();
				continue;
			}
			else{
				Event k(new_id, CPU_START, present_time);
				event_list.push(k);

			}
			c.busy = false;			
		}
		else if (type == CPU_END){
			c.busy = false;
			process_list[id-1].status= BLOCKED;
			process_list[id-1].phases[process_list[id-1].currentPhase].iterations --;
			//printf("E %d  %d\n",c.p.p_id, process_list[id -1].hasMoreIterations2());
			if (c.p.p_id == id && process_list[id -1].hasMoreIterations2()){
				printf("G = %d\n", present_time + process_list[id-1].phases[process_list[id-1].currentPhase].io_time);
				
				Event e(id, CPU_START, present_time + process_list[id-1].phases[process_list[id-1].currentPhase].io_time);
				event_list.push(e);
			}

			if (process_list[id-1].phases[process_list[id-1].currentPhase].iterations != 0){
			process_list[id-1].currentCPUTime = process_list[id-1].phases[process_list[id-1].currentPhase].cpu_time;
				}
				else {

				process_list[id-1].currentCPUTime = process_list[id-1].phases[process_list[id-1].currentPhase+1].cpu_time;
				process_list[id-1].currentPhase ++;	
			}


			int new_id = getNextProcess(runningProcess);
			//printf("HERE = %d\n",new_id);
			if (new_id==-1){
				//event_list.pop();
				continue;
			}
			else{
				Event k(new_id, CPU_START, present_time);
				event_list.push(k);

			}

		}
		
	}
}

int main(int argc, char* argv[]){
	c.busy = false;
	process_proc_file();

	process_scheduler_file();
	c.s = my_scheduler.levels[0];
	c.scheduler_time_next = my_scheduler.levels[0].time_slice;
	present_time = 0;
	for (int i = 0 ; i < process_list.size(); i++){
		Event e(process_list[i].p_id, CPU_START, process_list[i].admission );
		event_list.push(e);
	}
	int t = 0;
	for (int i = 0; i < my_scheduler.levels.size(); i++){
		Event e(i+1 , LEVEL_END, t+my_scheduler.levels[i].time_slice);
		t += my_scheduler.levels[i].time_slice;
		//event_list.push(e);
	}

	//cout << "ASTHA" << endl;
	if (argc == 1 ) processEventListMulti();

	else if (argc == 2) processEventListTimeSharing();

	// else if (argc == 3) processEventListPriority();

	return 0;




}