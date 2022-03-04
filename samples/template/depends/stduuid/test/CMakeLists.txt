# Test target
add_executable(test_${PROJECT_NAME} main.cpp test_generators.cpp test_uuid.cpp)
target_include_directories(test_${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/catch)
target_link_libraries(test_${PROJECT_NAME} PRIVATE ${PROJECT_NAME})
if (UUID_USING_CXX20_SPAN)
    set_target_properties(test_${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
else ()
    set_target_properties(test_${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
endif ()
if (WIN32)
    target_compile_options(test_${PROJECT_NAME} PRIVATE /EHc)
    target_compile_definitions(test_${PROJECT_NAME} PRIVATE _SCL_SECURE_NO_WARNINGS)
elseif (APPLE)
    target_compile_options(test_${PROJECT_NAME} PRIVATE -fexceptions -g -Wall)
else ()
    target_compile_options(test_${PROJECT_NAME} PRIVATE -fexceptions -g -Wall)
endif ()
get_target_property(CURRENT_COMPILE_OPTIONS test_${PROJECT_NAME} COMPILE_OPTIONS)
message(STATUS "** ${CMAKE_CXX_COMPILER_ID} flags: ${CURRENT_COMPILE_OPTIONS}")

# Tests
add_test(NAME "test_${PROJECT_NAME}" COMMAND "test_${PROJECT_NAME}" "-r compact")
set_tests_properties("test_${PROJECT_NAME}"
        PROPERTIES
        PASS_REGULAR_EXPRESSION "Passed all.*")
set_tests_properties("test_${PROJECT_NAME}"
        PROPERTIES
        FAIL_REGULAR_EXPRESSION "Failed \\d+ test cases")
set_tests_properties("test_${PROJECT_NAME}"
        PROPERTIES
        TIMEOUT 120)