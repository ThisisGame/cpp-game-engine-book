cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

#头文件目录
include_directories("depends/glfw-3.3.3.bin.WIN64/include")

#链接文件目录
link_directories("depends/glfw-3.3.3.bin.WIN64/lib-static-ucrt")

#链接库
link_libraries(opengl32 glfw3dll)

#源代码文件
add_executable(untitled main.cpp)