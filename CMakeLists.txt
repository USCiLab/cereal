cmake_minimum_required (VERSION 2.6.2)
project (cereal)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g -Wextra -Wshadow -pedantic ${CMAKE_CXX_FLAGS}")

include_directories(./include)

add_executable(sandbox sandbox.cpp)
add_executable(sandbox_json sandbox_json.cpp)
add_executable(sandbox_rtti sandbox_rtti.cpp)
add_executable(sandbox_vs sandbox_vs.cpp)
add_executable(portability_test portability_test.cpp)

find_package(Boost COMPONENTS serialization unit_test_framework)
if(Boost_FOUND)
  add_executable(performance performance.cpp)
  target_link_libraries(performance ${Boost_LIBRARIES})

  enable_testing()
  add_subdirectory(unittests)

endif(Boost_FOUND)
