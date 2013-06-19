CPPFLAGS=-std=c++11 -I./include -Wall -Werror
CC=g++

all: unittests sandbox performance

sandbox: sandbox.cpp
	${CC} sandbox.cpp -o sandbox ${CPPFLAGS}

unittests: unittests.cpp
	time ${CC} unittests.cpp -o unittests -lboost_unit_test_framework ${CPPFLAGS}
	time ./unittests --show_progress

performance: performance.cpp
	${CC} performance.cpp -o performance -lboost_serialization ${CPPFLAGS} -O3

clean:
	rm sandbox; rm unittests; rm performance;
