cmake_minimum_required(VERSION 3.6...3.15)

project(cereal LANGUAGES CXX)

if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CEREAL_MASTER_PROJECT ON)
endif()


if(APPLE)
    option(SKIP_PORTABILITY_TEST "Skip portability (32 bit) tests" ON)
endif()
option(SKIP_PERFORMANCE_COMPARISON "Skip building performance comparison (requires boost)" OFF)

if(NOT CMAKE_VERSION VERSION_LESS 3.0) # installing cereal requires INTERFACE lib
    option(JUST_INSTALL_CEREAL "Don't do anything besides installing the library" OFF)
endif()


if(UNIX)
    option(THREAD_SAFE "Use mutexes to ensure thread safety" OFF)
    if(THREAD_SAFE)
        message(WARNING "use mutexes")
        add_definitions(-DCEREAL_THREAD_SAFE=1)
        set(CEREAL_THREAD_LIBS "pthread")
    else()
        set(CEREAL_THREAD_LIBS "")
    endif()
endif()


if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /W3 /WX")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wshadow -Wold-style-cast ${CMAKE_CXX_FLAGS}")
    option(WITH_WERROR "Compile with '-Werror' C++ compiler flag" ON)
    if(WITH_WERROR)
        set(CMAKE_CXX_FLAGS "-Werror ${CMAKE_CXX_FLAGS}")
    endif()

    option(CLANG_USE_LIBCPP "Use libc++ for clang compilation" OFF)
    if(APPLE OR CLANG_USE_LIBCPP)
        message(WARNING "use libc++")
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
    add_library(cereal::cereal ALIAS cereal)
    target_include_directories(cereal INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    list(APPEND CEREAL_THREAD_LIBS "cereal::cereal")

    option(CEREAL_INSTALL "Generate the install target" ${CEREAL_MASTER_PROJECT})
    if(CEREAL_INSTALL)
        install(TARGETS cereal EXPORT cereal
            DESTINATION lib) # ignored
        install(EXPORT cereal FILE cereal-config.cmake
            NAMESPACE "cereal::"
            DESTINATION lib/cmake/cereal)
        install(DIRECTORY include/cereal DESTINATION include)
    endif()
endif()


if(JUST_INSTALL_CEREAL)
    return()
endif()

# TODO: should not be needed! CK
include_directories(SYSTEM ./include)

if(NOT CMAKE_VERSION VERSION_LESS 3.12)
    cmake_policy(VERSION 3.12)
endif()

if(NOT SKIP_PERFORMANCE_COMPARISON)
    # Boost serialization for performance sandbox
    find_package(Boost REQUIRED COMPONENTS serialization)

    # TODO: should not be needed! CK
    if(Boost_FOUND)
        include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
    endif()
endif()


option(BUILD_TESTS "Build tests" ${CEREAL_MASTER_PROJECT})
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(unittests)
endif()

add_subdirectory(sandbox)

add_subdirectory(doc)
