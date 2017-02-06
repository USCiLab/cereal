file(GLOB TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# A semi-colon separated list of test sources that should not be automatically built with doctest 
set(SPECIAL_TESTS "portability_test.cpp")

if(CMAKE_VERSION VERSION_LESS 2.8)
  # Portability test uses the `TARGET_FILE_DIR` generator expression which is available from CMake 2.8.
  set(SKIP_PORTABILITY_TEST ON)
endif()

if(NOT SKIP_PORTABILITY_TEST)
  # Build the portability test only if we are on a 64-bit machine (void* is 8 bytes)
  if((${CMAKE_SIZEOF_VOID_P} EQUAL 8))
    if(NOT MSVC)
      add_executable(portability_test32 portability_test.cpp)
      set_target_properties(portability_test32 PROPERTIES COMPILE_FLAGS "-m32")
      set_target_properties(portability_test32 PROPERTIES LINK_FLAGS "-m32")
    endif()

    add_executable(portability_test64 portability_test.cpp)

    add_test(NAME portability_test
            COMMAND ${CMAKE_COMMAND}
              -DPORTABILITY_TEST_DIR=$<TARGET_FILE_DIR:portability_test64>
            -P "${CMAKE_CURRENT_SOURCE_DIR}/run_portability_test.cmake")

  elseif(MSVC)
    add_executable(portability_test32 portability_test.cpp)
  endif()
endif()

# Build all of the non-special tests
foreach(TEST_SOURCE ${TESTS})

  string(REPLACE ".cpp" "" TEST_TARGET "${TEST_SOURCE}")
  set(TEST_TARGET "test_${TEST_TARGET}")

  # Check to see if our target is listed in "SPECIAL_TESTS"
  list(FIND SPECIAL_TESTS "${TEST_SOURCE}" IS_SPECIAL_TEST)

  if(IS_SPECIAL_TEST EQUAL -1)

    add_executable(${TEST_TARGET} ${TEST_SOURCE})
    target_link_libraries(${TEST_TARGET} ${CEREAL_THREAD_LIBS})
    add_test("${TEST_TARGET}" "${TEST_TARGET}")

    # If we are on a 64-bit machine, create an extra 32-bit version of the test if portability testing is enabled
    if((NOT MSVC) AND (${CMAKE_SIZEOF_VOID_P} EQUAL 8) AND (NOT SKIP_PORTABILITY_TEST))
      add_executable(${TEST_TARGET}_32 ${TEST_SOURCE})
      set_target_properties(${TEST_TARGET}_32 PROPERTIES
        COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
      add_test("${TEST_TARGET}_32" "${TEST_TARGET}_32")
    endif()

  endif()

endforeach()

# Add the valgrind target
if(NOT MSVC)
  add_custom_target(valgrind
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/run_valgrind.sh")

  # Add the coverage target
  add_custom_target(coverage)
  add_custom_command(TARGET coverage
    COMMAND "${CMAKE_SOURCE_DIR}/scripts/updatecoverage.sh" ${CMAKE_SOURCE_DIR}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/coverage")

  # add tests to coverage
  foreach(TEST_SOURCE ${TESTS})
    string(REPLACE ".cpp" "" COVERAGE_TARGET "${TEST_SOURCE}")
    set(COVERAGE_TARGET "coverage_${COVERAGE_TARGET}")

    # Check to see if our target is listed in "SPECIAL_TESTS"
    list(FIND SPECIAL_TESTS "${TEST_SOURCE}" IS_SPECIAL_TEST)

    if(IS_SPECIAL_TEST EQUAL -1)
      add_dependencies(coverage ${COVERAGE_TARGET})

      add_executable(${COVERAGE_TARGET} EXCLUDE_FROM_ALL ${TEST_SOURCE})
      set_target_properties(${COVERAGE_TARGET} PROPERTIES COMPILE_FLAGS "-coverage")
      set_target_properties(${COVERAGE_TARGET} PROPERTIES LINK_FLAGS "-coverage")
      set_target_properties(${COVERAGE_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/coverage")
      target_link_libraries(${COVERAGE_TARGET} ${CEREAL_THREAD_LIBS})
    endif()
  endforeach()
endif(NOT MSVC)

if(NOT CMAKE_VERSION VERSION_LESS 3.0)
  add_test(test_cmake_config_module ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake-config-module.cmake)
endif()
