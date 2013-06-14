all: test.cpp unittests
	g++ -std=c++0x test.cpp -o test -ljsoncpp -I./..

unittests: unittests.cpp
	g++ -std=c++0x unittests.cpp -o unittests -lboost_unit_test_framework -I./..
	./unittests --show_progress

