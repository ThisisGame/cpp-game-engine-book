# Common variables.
SET (CMAKE_CXX_STANDARD 11)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)
SET (TARGET "${PROJECT_NAME}-test" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wmissing-include-dirs -Wfloat-equal -Wshadow")
SET (TARGET "${PROJECT_NAME}-test" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion -Winit-self -Weffc++ ")
SET (TARGET "${PROJECT_NAME}-test" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wsign-promo")
SET (TARGET "${PROJECT_NAME}-test" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvla -Winvalid-pch -Winline -Wredundant-decls")
SET (TARGET "${PROJECT_NAME}-test" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
SET (TARGET "${PROJECT_NAME}-test" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual -Wpointer-arith")
SET (TARGET "${PROJECT_NAME}-test" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast")
SET (TARGET "${PROJECT_NAME}-test" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
SET (CMAKE_SHARED_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")


INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/boolinq)
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR}/googletest-src/googletest/include)

# Unit tests.
SET (
    BoolinqTest_SOURCES
    ${PROJECT_SOURCE_DIR}/test/AllTest.cpp
    ${PROJECT_SOURCE_DIR}/test/AnyTest.cpp
    ${PROJECT_SOURCE_DIR}/test/AvgTest.cpp
    ${PROJECT_SOURCE_DIR}/test/AppendTest.cpp
    ${PROJECT_SOURCE_DIR}/test/BitsTest.cpp
    ${PROJECT_SOURCE_DIR}/test/BytesTest.cpp
    ${PROJECT_SOURCE_DIR}/test/ConcatTest.cpp
    ${PROJECT_SOURCE_DIR}/test/ContainsTest.cpp
    ${PROJECT_SOURCE_DIR}/test/CountTest.cpp
    ${PROJECT_SOURCE_DIR}/test/DistinctTest.cpp
    ${PROJECT_SOURCE_DIR}/test/DotCallTest.cpp
    ${PROJECT_SOURCE_DIR}/test/ElementAtTest.cpp
    ${PROJECT_SOURCE_DIR}/test/FirstTest.cpp
    ${PROJECT_SOURCE_DIR}/test/ForEachTest.cpp
    ${PROJECT_SOURCE_DIR}/test/GroupByTest.cpp
    ${PROJECT_SOURCE_DIR}/test/CtorTest.cpp
    ${PROJECT_SOURCE_DIR}/test/LastTest.cpp
    ${PROJECT_SOURCE_DIR}/test/LinqTest.cpp
    ${PROJECT_SOURCE_DIR}/test/MaxTest.cpp
    ${PROJECT_SOURCE_DIR}/test/MinTest.cpp
    ${PROJECT_SOURCE_DIR}/test/OrderByTest.cpp
    ${PROJECT_SOURCE_DIR}/test/PrependTest.cpp
    ${PROJECT_SOURCE_DIR}/test/RangeTest.cpp
    ${PROJECT_SOURCE_DIR}/test/ReverseTest.cpp
    ${PROJECT_SOURCE_DIR}/test/SelectTest.cpp
    ${PROJECT_SOURCE_DIR}/test/SelectManyTest.cpp
    ${PROJECT_SOURCE_DIR}/test/SkipTest.cpp
    ${PROJECT_SOURCE_DIR}/test/SkipWhileTest.cpp
    ${PROJECT_SOURCE_DIR}/test/SumTest.cpp
    ${PROJECT_SOURCE_DIR}/test/TakeTest.cpp
    ${PROJECT_SOURCE_DIR}/test/TakeWhileTest.cpp
    ${PROJECT_SOURCE_DIR}/test/ToStdDequeTest.cpp
    ${PROJECT_SOURCE_DIR}/test/ToStdListTest.cpp
    ${PROJECT_SOURCE_DIR}/test/ToStdSetTest.cpp
    ${PROJECT_SOURCE_DIR}/test/ToStdVectorTest.cpp
    ${PROJECT_SOURCE_DIR}/test/UnbitsTest.cpp
    ${PROJECT_SOURCE_DIR}/test/UnbytesTest.cpp
    ${PROJECT_SOURCE_DIR}/test/WhereTest.cpp
)
ADD_EXECUTABLE (
    "${PROJECT_NAME}-test"
    ${BoolinqTest_SOURCES}
)
TARGET_LINK_LIBRARIES (
    "${PROJECT_NAME}-test"
    gtest_main
    gtest
    #gcov
)
ENABLE_TESTING ()
ADD_TEST (BoolinqTest "${PROJECT_NAME}-test")


# Test coverage report.
SET (Coverage_REPORT ${PROJECT_BINARY_DIR}/coverage.info)
SET (Coverage_DIR    ${PROJECT_BINARY_DIR}/coverage)
ADD_CUSTOM_COMMAND (
    OUTPUT  ${Coverage_REPORT}
    COMMAND lcov -q -c -f -b . -d ${PROJECT_BINARY_DIR}/boolinq -o ${Coverage_REPORT}
    COMMAND lcov -e ${Coverage_REPORT} '${PROJECT_SOURCE_DIR}/boolinq/*' -o ${Coverage_REPORT}
    COMMAND genhtml ${Coverage_REPORT} --legend --demangle-cpp -f -q -o ${Coverage_DIR}
    DEPENDS "${PROJECT_NAME}-test"
)
ADD_CUSTOM_TARGET (coverage DEPENDS ${Coverage_REPORT})
# FIXME: Doesn't work correctly (require explicit call cmake when files appear).
FILE (GLOB_RECURSE Coverage_GCNO ${PROJECT_BINARY_DIR}/*.gcno)
FILE (GLOB_RECURSE Coverage_GCDA ${PROJECT_BINARY_DIR}/*.gcda)
LIST (APPEND Coverage_DATA "${Coverage_REPORT}")
LIST (APPEND Coverage_DATA "${Coverage_DIR}")
LIST (APPEND Coverage_DATA "${Coverage_GCNO}")
LIST (APPEND Coverage_DATA "${Coverage_GCDA}")
SET_DIRECTORY_PROPERTIES (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${Coverage_DATA}")
