cmake_minimum_required(VERSION 3.17)
project(testrttr)

set(CMAKE_CXX_STANDARD 17)

#拷贝depends目录
file(COPY "../../template/depends/rttr-0.9.6/src/rttr/" DESTINATION "../depends/rttr/")

include_directories("depends")

file(GLOB_RECURSE rttr_cpp "depends/rttr/*.cpp")

add_executable(testrttr main.cpp ${rttr_cpp})