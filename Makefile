CPPFLAGS=-std=c++11 -I./include
CC=g++

all: unittests sandbox performance

sandbox: sandbox.cpp
	${CC} sandbox.cpp -o sandbox ${CPPFLAGS}

unittests: unittests.cpp
	${CC} unittests.cpp -o unittests -lboost_unit_test_framework ${CPPFLAGS}
	./unittests --show_progress

performance: performance.cpp
	${CC} performance.cpp -o performance -lboost_serialization ${CPPFLAGS} -O3

clean:
	rm sandbox; rm unittests; rm performance;
