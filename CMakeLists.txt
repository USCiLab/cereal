cmake_minimum_required (VERSION 2.6.2)
project (cereal)

option(SKIP_PORTABILITY_TEST "Skip portability tests" OFF)
if(NOT CMAKE_VERSION VERSION_LESS 3.0) # installing cereal requires INTERFACE lib
    option(JUST_INSTALL_CEREAL "Don't do anything besides installing the library" OFF)
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-Wall -Werror -g -Wextra -Wshadow -pedantic ${CMAKE_CXX_FLAGS}")
    if(CMAKE_VERSION VERSION_LESS 3.1)
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    else()
        if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD STREQUAL "98")
            set(CMAKE_CXX_STANDARD 11)
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

find_package(Boost COMPONENTS serialization unit_test_framework)

if(Boost_FOUND)
  include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
  enable_testing()
  add_subdirectory(unittests)
endif(Boost_FOUND)

add_subdirectory(sandbox)

find_package(Doxygen)
if(DOXYGEN_FOUND)

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.in" "${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg" @ONLY)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/updatedoc.in" "${CMAKE_CURRENT_BINARY_DIR}/updatedoc.sh" @ONLY)
  add_custom_target(update-doc
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/updatedoc.sh"
    DEPENDS doc
    COMMENT "Copying documentation to gh-pages branch" VERBATIM
    )

endif(DOXYGEN_FOUND)
