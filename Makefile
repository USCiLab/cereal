CPPFLAGS=-std=c++11 -I./include -Wall -Werror -g -Wextra -Wshadow -pedantic
CXX=g++
COVERAGE_OUTPUT=out

all: unittests sandbox sandbox_vs performance sandbox_rtti sandbox_json

sandbox: sandbox.cpp
	${CXX} sandbox.cpp -o sandbox ${CPPFLAGS}

sandbox_json: sandbox_json.cpp
	${CXX} sandbox_json.cpp -o sandbox_json ${CPPFLAGS}

sandbox_rtti: sandbox_rtti.cpp
	${CXX} sandbox_rtti.cpp -o sandbox_rtti ${CPPFLAGS} -O3

sandbox_vs: sandbox_vs.cpp
	${CXX} sandbox_vs.cpp -o sandbox_vs ${CPPFLAGS}

unittests: unittests.cpp
	${CXX} unittests.cpp -o unittests -lboost_unit_test_framework ${CPPFLAGS}
	./unittests --show_progress

performance: performance.cpp
	${CXX} performance.cpp -o performance -lboost_serialization ${CPPFLAGS} -O3

portability: portability_test.cpp
	${CXX} portability_test.cpp -o portability64 ${CPPFLAGS}
	${CXX} portability_test.cpp -o portability32 ${CPPFLAGS} -m32
	./portability64 save 64
	./portability32 load 32
	./portability32 save 32
	./portability64 load 64
	./portability64 remove 64

.PHONY: coverage
coverage:
	g++ -std=c++11 -I./include -Wall -Werror -g -O0 -coverage  unittests.cpp -o unittests_coverage -lboost_unit_test_framework
	./unittests_coverage --show_progress
	lcov --capture --directory . --output-file coverage.info --no-external
	lcov --remove coverage.info '*/external/*' 'cereal/details/util.hpp' 'unittests.cpp' -o coverage.info
	genhtml --demangle-cpp coverage.info --output-directory ${COVERAGE_OUTPUT}

.PHONY: doc
doc:
	@doxygen ./doc/doxygen.cfg

.PHONY: clean
clean:
	-@rm *.o sandbox sandbox_vs unittests performance sandbox_rtti sandbox_json 2>/dev/null || true
