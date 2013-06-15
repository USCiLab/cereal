all: test.cpp
	g++ -std=c++0x test.cpp -o test -ljsoncpp -I./..

unittests: unittests.cpp
	clang++ -std=c++0x unittests.cpp -o unittests -lboost_unit_test_framework -I./..
	./unittests --show_progress

boost_serialize: boost_serialize.cpp
	g++ -std=c++11 boost_serialize.cpp -o boost_serialize -lboost_serialization -I./..

clean:
	rm test; rm unittests;
