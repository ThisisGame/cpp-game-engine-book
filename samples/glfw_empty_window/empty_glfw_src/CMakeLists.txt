cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(/wd4819)#忽略保存为unicode警告

#拷贝depends目录
file(COPY "../../template/depends/glfw-3.3-3.4/" DESTINATION "../depends/glfw-3.3-3.4/")

include(CMakeLists.txt.glfw)

#头文件目录
include_directories("depends/glfw-3.3-3.4/include")

#源代码文件
add_executable(untitled main.cpp ${glfw_SOURCES})