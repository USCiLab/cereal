cmake_minimum_required (VERSION 2.6.2)
project (cereal)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g -Wextra -Wshadow -pedantic ${CMAKE_CXX_FLAGS}")

include_directories(./include)

add_subdirectory(sandbox)

find_package(Boost COMPONENTS serialization unit_test_framework)
if(Boost_FOUND)
  add_executable(performance performance.cpp)
  target_link_libraries(performance ${Boost_LIBRARIES})

  enable_testing()
  add_subdirectory(unittests)
endif(Boost_FOUND)
