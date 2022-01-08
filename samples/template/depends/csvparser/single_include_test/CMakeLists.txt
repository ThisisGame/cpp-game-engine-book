# The purpose of this executable is to make sure it successfully compiles
add_executable(single_include_test "")
target_sources(single_include_test
    PRIVATE
        file1.cpp
        file2.cpp
)
target_link_libraries(single_include_test PRIVATE Threads::Threads)
