downgrader: main.o
	g++ main.o -lalpm  -o downgrader

main.o: functions.cc http.cc interface.cc main.cc var.h
	g++ -g -c main.cc -o main.o
