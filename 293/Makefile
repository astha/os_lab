
all:  qsim
qsim:	main.o Event.o Request.o  Server.o QueueingSystem.o Simulation.o
	g++ -g -o qsim main.o Event.o Request.o Server.o QueueingSystem.o Simulation.o
main.o: main.cpp
	g++ -c -g main.cpp
Event.o: Event.cpp Event.h
	g++ -c -g Event.cpp
Request.o: Request.cpp Request.h
	g++ -c -g Request.cpp
Server.o: Server.cpp Server.h
	g++ -c -g Server.cpp
QueueingSystem.o: QueueingSystem.cpp QueueingSystem.h
	g++ -c -g QueueingSystem.cpp
Simulation.o: Simulation.cpp	Simulation.h
	g++ -c -g Simulation.cpp
