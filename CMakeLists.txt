cmake_minimum_required(VERSION 3.6...3.15)

project(cereal LANGUAGES CXX VERSION 1.3.2)

set(CEREAL_MASTER_PROJECT OFF)
set(CEREAL_NOT_MASTER_PROJECT ON)
if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CEREAL_MASTER_PROJECT ON)
    set(CEREAL_NOT_MASTER_PROJECT OFF)
endif()


if(APPLE)
    option(SKIP_PORTABILITY_TEST "Skip portability (32 bit) tests" ON)
endif()

option(CEREAL_BUILD_DOC "Build documentation" ${CEREAL_MASTER_PROJECT})
option(CEREAL_BUILD_SANDBOX "Build sandbox examples" ${CEREAL_MASTER_PROJECT})
option(CEREAL_SKIP_PERFORMANCE_COMPARISON "Skip building performance sandbox comparison (requires boost)" ${CEREAL_NOT_MASTER_PROJECT})

# TODO: should not be needed! CK
if(NOT CMAKE_VERSION VERSION_LESS 3.0) # installing cereal requires INTERFACE lib
    option(CEREAL_JUST_INSTALL_CEREAL "Don't do anything besides installing the library" OFF)
endif()


set(CEREAL_THREAD_LIBS)
if(UNIX)
    option(CEREAL_THREAD_SAFE "Use mutexes to ensure thread safety" OFF)
    if(CEREAL_THREAD_SAFE)
        message(STATUS "Use mutexes")
        add_definitions(-DCEREAL_CEREAL_THREAD_SAFE=1)
        set(CEREAL_THREAD_LIBS pthread)
    endif()
endif()


if(MSVC)
    add_compile_options(/bigobj /W3 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Wshadow -Wold-style-cast)
    option(CEREAL_WITH_WERROR "Compile with '-Werror' C++ compiler flag" ON)
    if(CEREAL_WITH_WERROR)
        add_compile_options(-Werror)
    endif()

    option(CEREAL_CLANG_USE_LIBCPP "Use libc++ for clang compilation" OFF)
    if(APPLE OR CEREAL_CLANG_USE_LIBCPP)
        message(STATUS "Use libc++")
        add_compile_options(-stdlib=libc++)
        # TODO: use add_link_options(-stdlib=libc++ -lc++abi") bud this needs cmake 3.13! CK
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
    endif()

    if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD STREQUAL "98")
        set(CMAKE_CXX_STANDARD 11)
    endif()

    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()


add_library(cereal INTERFACE)
add_library(cereal::cereal ALIAS cereal)
target_include_directories(cereal INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
list(APPEND CEREAL_THREAD_LIBS cereal::cereal)

if(NOT CMAKE_VERSION VERSION_LESS 3.8)
    target_compile_features(cereal INTERFACE cxx_std_11)
endif()


option(CEREAL_INSTALL "Generate the install target" ${CEREAL_MASTER_PROJECT})
if(CEREAL_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS cereal EXPORT ${PROJECT_NAME}Targets)
    install(DIRECTORY include/cereal DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    set(configFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)
    set(versionFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
    set(configInstallDestination ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        ${configFile}
        INSTALL_DESTINATION ${configInstallDestination}
    )
    write_basic_package_version_file(
        ${versionFile}
        COMPATIBILITY SameMajorVersion
    )

    install(FILES ${configFile} ${versionFile} DESTINATION ${configInstallDestination})
    install(
        EXPORT ${PROJECT_NAME}Targets
        NAMESPACE "cereal::"
        DESTINATION ${configInstallDestination}
    )
endif()


if(CEREAL_JUST_INSTALL_CEREAL)
    return()
endif()


if(NOT CEREAL_SKIP_PERFORMANCE_COMPARISON)
    # Boost serialization for performance sandbox
    find_package(Boost REQUIRED COMPONENTS serialization)
endif()


option(CEREAL_BUILD_TESTS "Build tests" ${CEREAL_MASTER_PROJECT})
if(CEREAL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(unittests)
endif()


if(CEREAL_BUILD_SANDBOX)
    add_subdirectory(sandbox)
endif()


if(CEREAL_BUILD_DOC)
    add_subdirectory(doc)
endif()
