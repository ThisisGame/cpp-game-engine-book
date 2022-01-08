cmake_minimum_required (VERSION 3.8)

if (DEFINED PROJECT_NAME)
    set (BITFLAGS_SUBPROJECT ON)
endif ()

set (BITFLAGS_VERSION_MAJOR 1)
set (BITFLAGS_VERSION_MINOR 4)
set (BITFLAGS_VERSION_PATCH 1)

set (BITFLAGS_VERSION "${BITFLAGS_VERSION_MAJOR}.${BITFLAGS_VERSION_MINOR}.${BITFLAGS_VERSION_PATCH}")

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.12")
    project (
        bitflags
            VERSION ${BITFLAGS_VERSION}
            LANGUAGES CXX
            HOMEPAGE_URL "https://github.com/m-peko/bitflags"
            DESCRIPTION ""
    )
elseif (CMAKE_VERSION VERSION_GREATER_EQUAL "3.9")
    project (
        bitflags
            VERSION ${BITFLAGS_VERSION}
            LANGUAGES CXX
            DESCRIPTION ""
    )
else ()
    project (
        bitflags
            VERSION ${BITFLAGS_VERSION}
            LANGUAGES CXX
    )
endif ()

if (BITFLAGS_CPP_VERSION EQUAL 17)
    message (STATUS "Using C++17")
elseif (BITFLAGS_CPP_VERSION EQUAL 14)
    message (STATUS "Using C++14")
elseif (BITFLAGS_CPP_VERSION EQUAL 11)
    message (STATUS "Using C++11")
else ()
    message (STATUS "Using default C++17")
    set (BITFLAGS_CPP_VERSION 17)
endif ()

set (CMAKE_CXX_STANDARD ${BITFLAGS_CPP_VERSION})
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

# Compilation flags
if (MSVC)
    # Don't always use Wall, since VC's /Wall is too verbose.
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /O2")
else ()
    option (BITFLAGS_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
    if (BITFLAGS_ENABLE_COVERAGE)
        message (STATUS "Code coverage reporting is enabled.")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O0 --coverage")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O3")
    endif ()
endif ()

option (BITFLAGS_BUILD_TESTS "Build tests" ON)
option (BITFLAGS_BUILD_SAMPLES "Build examples" ON)
option (BITFLAGS_BUILD_BENCHMARK "Build benchmark" OFF)

include (GNUInstallDirs)

add_library (bitflags INTERFACE)
add_library (bitflags::bitflags ALIAS bitflags)

target_compile_features (bitflags INTERFACE cxx_std_${BITFLAGS_CPP_VERSION})
target_include_directories (
    bitflags INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

if (BITFLAGS_BUILD_TESTS)
    # Only include googletest if the git submodule has been fetched
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
        # Enable tests and add the test directory
        message (STATUS "Tests have been enabled")
        set (GOOGLETEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
        set (GOOGLETEST_INCLUDE ${GOOGLETEST_ROOT}/googletest/include)
        set (GOOGLETEST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest)
        set (GOOGLETEST_LIBRARY ${GOOGLETEST_BINARY_DIR}/googletest)

        set (gtest_force_shared_crt ON CACHE BOOL "" FORCE)

        enable_testing ()
        add_subdirectory (googletest)
        add_subdirectory (tests)
    else ()
        message (STATUS "googletest git submodule is absent. Run `git submodule init && git submodule update` to get it")
    endif ()
endif ()

if (BITFLAGS_BUILD_SAMPLES)
    add_subdirectory (samples)
endif ()

if (BITFLAGS_BUILD_BENCHMARK)
    # Only include googlebench if the git submodule has been fetched
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googlebench/CMakeLists.txt")
        # Enable benchmark and add the benchmark directory
        message (STATUS "Benchmark has been enabled")
        set (GOOGLEBENCH_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/googlebench)
        set (GOOGLEBENCH_INCLUDE ${GOOGLEBENCH_ROOT}/googlebench/include)
        set (GOOGLEBENCH_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/googlebench)
        set (GOOGLEBENCH_LIBRARY ${GOOGLEBENCH_BINARY_DIR}/googlebench)

        set (BENCHMARK_ENABLE_GTEST_TESTS OFF)

        if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
            add_definitions(-DNDEBUG)
        endif()

        add_subdirectory (googlebench)
        add_subdirectory (benchmark)
    else ()
        message (STATUS "googlebench git submodule is absent. Run `git submodule init && git submodule update` to get it")
    endif ()
endif ()

if (NOT BITFLAGS_SUBPROJECT)
    include (CMakePackageConfigHelpers)

    install (TARGETS bitflags EXPORT bitflags-config)

    write_basic_package_version_file (
        bitflags-config-version.cmake
        VERSION ${BITFLAGS_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    install (
        EXPORT bitflags-config
        FILE bitflags-config.cmake
        NAMESPACE bitflags::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bitflags
    )

    install (
        FILES ${CMAKE_CURRENT_BINARY_DIR}/bitflags-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bitflags
    )

    install (
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/bitflags
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        USE_SOURCE_PERMISSIONS
        PATTERN "*.hpp"
    )
endif ()