all: test.cpp
	g++ -std=c++0x test.cpp -o test -ljsoncpp -I./..

