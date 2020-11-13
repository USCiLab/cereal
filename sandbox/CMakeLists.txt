add_subdirectory(sandbox_shared_lib)

add_executable(sandbox sandbox.cpp)
target_link_libraries(sandbox ${CEREAL_THREAD_LIBS})

add_executable(sandbox_json sandbox_json.cpp)
target_link_libraries(sandbox_json ${CEREAL_THREAD_LIBS})

add_executable(sandbox_rtti sandbox_rtti.cpp)
target_link_libraries(sandbox_rtti ${CEREAL_THREAD_LIBS})

add_executable(sandbox_vs sandbox_vs.cpp)
target_link_libraries(sandbox_vs sandbox_vs_dll)

if(Boost_FOUND AND NOT SKIP_PERFORMANCE_COMPARISON)
  add_executable(performance performance.cpp)
  if(MSVC)
    set_target_properties(performance PROPERTIES COMPILE_DEFINITIONS "BOOST_SERIALIZATION_DYN_LINK")
  endif()
  target_include_directories(performance PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(performance ${CEREAL_THREAD_LIBS} ${Boost_LIBRARIES})
endif()
