cmake_minimum_required(VERSION 3.17)
project(test)

set(CMAKE_CXX_STANDARD 14)

#拷贝depends目录
file(COPY "../../../template/depends/lua/" DESTINATION "../depends/lua/")

#头文件目录
include_directories("depends/lua/src")

file(GLOB_RECURSE SOURCES "depends/lua/src/*.c")

add_executable(test ${SOURCES} "main.cpp")