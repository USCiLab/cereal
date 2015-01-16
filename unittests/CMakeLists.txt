file(GLOB TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

# A semi-colon separated list of test sources that should not be automatically built with boost unit test
set(SPECIAL_TESTS "portability_test.cpp" "deserialize_only" "multi_registration")

# Build the portability test only if we are on a 64-bit machine (void* is 8 bytes)
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8) 
  add_executable(portability_test32 portability_test.cpp)
  set_target_properties(portability_test32 PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(portability_test32 PROPERTIES LINK_FLAGS "-m32")

  add_executable(portability_test64 portability_test.cpp)

  add_test(NAME portability_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_portability_test.sh)
endif()

# Add the valgrind target
add_custom_target(valgrind
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_valgrind.sh)

# Add the coverage target
add_custom_target(coverage)
add_custom_command(TARGET coverage
  COMMAND ${CMAKE_SOURCE_DIR}/scripts/updatecoverage.sh ${CMAKE_SOURCE_DIR}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage)

# Build all of the non-special tests and link against the boost unit test framework.
# Add tests to coverage.
foreach(TEST_ENTRY ${TESTS})

  # Check to see if our target is listed in "SPECIAL_TESTS"
  list(FIND SPECIAL_TESTS "${TEST_ENTRY}" IS_SPECIAL_TEST)

  if(IS_SPECIAL_TEST EQUAL -1)
    
    set(TEST_SOURCE)
    set(TEST_SUITE_NAME)
    if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_ENTRY}")
      file(GLOB TEST_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${TEST_ENTRY}/*.cpp")
      set(TEST_SUITE_NAME "${TEST_ENTRY}")
    else()
      get_filename_component(EXTENSION "${TEST_ENTRY}" EXT)
      get_filename_component(NAME "${TEST_ENTRY}" NAME_WE)
      
      if ("${EXTENSION}" STREQUAL ".cpp")
        set(TEST_SOURCE "${TEST_ENTRY}")
        set(TEST_SUITE_NAME "${NAME}")
      endif()
    endif()
    
    if (NOT "${TEST_SOURCE}" STREQUAL "")
      # In a multi-file test module only one source file may contain the BOOST_TEST_MODULE definition if 
      # BOOST_TEST_DYN_LINK mode is specified.
      list(GET TEST_SOURCE 0 FIRST_SOURCE_FILE)
      set_source_files_properties(${FIRST_SOURCE_FILE} PROPERTIES COMPILE_FLAGS "-DBOOST_TEST_MODULE=${TEST_TARGET}")
      
      set(TEST_TARGET "test_${TEST_SUITE_NAME}")
      set(COVERAGE_TARGET "coverage_${TEST_SUITE_NAME}")
      
      # Add test target
      add_executable(${TEST_TARGET} ${TEST_SOURCE})
      set_target_properties(${TEST_TARGET} PROPERTIES COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK")
      target_link_libraries(${TEST_TARGET} ${Boost_LIBRARIES})
      add_test("${TEST_TARGET}" "${TEST_TARGET}")

      # TODO: This won't work right now, because we would need a 32-bit boost
      ## If we are on a 64-bit machine, create an extra 32-bit version of the test
      #if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
      #  add_executable(${TEST_TARGET}_32 ${ARGN})
      #  set_target_properties(${TEST_TARGET}_32 PROPERTIES 
      #    COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK;BOOST_TEST_MODULE=${TEST_TARGET}"
      #    COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
      #  target_link_libraries(${TEST_TARGET}_32 ${Boost_LIBRARIES})
      #  add_test("${TEST_TARGET}_32" "${TEST_TARGET}_32")
      #endif()
      
      # Add coverage target
      add_dependencies(coverage ${COVERAGE_TARGET})

      add_executable(${COVERAGE_TARGET} ${TEST_SOURCE})
      set_target_properties(${COVERAGE_TARGET} PROPERTIES COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK")
      set_target_properties(${COVERAGE_TARGET} PROPERTIES COMPILE_FLAGS "-coverage")
      set_target_properties(${COVERAGE_TARGET} PROPERTIES LINK_FLAGS "-coverage")
      set_target_properties(${COVERAGE_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/coverage")
      target_link_libraries(${COVERAGE_TARGET} ${Boost_LIBRARIES})
    endif()
  endif()
endforeach()

# Add some of the special tests.

# Adds a special test that consists of a static library and an executable.
function(add_test_with_lib TEST_NAME __LIB_SOURCE __TEST_SUITE_SOURCE)
  set(LIB_SOURCE ${${__LIB_SOURCE}})
  set(TEST_SUITE_SOURCE ${${__TEST_SUITE_SOURCE}})
  
  set(LIB_TARGET "lib_${TEST_NAME}")
  set(TEST_SUITE_TARGET "test_${TEST_NAME}")
  set(LIB_COVERAGE_TARGET "coverage_lib_${TEST_NAME}")
  set(TEST_SUITE_COVERAGE_TARGET "coverage_${TEST_NAME}")

  # In a multi-file test module only one source file may contain the BOOST_TEST_MODULE definition if 
  # BOOST_TEST_DYN_LINK mode is specified.
  list(GET TEST_SUITE_SOURCE 0 FIRST_SOURCE_FILE)
  set_source_files_properties(${FIRST_SOURCE_FILE} PROPERTIES COMPILE_FLAGS "-DBOOST_TEST_MODULE=${TEST_TARGET}")

  add_library(${LIB_TARGET} STATIC ${LIB_SOURCE})
  add_executable(${TEST_SUITE_TARGET} ${TEST_SUITE_SOURCE})
  set_target_properties(${TEST_SUITE_TARGET} PROPERTIES COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK")
  target_link_libraries(${TEST_SUITE_TARGET} ${LIB_TARGET} ${Boost_LIBRARIES})
  add_test("${TEST_SUITE_TARGET}" "${TEST_SUITE_TARGET}")

  add_dependencies(coverage ${TEST_SUITE_COVERAGE_TARGET})

  add_library(${LIB_COVERAGE_TARGET} STATIC ${LIB_SOURCE})
  add_executable(${TEST_SUITE_COVERAGE_TARGET} ${TEST_SUITE_SOURCE})
  set_target_properties(${TEST_SUITE_COVERAGE_TARGET} PROPERTIES COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK")
  set_target_properties(${TEST_SUITE_COVERAGE_TARGET} PROPERTIES COMPILE_FLAGS "-coverage")
  set_target_properties(${TEST_SUITE_COVERAGE_TARGET} PROPERTIES LINK_FLAGS "-coverage")
  set_target_properties(${TEST_SUITE_COVERAGE_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/coverage")
  target_link_libraries(${TEST_SUITE_COVERAGE_TARGET} ${LIB_COVERAGE_TARGET} ${Boost_LIBRARIES})
endfunction()

set(LIB_SOURCE "deserialize_only/helper.cpp" "deserialize_only/test_class.cpp")
set(SUITE_SOURCE "deserialize_only/test_suite.cpp")
add_test_with_lib(deserialize_only LIB_SOURCE SUITE_SOURCE)

set(LIB_SOURCE "multi_registration/common.cpp" "multi_registration/helper1.cpp" "multi_registration/helper2.cpp")
set(SUITE_SOURCE "multi_registration/test_suite1.cpp" "multi_registration/test_suite2.cpp")
add_test_with_lib(multi_registration LIB_SOURCE SUITE_SOURCE)

