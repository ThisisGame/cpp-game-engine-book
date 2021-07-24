# For cppcodec itself, don't prefer system headers over development ones.
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

add_executable(helloworld helloworld.cpp)

add_executable(type_support_wrapper type_support_wrapper.cpp)
