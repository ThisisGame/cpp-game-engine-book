# Common variables.
CMAKE_MINIMUM_REQUIRED (VERSION 3.0)
if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif()
PROJECT (boolinq VERSION 3.0.0 LANGUAGES CXX)
INCLUDE(Dart)


SET (CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -ggdb3 -DDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")


# Boolinq source code.
SET (
    Boolinq_INCLUDES
    ${PROJECT_SOURCE_DIR}/boolinq/boolinq.h
)


# Static code analyse.
SET (CppCheck_REPORT ${PROJECT_BINARY_DIR}/cppcheck.report)
ADD_CUSTOM_COMMAND (
    OUTPUT  ${CppCheck_REPORT}
    COMMAND cppcheck ${Boolinq_INCLUDES} --enable=all --force --inconclusive &>cppcheck.report
)
ADD_CUSTOM_TARGET  (cppcheck DEPENDS ${CppCheck_REPORT})
SET_DIRECTORY_PROPERTIES (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CppCheck_REPORT})


# Testing.
ADD_SUBDIRECTORY (externals/googletest)
ADD_SUBDIRECTORY (externals/benchmark)
ADD_SUBDIRECTORY (test)
ADD_SUBDIRECTORY (bench)
ADD_DEPENDENCIES ("${PROJECT_NAME}-test" gtest)
ADD_DEPENDENCIES ("${PROJECT_NAME}-bench" benchmark)
