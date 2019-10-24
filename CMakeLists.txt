cmake_minimum_required (VERSION 2.6.2)
project (cereal)

option(SKIP_PORTABILITY_TEST "Skip portability (32 bit) tests" OFF)
option(SKIP_PERFORMANCE_COMPARISON "Skip building performance comparison (requires boost)" OFF)
if(NOT CMAKE_VERSION VERSION_LESS 3.0) # installing cereal requires INTERFACE lib
    option(JUST_INSTALL_CEREAL "Don't do anything besides installing the library" OFF)
endif()

option(THREAD_SAFE "Use mutexes to ensure thread safety" OFF)
if(THREAD_SAFE)
    add_definitions(-DCEREAL_THREAD_SAFE=1)
    set(CEREAL_THREAD_LIBS "pthread")
else()
    set(CEREAL_THREAD_LIBS "")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /W3 /WX")
else()
    set(CMAKE_CXX_FLAGS "-Wall -g -Wextra -Wshadow -pedantic -Wold-style-cast ${CMAKE_CXX_FLAGS}")
    option(WITH_WERROR "Compile with '-Werror' C++ compiler flag" ON)
    if(WITH_WERROR)
        set(CMAKE_CXX_FLAGS "-Werror ${CMAKE_CXX_FLAGS}")
    endif(WITH_WERROR)

    option(CLANG_USE_LIBCPP "Use libc++ for clang compilation" OFF)
    if(CLANG_USE_LIBCPP)
        set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
    endif()

    if(CMAKE_VERSION VERSION_LESS 3.1)
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    else()
      if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD STREQUAL "98")
        set(CMAKE_CXX_STANDARD 11)
      endif()

      if(CMAKE_CXX_STANDARD GREATER 14)
        cmake_minimum_required(VERSION 3.8)
      endif()

      set(CMAKE_CXX_STANDARD_REQUIRED ON)
    endif()

endif()

if(NOT CMAKE_VERSION VERSION_LESS 3.0)
    add_library(cereal INTERFACE)
    target_include_directories(cereal INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    install(TARGETS cereal EXPORT cereal
        DESTINATION lib) # ignored
    install(EXPORT cereal FILE cereal-config.cmake
        DESTINATION share/cmake/cereal)
    install(DIRECTORY include/cereal DESTINATION include)
endif()

if(JUST_INSTALL_CEREAL)
    return()
endif()

include_directories(./include)

if(NOT CMAKE_VERSION VERSION_LESS 3.12)
  cmake_policy(VERSION 3.12)
endif()

# Boost serialization for performance sandbox
find_package(Boost COMPONENTS serialization)

if(Boost_FOUND)
  include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
endif(Boost_FOUND)
  
enable_testing()
add_subdirectory(unittests)

add_subdirectory(sandbox)

add_subdirectory(doc)
