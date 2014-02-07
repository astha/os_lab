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

	
	int getPhaseCPUTime (){
		return phases[currentPhase].cpu_time;
	}

	void preempt(int time){
		currentCPUTime -= (time - start_time);
	}
	int getPhaseIOTime (){
		return phases[currentPhase].io_time;
	}
	void reduceIterations(int t){
		currentCPUTime = phases[currentPhase].cpu_time;
		start_time = t;
		phases[currentPhase].iterations --;
		if (phases[currentPhase].iterations == 0){
			currentPhase ++;
		}
	}

	bool hasMoreIterations(){
		if ((currentPhase >= phases.size()-1) && phases[currentPhase].iterations == 0){
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
};

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


void processEventList(){
	while (!event_list.empty()){
		Event e = event_list.top();
		present_time = e.time;

		eventType type = e.type;
		int id = e.process_id;
		if (type == LEVEL_END){
			c.s = my_scheduler.levels[id];
		}
		else if (type == CPU_START){
			c.busy = true;
			if (c.busy){
				if (c.p.start_priority < process_list[id-1].start_priority){
					// run this process
					Event k(id, CPU_END, present_time + process_list[id-1].getPhaseCPUTime());
					event_list.push(k);
					process_list[id-1].reduceIterations(present_time);
					Event k1(c.p.p_id, CPU_CONT, process_list[id].currentCPUTime - (present_time - process_list[id].start_time));
					c.p = process_list[id -1];
					event_list.push(k1);
				}
				else {
					Event k(id, CPU_START, process_list[c.p.p_id].currentCPUTime - (present_time - process_list[c.p.p_id].start_time));
				}

			}
			else {
				c.p = process_list[id -1];
				c.busy = true;
				Event k(id, CPU_END, present_time + process_list[id-1].getPhaseCPUTime());
				event_list.push(k);
				process_list[id-1].reduceIterations(present_time);
			}

		}
		else if (type == CPU_CONT){
			c.busy = true;
			if (c.busy){
				if (c.p.start_priority < process_list[id-1].start_priority){
					// run this process
					Event k(id, CPU_END, present_time + process_list[id-1].currentCPUTime);
					event_list.push(k);
					Event k1(c.p.p_id, CPU_CONT, process_list[id].currentCPUTime - (present_time - process_list[id].start_time));
					event_list.push(k1);
					c.p = process_list[id -1];
				}
				else {
					Event k(id, CPU_CONT, process_list[c.p.p_id].currentCPUTime - (present_time - process_list[c.p.p_id].start_time));
					event_list.push(k);
				}

			}
			else {
				c.p = process_list[id -1];
				c.busy = true;
				Event k(id, CPU_END, present_time + process_list[id-1].currentCPUTime);
				event_list.push(k);
				process_list[id-1].start_time = present_time;
			}
		}
		else if (type == CPU_END){
			c.busy = false;


		}
		event_list.pop();
	}

}

int main(){
	c.busy = false;
	//c.p = NULL;
	process_proc_file();
	cout<<(process_list[1].phases[1]).io_time<<endl;
	process_scheduler_file();
	cout<<my_scheduler.levels[1].time_slice<<endl;
	c.s = my_scheduler.levels[0];
	
	present_time = 0;
	for (int i = 0 ; i < process_list.size(); i++){
		Event e(process_list[i].p_id, CPU_START, process_list[i].admission );
		event_list.push(e);
	}
	int t = 0;
	for (int i = 0; i < my_scheduler.levels.size(); i++){
		Event e(i+1 , LEVEL_END, t+my_scheduler.levels[i].time_slice);
		t += my_scheduler.levels[i].time_slice;
	}

	processEventList();






}