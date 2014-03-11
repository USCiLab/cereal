file(GLOB TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# A semi-colon separated list of test sources that should not be automatically built with boost unit test
set(SPECIAL_TESTS "portability_test.cpp")

# Build the portability test only if we are on a 64-bit machine (void* is 8 bytes)
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8) 
  add_executable(portability_test32 portability_test.cpp)
  set_target_properties(portability_test32 PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(portability_test32 PROPERTIES LINK_FLAGS "-m32")

  add_executable(portability_test64 portability_test.cpp)

  add_test(NAME portability_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_portability_test.sh)

endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)

# Build all of the non-special tests and link against the boost unit test framework
foreach(TEST_SOURCE ${TESTS})

  string(REPLACE ".cpp" "" TEST_TARGET "${TEST_SOURCE}")
  set(TEST_TARGET "test_${TEST_TARGET}")

  # Check to see if our target is listed in "SPECIAL_TESTS"
  list(FIND SPECIAL_TESTS "${TEST_SOURCE}" IS_SPECIAL_TEST)

  if(IS_SPECIAL_TEST EQUAL -1)

    add_executable(${TEST_TARGET} ${TEST_SOURCE})
    set_target_properties(${TEST_TARGET} PROPERTIES COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK;BOOST_TEST_MODULE=${TEST_TARGET}")
    target_link_libraries(${TEST_TARGET} ${Boost_LIBRARIES})
    add_test("${TEST_TARGET}" "${TEST_TARGET}")

  endif(IS_SPECIAL_TEST EQUAL -1)

endforeach()

