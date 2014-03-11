file(GLOB TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(TEST_SOURCE ${TESTS})

  string(REPLACE ".cpp" "" TEST_TARGET "${TEST_SOURCE}")
  set(TEST_TARGET "test_${TEST_TARGET}")

  add_executable(${TEST_TARGET} ${TEST_SOURCE})
  set_target_properties(${TEST_TARGET} PROPERTIES COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK;BOOST_TEST_MODULE=${TEST_TARGET}")
  target_link_libraries(${TEST_TARGET} ${Boost_LIBRARIES})
  add_test("${TEST_TARGET}" "${TEST_TARGET}")

endforeach()
