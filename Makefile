all: kidshell helloworld

helloworld: helloWorld.cpp
	g++ helloWorld.cpp -o helloworld 

kidshell: kidshell.cpp
	g++ kidshell.cpp -o shell