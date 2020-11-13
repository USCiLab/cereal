if(CMAKE_VERSION LESS 3.0)
  message(FATAL_ERROR "Cereal can't be installed with CMake < 3.0")
endif()

get_filename_component(BINARY_DIR ${CMAKE_BINARY_DIR}/build ABSOLUTE)
get_filename_component(INSTALL_DIR ${CMAKE_BINARY_DIR}/out ABSOLUTE)

# cmake configure step for cereal
file(MAKE_DIRECTORY ${BINARY_DIR}/cereal)
execute_process(
  COMMAND ${CMAKE_COMMAND}
    -DJUST_INSTALL_CEREAL=1
    -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
  WORKING_DIRECTORY ${BINARY_DIR}/cereal
  RESULT_VARIABLE result
)
if(result)
  message(FATAL_ERROR "Cereal cmake configure-step failed")
endif()

# cmake install cereal
execute_process(
  COMMAND ${CMAKE_COMMAND}
    --build ${BINARY_DIR}/cereal
    --target install
  RESULT_VARIABLE result
)
if(result)
  message(FATAL_ERROR "Cereal cmake install-step failed")
endif()

# create test project sources
file(WRITE ${BINARY_DIR}/test_source/CMakeLists.txt "
  cmake_minimum_required(VERSION ${CMAKE_VERSION})
  project(cereal-test-config-module)
  if(NOT MSVC)
      if(CMAKE_VERSION VERSION_LESS 3.1)
          set(CMAKE_CXX_FLAGS \"-std=c++11 \${CMAKE_CXX_FLAGS}\")
      else()
          set(CMAKE_CXX_STANDARD 11)
          set(CMAKE_CXX_STANDARD_REQUIRED ON)
      endif()
  endif()
  find_package(cereal REQUIRED)
  add_executable(cereal-test-config-module main.cpp)
  target_link_libraries(cereal-test-config-module cereal::cereal)
  enable_testing()
  add_test(NAME test-cereal-test-config-module COMMAND cereal-test-config-module)
")

file(WRITE ${BINARY_DIR}/test_source/main.cpp "
  #include <cereal/archives/binary.hpp>
  #include <sstream>
  #include <cstdlib>
  struct MyData
  {
    int x = 0, y = 0, z = 0;
    void set() { x = 1; y = 2; z = 3; }
    bool is_set() const { return x == 1 && y == 2 && z == 3; }

    // This method lets cereal know which data members to serialize
    template<class Archive>
    void serialize(Archive & archive)
    {
      archive( x, y, z ); // serialize things by passing them to the archive
    }
  };
  int main()
  {
    std::stringstream ss; // any stream can be used

    {
      cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

      MyData m1, m2, m3;
      m1.set();
      m2.set();
      m3.set();
      oarchive(m1, m2, m3); // Write the data to the archive
    }

    {
      cereal::BinaryInputArchive iarchive(ss); // Create an input archive

      MyData m1, m2, m3;
      iarchive(m1, m2, m3); // Read the data from the archive

      return (m1.is_set() && m2.is_set() && m3.is_set())
      ? EXIT_SUCCESS : EXIT_FAILURE;
    }
  }"
)

file(MAKE_DIRECTORY ${BINARY_DIR}/test)
execute_process(
  COMMAND ${CMAKE_COMMAND}
    -DCMAKE_PREFIX_PATH=${INSTALL_DIR}
    ${BINARY_DIR}/test_source
  WORKING_DIRECTORY ${BINARY_DIR}/test
  RESULT_VARIABLE result
)
if(result)
  message(FATAL_ERROR "Test cmake configure-step failed")
endif()

# cmake install cereal
execute_process(
  COMMAND ${CMAKE_COMMAND}
    --build ${BINARY_DIR}/test
  RESULT_VARIABLE result
)
if(result)
  message(FATAL_ERROR "Test cmake build-step failed")
endif()

execute_process(
  COMMAND ${CMAKE_CTEST_COMMAND}
  WORKING_DIRECTORY ${BINARY_DIR}/test
  RESULT_VARIABLE result
)

if(result)
  message(FATAL_ERROR "Test run failed")
endif()
