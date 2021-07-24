# For cppcodec itself, don't prefer system headers over development ones.
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(CATCH2 catch2)
endif()

if (CATCH2_FOUND)
    message(STATUS "Found system Catch2, not using bundled version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CATCH2_CFLAGS}")
else()
    message(STATUS "Did NOT find system Catch2, instead using bundled version")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/catch/single_include)
endif()

add_executable(test_cppcodec test_cppcodec.cpp)
add_test(cppcodec test_cppcodec)

add_executable(benchmark_cppcodec benchmark_cppcodec.cpp)

add_executable(minimal_decode minimal_decode.cpp)
