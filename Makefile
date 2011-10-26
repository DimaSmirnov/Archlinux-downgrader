downgrader: main.o
	g++ main.o -lalpm  -o downgrader

main.o: main.cc class_Interface.h class_Actions.h
	g++ -g -c main.cc -o main.o
	
install: downgrader
	cp downgrader /usr/bin/downgrader
