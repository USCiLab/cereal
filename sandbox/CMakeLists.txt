add_subdirectory(sandbox_shared_lib)

add_executable(sandbox sandbox.cpp)
add_executable(sandbox_json sandbox_json.cpp)
add_executable(sandbox_rtti sandbox_rtti.cpp)

add_executable(sandbox_vs sandbox_vs.cpp)
target_link_libraries(sandbox_vs sandbox_vs_dll)
include_directories(sandbox_shared_lib)

if((Boost_FOUND) AND NOT SKIP_PERFORMANCE_COMPARISON)
  add_executable(performance performance.cpp)
  if(MSVC)
    set_target_properties(performance PROPERTIES COMPILE_DEFINITIONS "BOOST_SERIALIZATION_DYN_LINK")
  endif()
  target_link_libraries(performance ${Boost_LIBRARIES})
endif()
