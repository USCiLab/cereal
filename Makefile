all: test.cpp cereal.hpp
	g++ -std=c++0x test.cpp -o test -ljsoncpp
