downgrader: main.o
	g++ main.o cJSON.c -lalpm -lcurl -o downgrader

main.o: main.cc class_Interface.h class_Actions.h
	g++ -g -c main.cc -o main.o
