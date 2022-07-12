cmake_minimum_required(VERSION 3.6...3.15)

project(cereal LANGUAGES CXX VERSION 1.3.2)

if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CEREAL_MASTER_PROJECT ON)
endif()

option(SKIP_PORTABILITY_TEST "Skip portability (32 bit) tests" OFF)
include(CheckCXXCompilerFlag)
set(CMAKE_REQUIRED_FLAGS "-m32")
check_cxx_compiler_flag("-m32" COMPILER_SUPPORT_M32)
unset(CMAKE_REQUIRED_FLAGS)
if(NOT COMPILER_SUPPORT_M32)
    set(SKIP_PORTABILITY_TEST ON CACHE BOOL "Skip portability (32 bit) tests" FORCE)
endif()

option(BUILD_DOC "Build documentation" ON)
option(BUILD_SANDBOX "Build sandbox examples" ON)
option(SKIP_PERFORMANCE_COMPARISON "Skip building performance sandbox comparison (requires boost)" OFF)

# TODO: should not be needed! CK
if(NOT CMAKE_VERSION VERSION_LESS 3.0) # installing cereal requires INTERFACE lib
    option(JUST_INSTALL_CEREAL "Don't do anything besides installing the library" OFF)
endif()


set(CEREAL_THREAD_LIBS)
if(UNIX)
    option(THREAD_SAFE "Use mutexes to ensure thread safety" OFF)
    if(THREAD_SAFE)
        message(STATUS "Use mutexes")
        add_definitions(-DCEREAL_THREAD_SAFE=1)
        set(CEREAL_THREAD_LIBS pthread)
    endif()
endif()


if(MSVC)
    add_compile_options(/bigobj /W3 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Wshadow -Wold-style-cast)
    option(WITH_WERROR "Compile with '-Werror' C++ compiler flag" ON)
    if(WITH_WERROR)
        add_compile_options(-Werror)
    endif()

    option(CLANG_USE_LIBCPP "Use libc++ for clang compilation" OFF)
    if(APPLE OR CLANG_USE_LIBCPP)
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
    if(${CMAKE_VERSION} VERSION_GREATER 3.13)
        write_basic_package_version_file("${versionFile}" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
    else()
        write_basic_package_version_file("${versionFile}" COMPATIBILITY SameMajorVersion)
    endif()

    install(FILES ${configFile} ${versionFile} DESTINATION ${configInstallDestination})
    install(
        EXPORT ${PROJECT_NAME}Targets
        NAMESPACE "cereal::"
        DESTINATION ${configInstallDestination}
    )
endif()


if(JUST_INSTALL_CEREAL)
    return()
endif()


if(NOT SKIP_PERFORMANCE_COMPARISON)
    # Boost serialization for performance sandbox
    find_package(Boost REQUIRED COMPONENTS serialization)
endif()


option(BUILD_TESTS "Build tests" ${CEREAL_MASTER_PROJECT})
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(unittests)
endif()


if(BUILD_SANDBOX)
    add_subdirectory(sandbox)
endif()


if(BUILD_DOC)
    add_subdirectory(doc)
endif()
